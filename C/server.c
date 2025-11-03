#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

SOCKET clientSocket;

void encryptDecrypt(char *msg) {
    char key = 'K'; // khóa bí mật
    for (int i = 0; msg[i] != '\0'; i++) {
        msg[i] ^= key;
    }
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        buffer[bytes] = '\0';

        encryptDecrypt(buffer); // giải mã
        printf("Client: %s\n", buffer);
    }
    return NULL;
}

void *send_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // bỏ newline

        encryptDecrypt(buffer); // mã hóa
        send(clientSocket, buffer, strlen(buffer), 0);
    }
    return NULL;
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 3);

    printf("Server listening on port %d...\n", PORT);

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    printf("Client connected!\n");

    pthread_t recvThread, sendThread;
    pthread_create(&recvThread, NULL, receive_messages, NULL);
    pthread_create(&sendThread, NULL, send_messages, NULL);

    pthread_join(recvThread, NULL);
    pthread_join(sendThread, NULL);

    closesocket(serverSocket);
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
