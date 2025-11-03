// protocol.h
// Common protocol definitions and constants for the chat application

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_PORT 9090
#define BACKLOG 10
#define MAX_CLIENTS 100
#define BUF_SIZE 4096
#define NAME_LEN 32

// Protocol messages
#define MSG_USERNAME "USERNAME:\n"
#define MSG_PASSWORD "PASSWORD:\n"
#define MSG_AUTH_OK "AUTH OK\n"
#define MSG_AUTH_FAIL "AUTH FAIL\n"
#define MSG_GOODBYE "Goodbye\n"
#define MSG_UPLOAD_OK "UPLOAD OK\n"
#define MSG_UPLOAD_FAIL "UPLOAD FAIL\n"
#define MSG_DOWNLOAD_FAIL "DOWNLOAD FAIL\n"
#define MSG_UNKNOWN_CMD "Unknown command\n"

// Commands
#define CMD_LIST "/LIST"
#define CMD_TIME "/TIME"
#define CMD_EXIT "/EXIT"
#define CMD_UPLOAD "/UPLOAD"
#define CMD_DOWNLOAD "/DOWNLOAD"

// File paths
#define UPLOAD_DIR "uploads"
#define DOWNLOAD_DIR "downloads"
#define LOG_FILE "server_log.txt"

#endif // PROTOCOL_H
