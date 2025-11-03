// socket_utils.c
// Implementation of socket utility functions

#define _POSIX_C_SOURCE 200809L
#include "socket_utils.h"
#include <sys/socket.h>
#include <time.h>
#include <string.h>

// Send all bytes reliably
ssize_t send_all(int sock, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t sent = send(sock, p + total, len - total, 0);
        if (sent <= 0) return sent;
        total += sent;
    }
    return total;
}

// Receive a line ending with '\n' into out (null-terminated)
ssize_t recv_line(int sock, char *out, size_t maxlen) {
    size_t idx = 0;
    while (idx + 1 < maxlen) {
        char c;
        ssize_t r = recv(sock, &c, 1, 0);
        if (r <= 0) return -1;
        out[idx++] = c;
        if (c == '\n') break;
    }
    out[idx] = '\0';
    return idx;
}

// Helper: timestamp string (YYYY-MM-DD HH:MM:SS)
void timestamp(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &tm);
}
