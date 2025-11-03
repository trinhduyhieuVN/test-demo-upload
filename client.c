// client.c
// Main client entry point - uses modular UI component

#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Forward declaration from client_ui.c
void run_client_ui(int sock);

int main(int argc, char **argv) {
    const char *server_addr = argc > 1 ? argv[1] : "127.0.0.1";
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_addr, &serv.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("connect");
        return 1;
    }
    
    run_client_ui(sock);
    
    close(sock);
    return 0;
}
