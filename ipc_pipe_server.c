// ipc_pipe_server.c
// Server using Named Pipes (FIFOs) for IPC
// Demonstrates bidirectional communication using named pipes

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#define SERVER_FIFO "/tmp/chat_server_fifo"
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 4096

typedef struct {
    char client_fifo[128];
    char username[32];
    int active;
} client_info_t;

client_info_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int running = 1;

void timestamp(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &tm);
}

void log_msg(const char *fmt, ...) {
    char timebuf[64];
    timestamp(timebuf, sizeof(timebuf));
    FILE *fp = fopen("server_log_pipe.txt", "a");
    if (!fp) return;
    fprintf(fp, "[%s] ", timebuf);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fclose(fp);
}

int add_client(const char *fifo_name, const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            strncpy(clients[i].client_fifo, fifo_name, sizeof(clients[i].client_fifo) - 1);
            strncpy(clients[i].username, username, sizeof(clients[i].username) - 1);
            clients[i].active = 1;
            pthread_mutex_unlock(&clients_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

void remove_client(const char *fifo_name) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].client_fifo, fifo_name) == 0) {
            clients[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast(const char *msg, const char *exclude_fifo) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && 
            (exclude_fifo == NULL || strcmp(clients[i].client_fifo, exclude_fifo) != 0)) {
            int fd = open(clients[i].client_fifo, O_WRONLY | O_NONBLOCK);
            if (fd != -1) {
                write(fd, msg, strlen(msg));
                close(fd);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int send_to_client(const char *fifo_name, const char *msg) {
    int fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
    if (fd == -1) return -1;
    write(fd, msg, strlen(msg));
    close(fd);
    return 0;
}

void handle_list(const char *client_fifo) {
    char response[MAX_MSG_SIZE];
    int off = snprintf(response, sizeof(response), "Connected clients:\n");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            off += snprintf(response + off, sizeof(response) - off, " - %s\n", clients[i].username);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    send_to_client(client_fifo, response);
}

void handle_time(const char *client_fifo) {
    char timebuf[128], response[256];
    timestamp(timebuf, sizeof(timebuf));
    snprintf(response, sizeof(response), "Server time: %s\n", timebuf);
    send_to_client(client_fifo, response);
}

void process_message(char *msg) {
    // Format: "FIFO_NAME:USERNAME:MESSAGE"
    char *fifo_name = strtok(msg, ":");
    char *username = strtok(NULL, ":");
    char *content = strtok(NULL, "");
    
    if (!fifo_name || !username || !content) return;
    
    printf("[%s] %s: %s\n", fifo_name, username, content);
    
    if (strcmp(content, "REGISTER") == 0) {
        int idx = add_client(fifo_name, username);
        if (idx >= 0) {
            log_msg("%s (%s) connected", username, fifo_name);
            send_to_client(fifo_name, "AUTH OK\n");
            char joinmsg[256];
            snprintf(joinmsg, sizeof(joinmsg), "%s has joined the chat\n", username);
            broadcast(joinmsg, fifo_name);
        } else {
            send_to_client(fifo_name, "AUTH FAIL: Server full\n");
        }
        return;
    }
    
    if (strcmp(content, "DISCONNECT") == 0) {
        remove_client(fifo_name);
        log_msg("%s (%s) disconnected", username, fifo_name);
        char leavemsg[256];
        snprintf(leavemsg, sizeof(leavemsg), "%s has left the chat\n", username);
        broadcast(leavemsg, NULL);
        return;
    }
    
    if (content[0] == '/') {
        if (strcmp(content, "/LIST") == 0) {
            handle_list(fifo_name);
        } else if (strcmp(content, "/TIME") == 0) {
            handle_time(fifo_name);
        } else if (strcmp(content, "/EXIT") == 0) {
            send_to_client(fifo_name, "Goodbye\n");
            remove_client(fifo_name);
            char leavemsg[256];
            snprintf(leavemsg, sizeof(leavemsg), "%s has left the chat\n", username);
            broadcast(leavemsg, NULL);
        } else {
            send_to_client(fifo_name, "Unknown command\n");
        }
    } else {
        char broadcast_msg[MAX_MSG_SIZE];
        snprintf(broadcast_msg, sizeof(broadcast_msg), "%s: %s\n", username, content);
        broadcast(broadcast_msg, NULL);
        log_msg("%s: %s", username, content);
    }
}

void cleanup(int sig) {
    printf("\nShutting down server...\n");
    running = 0;
    unlink(SERVER_FIFO);
    exit(0);
}

int main() {
    char buffer[MAX_MSG_SIZE];
    
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    unlink(SERVER_FIFO);
    
    if (mkfifo(SERVER_FIFO, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }
    
    printf("Named Pipe Server started\n");
    printf("FIFO: %s\n", SERVER_FIFO);
    printf("Waiting for messages...\n");
    
    memset(clients, 0, sizeof(clients));
    
    while (running) {
        int fd = open(SERVER_FIFO, O_RDONLY);
        if (fd == -1) {
            perror("open server fifo");
            break;
        }
        
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            process_message(buffer);
        }
        
        close(fd);
    }
    
    cleanup(0);
    return 0;
}
