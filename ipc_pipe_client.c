// ipc_pipe_client.c
// Client using Named Pipes (FIFOs) for IPC

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/chat_server_fifo"
#define MAX_MSG_SIZE 4096

char client_fifo_name[128];
char username[32];
int running = 1;

void *reader_thread(void *arg) {
    char buffer[MAX_MSG_SIZE];
    int fd;
    
    while (running) {
        fd = open(client_fifo_name, O_RDONLY);
        if (fd == -1) {
            if (running) perror("open client fifo");
            break;
        }
        
        ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("%s", buffer);
            fflush(stdout);
            
            if (strstr(buffer, "Goodbye")) {
                running = 0;
                close(fd);
                break;
            }
        }
        
        close(fd);
    }
    return NULL;
}

int send_to_server(const char *msg) {
    char full_msg[MAX_MSG_SIZE];
    snprintf(full_msg, sizeof(full_msg), "%s:%s:%s", client_fifo_name, username, msg);
    
    int fd = open(SERVER_FIFO, O_WRONLY);
    if (fd == -1) {
        perror("open server fifo");
        return -1;
    }
    
    write(fd, full_msg, strlen(full_msg) + 1);
    close(fd);
    return 0;
}

int main() {
    char input[MAX_MSG_SIZE];
    pthread_t rt;
    
    snprintf(client_fifo_name, sizeof(client_fifo_name), "/tmp/chat_client_%d", getpid());
    
    printf("=== Named Pipe Chat Client ===\n");
    printf("Username: ");
    fflush(stdout);
    if (fgets(username, sizeof(username), stdin) == NULL) return 1;
    username[strcspn(username, "\r\n")] = '\0';
    
    unlink(client_fifo_name);
    if (mkfifo(client_fifo_name, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }
    
    printf("Connected to server via named pipe\n");
    
    if (send_to_server("REGISTER") != 0) {
        fprintf(stderr, "Failed to register\n");
        goto cleanup;
    }
    
    if (pthread_create(&rt, NULL, reader_thread, NULL) != 0) {
        perror("pthread_create");
        goto cleanup;
    }
    pthread_detach(rt);
    
    printf("Type messages or commands (/LIST, /TIME, /EXIT):\n");
    
    while (running && fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\r\n")] = '\0';
        if (strlen(input) == 0) continue;
        
        if (send_to_server(input) != 0) {
            fprintf(stderr, "Failed to send message\n");
            break;
        }
        
        if (strcmp(input, "/EXIT") == 0) {
            sleep(1);
            break;
        }
    }
    
cleanup:
    running = 0;
    send_to_server("DISCONNECT");
    sleep(1);
    unlink(client_fifo_name);
    
    printf("Disconnected\n");
    return 0;
}
