CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11
LDFLAGS = -lrt

# Object files for socket-based programs
SERVER_OBJS = server.o socket_init.o socket_utils.o server_thread.o
CLIENT_OBJS = client.o client_ui.o socket_utils.o

# All targets
all: socket ipc

# Socket-based (TCP/IP over network)
socket: server client

# IPC variants (local inter-process communication)
ipc: msgqueue pipe

# Message Queue IPC
msgqueue: ipc_msgqueue_server ipc_msgqueue_client

# Named Pipe IPC
pipe: ipc_pipe_server ipc_pipe_client

# Socket TCP Server & Client
server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o client $(CLIENT_OBJS)

server.o: server.c protocol.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c protocol.h
	$(CC) $(CFLAGS) -c client.c

socket_init.o: socket_init.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c socket_init.c

socket_utils.o: socket_utils.c socket_utils.h
	$(CC) $(CFLAGS) -c socket_utils.c

server_thread.o: server_thread.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c server_thread.c

client_ui.o: client_ui.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c client_ui.c

# Message Queue IPC Programs
ipc_msgqueue_server: ipc_msgqueue_server.c
	$(CC) $(CFLAGS) -o ipc_msgqueue_server ipc_msgqueue_server.c $(LDFLAGS)

ipc_msgqueue_client: ipc_msgqueue_client.c
	$(CC) $(CFLAGS) -o ipc_msgqueue_client ipc_msgqueue_client.c $(LDFLAGS)

# Named Pipe IPC Programs
ipc_pipe_server: ipc_pipe_server.c
	$(CC) $(CFLAGS) -o ipc_pipe_server ipc_pipe_server.c

ipc_pipe_client: ipc_pipe_client.c
	$(CC) $(CFLAGS) -o ipc_pipe_client ipc_pipe_client.c

# Clean
clean:
	rm -f server client *.o
	rm -f ipc_msgqueue_server ipc_msgqueue_client
	rm -f ipc_pipe_server ipc_pipe_client
	rm -f /tmp/chat_*
	rm -f /dev/mqueue/chat_*

.PHONY: all socket ipc msgqueue pipe clean

