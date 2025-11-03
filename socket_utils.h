// socket_utils.h
// Utility functions for socket operations (send/recv helpers)

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/types.h>
#include <stddef.h>

// Send all bytes reliably
ssize_t send_all(int sock, const void *buf, size_t len);

// Receive a line ending with '\n' (null-terminated)
ssize_t recv_line(int sock, char *out, size_t maxlen);

// Helper: timestamp string (YYYY-MM-DD HH:MM:SS)
void timestamp(char *buf, size_t n);

#endif // SOCKET_UTILS_H
