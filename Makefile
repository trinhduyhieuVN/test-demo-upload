CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11

# Object files
SERVER_OBJS = server.o socket_init.o socket_utils.o server_thread.o
CLIENT_OBJS = client.o client_ui.o socket_utils.o

all: server client

# Server
server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJS)

server.o: server.c protocol.h
	$(CC) $(CFLAGS) -c server.c

socket_init.o: socket_init.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c socket_init.c

socket_utils.o: socket_utils.c socket_utils.h
	$(CC) $(CFLAGS) -c socket_utils.c

server_thread.o: server_thread.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c server_thread.c

# Client
client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o client $(CLIENT_OBJS)

client.o: client.c protocol.h
	$(CC) $(CFLAGS) -c client.c

client_ui.o: client_ui.c protocol.h socket_utils.h
	$(CC) $(CFLAGS) -c client_ui.c

clean:
	rm -f server client *.o

