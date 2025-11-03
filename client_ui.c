// client_ui.c
// Client user interface and interaction logic

#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

// Reader thread: prints server messages
static void *reader_thread(void *arg) {
    int sock = *(int *)arg;
    char buf[BUF_SIZE];
    
    while (1) {
        ssize_t r = recv_line(sock, buf, sizeof(buf));
        if (r <= 0) {
            printf("Connection closed by server.\n");
            exit(0);
        }
        
        // Handle FILESIZE header for downloads
        if (strncmp(buf, "FILESIZE ", 9) == 0) {
            long sz = atol(buf + 9);
            printf("Receiving file (%ld bytes)...\n", sz);
            mkdir(DOWNLOAD_DIR, 0755);
            FILE *fp = fopen("downloads/last_download", "wb");
            if (!fp) {
                perror("fopen");
                continue;
            }
            long remaining = sz;
            char tmp[BUF_SIZE];
            while (remaining > 0) {
                ssize_t got = recv(sock, tmp, 
                    remaining > sizeof(tmp) ? sizeof(tmp) : remaining, 0);
                if (got <= 0) {
                    fclose(fp);
                    perror("recv");
                    break;
                }
                fwrite(tmp, 1, got, fp);
                remaining -= got;
            }
            fclose(fp);
            printf("Saved to downloads/last_download\n");
            continue;
        }
        
        printf("%s", buf);
    }
    return NULL;
}

// Handle authentication prompts
static int handle_auth(int sock) {
    char buf[BUF_SIZE];
    
    while (1) {
        ssize_t r = recv_line(sock, buf, sizeof(buf));
        if (r <= 0) {
            printf("Server closed\n");
            return -1;
        }
        
        if (strcmp(buf, MSG_USERNAME) == 0) {
            char username[128];
            printf("Username: ");
            fflush(stdout);
            if (fgets(username, sizeof(username), stdin) == NULL) return -1;
            send_all(sock, username, strlen(username));
        } else if (strcmp(buf, MSG_PASSWORD) == 0) {
            char password[128];
            printf("Password: ");
            fflush(stdout);
            if (fgets(password, sizeof(password), stdin) == NULL) return -1;
            send_all(sock, password, strlen(password));
        } else if (strcmp(buf, MSG_AUTH_OK) == 0) {
            printf("Authenticated. Commands: /LIST /TIME /EXIT /UPLOAD /DOWNLOAD\n");
            return 0;
        } else if (strcmp(buf, MSG_AUTH_FAIL) == 0) {
            printf("Authentication failed\n");
            return -1;
        } else {
            printf("%s", buf);
        }
    }
}

// Handle file upload command
static void handle_upload_command(int sock, const char *line) {
    char local[512], remote[256];
    if (sscanf(line + 8, "%511s %255s", local, remote) >= 1) {
        FILE *fp = fopen(local, "rb");
        if (!fp) {
            perror("fopen");
            return;
        }
        fseek(fp, 0, SEEK_END);
        long sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        char header[512];
        if (strlen(remote) == 0) snprintf(remote, sizeof(remote), "upload.bin");
        snprintf(header, sizeof(header), "/UPLOAD %s %ld\n", remote, sz);
        
        if (send_all(sock, header, strlen(header)) <= 0) {
            perror("send header");
            fclose(fp);
            return;
        }
        
        char tmp[BUF_SIZE];
        size_t r;
        while ((r = fread(tmp, 1, sizeof(tmp), fp)) > 0) {
            if (send_all(sock, tmp, r) <= 0) {
                perror("send file");
                break;
            }
        }
        fclose(fp);
    } else {
        printf("Usage: /UPLOAD <localpath> [remotename]\n");
    }
}

// Main input loop
void run_client_ui(int sock) {
    // Start reader thread
    pthread_t rt;
    if (pthread_create(&rt, NULL, reader_thread, &sock) != 0) {
        perror("pthread_create");
        return;
    }
    
    // Handle authentication
    if (handle_auth(sock) != 0) {
        return;
    }
    
    // Main input loop
    char line[BUF_SIZE];
    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;
        
        // Handle upload
        if (strncmp(line, "/UPLOAD ", 8) == 0) {
            handle_upload_command(sock, line);
            continue;
        }
        
        // Forward other commands and messages
        if (send_all(sock, line, strlen(line)) <= 0) {
            perror("send");
            break;
        }
    }
}
