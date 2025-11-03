// socket_init.c
// Server socket initialization and main accept loop

#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

// Forward declaration of client thread handler (defined in server_thread.c)
void *client_thread(void *arg);

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in addr;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }
    
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    
    return server_fd;
}

void run_server(int server_fd) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    
    printf("Server listening on port %d\n", SERVER_PORT);
    
    while (1) {
        int new_sock = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        if (new_sock == -1) {
            perror("accept");
            continue;
        }
        
        char addrstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, addrstr, sizeof(addrstr));
        printf("Incoming connection from %s:%d (sock=%d)\n", 
               addrstr, ntohs(addr.sin_port), new_sock);
        
        int *pclient = malloc(sizeof(int));
        if (!pclient) {
            perror("malloc");
            close(new_sock);
            continue;
        }
        *pclient = new_sock;
        
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, pclient) != 0) {
            perror("pthread_create");
            close(new_sock);
            free(pclient);
            continue;
        }
        pthread_detach(tid);
    }
}
