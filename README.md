# Simple TCP Chat (C) — Multi-client Server & Client

A modular, multi-client TCP chat application written in C for Linux/POSIX, featuring authentication, logging, commands, broadcast messaging, and file transfer capabilities.

## Features

- **Multi-client server** using pthreads for concurrent connections
- **Authentication** with username/password (hard-coded credentials)
- **Conversation logging** to `server_log.txt` with timestamps
- **Chat commands:**
  - `/LIST` — list all connected clients
  - `/TIME` — get current server time
  - `/EXIT` — disconnect from server
  - `/UPLOAD <localpath> [remotename]` — upload a file to server
  - `/DOWNLOAD <filename>` — download a file from server
- **Broadcast chat** — messages sent to all connected clients
- **Modular architecture** for maintainability and extensibility

## Project Structure

```
IPC_Project_Group5/
├── protocol.h          # Common protocol definitions and constants
├── socket_utils.h      # Socket utility functions (send/recv helpers)
├── socket_utils.c      # Implementation of socket utilities
├── socket_init.c       # Server socket initialization and accept loop
├── server_thread.c     # Client thread handler (auth, commands, file ops)
├── server.c            # Main server entry point
├── client_ui.c         # Client UI and interaction logic
├── client.c            # Main client entry point
├── Makefile            # Build configuration
├── run.sh              # Script to build and start server
├── test_clients.sh     # Demo script to launch multiple clients
├── demo_script.txt     # Example usage walkthrough
└── README.md           # This file
```

## Build Instructions (Linux)

### Prerequisites

- GCC compiler
- Make utility
- POSIX-compliant system (Linux, WSL, macOS)

### Compile

```bash
make
```

This will compile both `server` and `client` executables.

### Clean

```bash
make clean
```

## Usage

### 1. Start the Server

```bash
./server
```

The server will listen on port **9090** and accept incoming connections.

### 2. Connect Clients

Open a new terminal for each client:

```bash
./client 127.0.0.1
```

You can connect from remote machines by replacing `127.0.0.1` with the server's IP address.

### 3. Authenticate

When you connect, the server will prompt for credentials:

**Available credentials:**

- Username: `user1` / Password: `pass1`
- Username: `user2` / Password: `pass2`
- Username: `alice` / Password: `wonder`

### 4. Chat and Use Commands

After authentication:

- Type a message and press Enter to broadcast to all clients
- Use commands starting with `/` (e.g., `/LIST`, `/TIME`, `/EXIT`)
- Upload files: `/UPLOAD ./myfile.txt remotename.txt`
- Download files: `/DOWNLOAD remotename.txt`

### 5. Quick Test with Multiple Clients

```bash
chmod +x test_clients.sh
./test_clients.sh
```

This will open multiple terminal windows with clients (requires gnome-terminal or xterm).

## Example Session

See `demo_script.txt` for a complete walkthrough of a multi-client chat session.

## Architecture Details

### Modular Design

The codebase is organized into logical modules:

1. **protocol.h** — Defines constants, commands, and protocol messages
2. **socket_utils.{c,h}** — Reusable socket I/O helpers (`send_all`, `recv_line`, `timestamp`)
3. **socket_init.c** — Server initialization (create socket, bind, listen, accept loop)
4. **server_thread.c** — Client handler (authentication, command processing, broadcast, file transfer)
5. **server.c** — Minimal main entry point for server
6. **client_ui.c** — Client UI logic (reader thread, authentication, upload/download)
7. **client.c** — Minimal main entry point for client

### Threading Model

- Server uses **pthreads** to handle each client in a separate thread
- Client uses a **reader thread** to asynchronously receive and display server messages

### File Transfer Protocol

- **Upload:** Client sends `/UPLOAD filename size`, then sends raw file bytes
- **Download:** Client sends `/DOWNLOAD filename`, server responds with `FILESIZE <n>` then raw bytes
- Uploaded files are stored in `uploads/` directory on server
- Downloaded files are saved to `downloads/last_download` on client

### Logging

All server events (connections, messages, uploads, downloads) are logged to `server_log.txt` with timestamps.

## Notes & Limitations

- Educational example — not production-hardened
- Credentials are hard-coded for simplicity
- File transfers block until complete (no resume/partial transfer support)
- Client saves downloads to `downloads/last_download` (simple approach)
- No encryption — data sent in plaintext

## Future Enhancements

- [ ] Save downloaded files with original filename
- [ ] Server admin console for broadcasting messages
- [ ] File-based or encrypted credential storage
- [ ] SSL/TLS encryption support
- [ ] Partial file transfer and resume capability
- [ ] Unit and integration tests

## License

This is an educational project. Feel free to use and modify as needed.
