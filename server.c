// server.c
// Main server entry point - uses modular components

#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Forward declarations from socket_init.c
int create_server_socket(int port);
void run_server(int server_fd);

int main(int argc, char **argv) {
    int server_fd = create_server_socket(SERVER_PORT);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(1);
    }
    
    run_server(server_fd);
    
    close(server_fd);
    return 0;
}
