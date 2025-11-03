// server_thread.c
// Client thread handler: authentication, commands, broadcast, file operations

#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <strings.h>

// Hard-coded credentials
struct cred { const char *user; const char *pass; };
static struct cred credentials[] = {
    {"user1", "pass1"},
    {"user2", "pass2"},
    {"alice", "wonder"},
};
static const int CRED_COUNT = sizeof(credentials) / sizeof(credentials[0]);

// Client information
typedef struct {
    int sock;
    char name[NAME_LEN];
    pthread_t tid;
    int active;
} client_t;

static client_t clients[MAX_CLIENTS];
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static FILE *log_fp = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Logging
static void log_msg(const char *fmt, ...) {
    char timebuf[64];
    timestamp(timebuf, sizeof(timebuf));
    pthread_mutex_lock(&log_mutex);
    if (!log_fp) {
        log_fp = fopen(LOG_FILE, "a");
        if (!log_fp) {
            perror("fopen log");
            pthread_mutex_unlock(&log_mutex);
            return;
        }
    }
    fprintf(log_fp, "[%s] ", timebuf);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(log_fp, fmt, ap);
    va_end(ap);
    fprintf(log_fp, "\n");
    fflush(log_fp);
    pthread_mutex_unlock(&log_mutex);
}

// Broadcast message to all clients (optionally exclude sender)
static void broadcast(const char *msg, int exclude_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && clients[i].sock != exclude_sock) {
            send_all(clients[i].sock, msg, strlen(msg));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Add client
static int add_client(client_t *c) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) {
            clients[i] = *c;
            clients[i].active = 1;
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return -1;
}

// Remove client by socket
static void remove_client_by_sock(int sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && clients[i].sock == sock) {
            close(clients[i].sock);
            clients[i].active = 0;
            clients[i].sock = -1;
            clients[i].name[0] = '\0';
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Handle LIST command
static void handle_list(int sock) {
    char out[BUF_SIZE];
    size_t off = 0;
    off += snprintf(out + off, sizeof(out) - off, "Connected clients:\n");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            off += snprintf(out + off, sizeof(out) - off, " - %s\n", clients[i].name);
            if (off + 128 >= sizeof(out)) break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    send_all(sock, out, strlen(out));
}

// Handle TIME command
static void handle_time(int sock) {
    char timebuf[128];
    timestamp(timebuf, sizeof(timebuf));
    char out[256];
    snprintf(out, sizeof(out), "Server time: %s\n", timebuf);
    send_all(sock, out, strlen(out));
}

// Handle file upload
static int handle_upload(int sock, const char *filename, size_t size) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", UPLOAD_DIR, filename);
    mkdir(UPLOAD_DIR, 0755);
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    size_t remaining = size;
    char buf[BUF_SIZE];
    while (remaining) {
        ssize_t r = recv(sock, buf, remaining > sizeof(buf) ? sizeof(buf) : remaining, 0);
        if (r <= 0) { fclose(fp); return -1; }
        fwrite(buf, 1, r, fp);
        remaining -= r;
    }
    fclose(fp);
    return 0;
}

// Handle file download
static int handle_download(int sock, const char *filename) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", UPLOAD_DIR, filename);
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char header[128];
    snprintf(header, sizeof(header), "FILESIZE %ld\n", sz);
    if (send_all(sock, header, strlen(header)) <= 0) { fclose(fp); return -1; }
    char buf[BUF_SIZE];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (send_all(sock, buf, r) <= 0) { fclose(fp); return -1; }
    }
    fclose(fp);
    return 0;
}

// Authentication
static int authenticate(int sock, char *out_name, size_t n) {
    char buf[256];
    if (send_all(sock, MSG_USERNAME, strlen(MSG_USERNAME)) <= 0) return -1;
    if (recv_line(sock, buf, sizeof(buf)) <= 0) return -1;
    buf[strcspn(buf, "\r\n")] = '\0';
    char username[128];
    strncpy(username, buf, sizeof(username));
    
    if (send_all(sock, MSG_PASSWORD, strlen(MSG_PASSWORD)) <= 0) return -1;
    if (recv_line(sock, buf, sizeof(buf)) <= 0) return -1;
    buf[strcspn(buf, "\r\n")] = '\0';
    char password[128];
    strncpy(password, buf, sizeof(password));
    
    for (int i = 0; i < CRED_COUNT; ++i) {
        if (strcmp(username, credentials[i].user) == 0 && 
            strcmp(password, credentials[i].pass) == 0) {
            snprintf(out_name, n, "%s", username);
            send_all(sock, MSG_AUTH_OK, strlen(MSG_AUTH_OK));
            return 0;
        }
    }
    send_all(sock, MSG_AUTH_FAIL, strlen(MSG_AUTH_FAIL));
    return -1;
}

// Client thread
void *client_thread(void *arg) {
    int sock = *((int *)arg);
    free(arg);
    char name[NAME_LEN] = {0};
    
    if (authenticate(sock, name, sizeof(name)) != 0) {
        log_msg("Authentication failed for socket %d", sock);
        close(sock);
        return NULL;
    }
    
    // Add client
    client_t c;
    c.sock = sock;
    strncpy(c.name, name, NAME_LEN - 1);
    c.active = 1;
    add_client(&c);
    
    char joinmsg[256];
    snprintf(joinmsg, sizeof(joinmsg), "%s has joined the chat\n", c.name);
    broadcast(joinmsg, sock);
    log_msg("%s connected (sock=%d)", c.name, sock);
    
    char buf[BUF_SIZE];
    while (1) {
        ssize_t r = recv_line(sock, buf, sizeof(buf));
        if (r <= 0) break;
        buf[strcspn(buf, "\r\n")] = '\0';
        if (strlen(buf) == 0) continue;
        
        if (buf[0] == '/') {
            char cmd[64];
            sscanf(buf + 1, "%63s", cmd);
            
            if (strcasecmp(cmd, "LIST") == 0) {
                handle_list(sock);
            } else if (strcasecmp(cmd, "TIME") == 0) {
                handle_time(sock);
            } else if (strcasecmp(cmd, "EXIT") == 0) {
                send_all(sock, MSG_GOODBYE, strlen(MSG_GOODBYE));
                break;
            } else if (strncasecmp(buf + 1, "UPLOAD ", 7) == 0) {
                char filename[256];
                size_t fsize = 0;
                if (sscanf(buf + 8, "%255s %zu", filename, &fsize) == 2) {
                    log_msg("%s uploading %s (%zu bytes)", c.name, filename, fsize);
                    if (handle_upload(sock, filename, fsize) == 0) {
                        send_all(sock, MSG_UPLOAD_OK, strlen(MSG_UPLOAD_OK));
                        log_msg("%s upload OK %s", c.name, filename);
                    } else {
                        send_all(sock, MSG_UPLOAD_FAIL, strlen(MSG_UPLOAD_FAIL));
                        log_msg("%s upload FAIL %s", c.name, filename);
                    }
                } else {
                    send_all(sock, "USAGE: /UPLOAD filename size\n", 29);
                }
            } else if (strncasecmp(buf + 1, "DOWNLOAD ", 9) == 0) {
                char filename[256];
                if (sscanf(buf + 10, "%255s", filename) == 1) {
                    log_msg("%s requested download %s", c.name, filename);
                    if (handle_download(sock, filename) != 0) {
                        send_all(sock, MSG_DOWNLOAD_FAIL, strlen(MSG_DOWNLOAD_FAIL));
                    }
                } else {
                    send_all(sock, "USAGE: /DOWNLOAD filename\n", 27);
                }
            } else {
                send_all(sock, MSG_UNKNOWN_CMD, strlen(MSG_UNKNOWN_CMD));
            }
        } else {
            // Regular message: broadcast
            char out[BUF_SIZE + NAME_LEN + 8];
            snprintf(out, sizeof(out), "%s: %s\n", c.name, buf);
            broadcast(out, -1);
            log_msg("%s: %s", c.name, buf);
        }
    }
    
    // Cleanup
    remove_client_by_sock(sock);
    snprintf(buf, sizeof(buf), "%s has left the chat\n", c.name);
    broadcast(buf, -1);
    log_msg("%s disconnected (sock=%d)", c.name, sock);
    return NULL;
}
