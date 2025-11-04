// ipc_msgqueue_server.c
// Server using POSIX Message Queue for IPC (local inter-process communication)
// This demonstrates "true IPC" using message queues instead of network sockets

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#define SERVER_QUEUE_NAME "/chat_server_mq"
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 8192
#define QUEUE_PERMISSIONS 0660

typedef struct {
    char client_queue[64];  // Client's queue name for responses
    char username[32];
    int active;
    pthread_t tid;
} client_info_t;

client_info_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
mqd_t server_mq;
int running = 1;

// Timestamp helper
void timestamp(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &tm);
}

// Log to file
void log_msg(const char *fmt, ...) {
    char timebuf[64];
    timestamp(timebuf, sizeof(timebuf));
    FILE *fp = fopen("server_log_mq.txt", "a");
    if (!fp) return;
    fprintf(fp, "[%s] ", timebuf);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fclose(fp);
}

// Add client
int add_client(const char *queue_name, const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            strncpy(clients[i].client_queue, queue_name, sizeof(clients[i].client_queue) - 1);
            strncpy(clients[i].username, username, sizeof(clients[i].username) - 1);
            clients[i].active = 1;
            pthread_mutex_unlock(&clients_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

// Remove client
void remove_client(const char *queue_name) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].client_queue, queue_name) == 0) {
            clients[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Broadcast message to all clients
void broadcast(const char *msg, const char *exclude_queue) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && 
            (exclude_queue == NULL || strcmp(clients[i].client_queue, exclude_queue) != 0)) {
            mqd_t client_mq = mq_open(clients[i].client_queue, O_WRONLY | O_NONBLOCK);
            if (client_mq != (mqd_t)-1) {
                mq_send(client_mq, msg, strlen(msg) + 1, 0);
                mq_close(client_mq);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Send to specific client
int send_to_client(const char *queue_name, const char *msg) {
    mqd_t client_mq = mq_open(queue_name, O_WRONLY | O_NONBLOCK);
    if (client_mq == (mqd_t)-1) return -1;
    int ret = mq_send(client_mq, msg, strlen(msg) + 1, 0);
    mq_close(client_mq);
    return ret;
}

// Handle LIST command
void handle_list(const char *client_queue) {
    char response[MAX_MSG_SIZE];
    int off = snprintf(response, sizeof(response), "Connected clients:\n");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            off += snprintf(response + off, sizeof(response) - off, " - %s\n", clients[i].username);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    send_to_client(client_queue, response);
}

// Handle TIME command
void handle_time(const char *client_queue) {
    char timebuf[128], response[256];
    timestamp(timebuf, sizeof(timebuf));
    snprintf(response, sizeof(response), "Server time: %s\n", timebuf);
    send_to_client(client_queue, response);
}

// Process message
void process_message(char *msg) {
    // Format: "QUEUE_NAME:USERNAME:MESSAGE"
    char *queue_name = strtok(msg, ":");
    char *username = strtok(NULL, ":");
    char *content = strtok(NULL, "");
    
    if (!queue_name || !username || !content) return;
    
    printf("[%s] %s: %s\n", queue_name, username, content);
    
    // Handle REGISTER
    if (strcmp(content, "REGISTER") == 0) {
        int idx = add_client(queue_name, username);
        if (idx >= 0) {
            log_msg("%s (%s) connected", username, queue_name);
            send_to_client(queue_name, "AUTH OK");
            char joinmsg[256];
            snprintf(joinmsg, sizeof(joinmsg), "%s has joined the chat\n", username);
            broadcast(joinmsg, queue_name);
        } else {
            send_to_client(queue_name, "AUTH FAIL: Server full");
        }
        return;
    }
    
    // Handle DISCONNECT
    if (strcmp(content, "DISCONNECT") == 0) {
        remove_client(queue_name);
        log_msg("%s (%s) disconnected", username, queue_name);
        char leavemsg[256];
        snprintf(leavemsg, sizeof(leavemsg), "%s has left the chat\n", username);
        broadcast(leavemsg, NULL);
        return;
    }
    
    // Handle commands
    if (content[0] == '/') {
        if (strcmp(content, "/LIST") == 0) {
            handle_list(queue_name);
        } else if (strcmp(content, "/TIME") == 0) {
            handle_time(queue_name);
        } else if (strcmp(content, "/EXIT") == 0) {
            send_to_client(queue_name, "Goodbye");
            remove_client(queue_name);
            char leavemsg[256];
            snprintf(leavemsg, sizeof(leavemsg), "%s has left the chat\n", username);
            broadcast(leavemsg, NULL);
        } else {
            send_to_client(queue_name, "Unknown command");
        }
    } else {
        // Regular message: broadcast
        char broadcast_msg[MAX_MSG_SIZE];
        snprintf(broadcast_msg, sizeof(broadcast_msg), "%s: %s\n", username, content);
        broadcast(broadcast_msg, NULL);
        log_msg("%s: %s", username, content);
    }
}

void cleanup(int sig) {
    printf("\nShutting down server...\n");
    running = 0;
    mq_close(server_mq);
    mq_unlink(SERVER_QUEUE_NAME);
    exit(0);
}

int main() {
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    
    // Setup message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    // Remove old queue if exists
    mq_unlink(SERVER_QUEUE_NAME);
    
    // Create server message queue
    server_mq = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if (server_mq == (mqd_t)-1) {
        perror("mq_open server");
        exit(1);
    }
    
    printf("Message Queue Server started\n");
    printf("Queue name: %s\n", SERVER_QUEUE_NAME);
    printf("Waiting for messages...\n");
    
    memset(clients, 0, sizeof(clients));
    
    // Main loop: receive and process messages
    while (running) {
        ssize_t bytes_read = mq_receive(server_mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            process_message(buffer);
        } else if (errno != EINTR) {
            perror("mq_receive");
        }
    }
    
    cleanup(0);
    return 0;
}
