// ipc_msgqueue_client.c
// Client using POSIX Message Queue for IPC
// Communicates with server on the same machine using message queues

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

#define SERVER_QUEUE_NAME "/chat_server_mq"
#define MAX_MSG_SIZE 8192
#define QUEUE_PERMISSIONS 0660

char client_queue_name[64];
char username[32];
mqd_t client_mq;
mqd_t server_mq;
int running = 1;

// Reader thread: receives messages from server
void *reader_thread(void *arg) {
    char buffer[MAX_MSG_SIZE];
    
    while (running) {
        ssize_t bytes_read = mq_receive(client_mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
            fflush(stdout);
            
            // Check for disconnect
            if (strcmp(buffer, "Goodbye") == 0) {
                running = 0;
                break;
            }
        } else if (errno != EINTR && running) {
            perror("mq_receive");
        }
    }
    return NULL;
}

// Send message to server
int send_to_server(const char *msg) {
    char full_msg[MAX_MSG_SIZE];
    snprintf(full_msg, sizeof(full_msg), "%s:%s:%s", client_queue_name, username, msg);
    
    if (mq_send(server_mq, full_msg, strlen(full_msg) + 1, 0) == -1) {
        perror("mq_send");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct mq_attr attr;
    char input[MAX_MSG_SIZE];
    pthread_t rt;
    
    // Generate unique client queue name
    snprintf(client_queue_name, sizeof(client_queue_name), "/chat_client_%d", getpid());
    
    printf("=== Message Queue Chat Client ===\n");
    printf("Username: ");
    fflush(stdout);
    if (fgets(username, sizeof(username), stdin) == NULL) return 1;
    username[strcspn(username, "\r\n")] = '\0';
    
    // Setup client queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    // Create client message queue
    mq_unlink(client_queue_name);
    client_mq = mq_open(client_queue_name, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
    if (client_mq == (mqd_t)-1) {
        perror("mq_open client");
        exit(1);
    }
    
    // Open server message queue
    server_mq = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_mq == (mqd_t)-1) {
        perror("mq_open server (Is server running?)");
        mq_close(client_mq);
        mq_unlink(client_queue_name);
        exit(1);
    }
    
    printf("Connected to server via message queue\n");
    
    // Register with server
    if (send_to_server("REGISTER") != 0) {
        fprintf(stderr, "Failed to register\n");
        goto cleanup;
    }
    
    // Start reader thread
    if (pthread_create(&rt, NULL, reader_thread, NULL) != 0) {
        perror("pthread_create");
        goto cleanup;
    }
    pthread_detach(rt);
    
    printf("Type messages or commands (/LIST, /TIME, /EXIT):\n");
    
    // Main input loop
    while (running && fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\r\n")] = '\0';
        if (strlen(input) == 0) continue;
        
        if (send_to_server(input) != 0) {
            fprintf(stderr, "Failed to send message\n");
            break;
        }
        
        if (strcmp(input, "/EXIT") == 0) {
            sleep(1); // Wait for server response
            break;
        }
    }
    
cleanup:
    running = 0;
    send_to_server("DISCONNECT");
    sleep(1);
    mq_close(client_mq);
    mq_close(server_mq);
    mq_unlink(client_queue_name);
    
    printf("Disconnected\n");
    return 0;
}
