#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

SOCKET sock;

void encryptDecrypt(char *msg) {
    char key = 'K'; // khóa bí mật
    for (int i = 0; msg[i] != '\0'; i++) {
        msg[i] ^= key;
    }
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        buffer[bytes] = '\0';

        encryptDecrypt(buffer); // giải mã
        printf("Server: %s\n", buffer);
    }
    return NULL;
}

void *send_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // bỏ newline

        encryptDecrypt(buffer); // mã hóa
        send(sock, buffer, strlen(buffer), 0);
    }
    return NULL;
}

int main() {
    WSADATA wsa;
    struct sockaddr_in serverAddr;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connect failed\n");
        return 1;
    }

    printf("Connected to server.\n");

    pthread_t recvThread, sendThread;
    pthread_create(&recvThread, NULL, receive_messages, NULL);
    pthread_create(&sendThread, NULL, send_messages, NULL);

    pthread_join(recvThread, NULL);
    pthread_join(sendThread, NULL);

    closesocket(sock);
    WSACleanup();

    return 0;
}
