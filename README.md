# IPC Client-Server Chat (C) — Multiple IPC Methods

A comprehensive Inter-Process Communication (IPC) demonstration project in C, implementing client-server chat using **3 different IPC methods**: Socket TCP, POSIX Message Queue, and Named Pipes (FIFOs).

## 🎯 Project Overview

This project fulfills the "IPC Client-Server" requirement by implementing:
1. ✅ **Socket TCP** — Network-based IPC (client-server over network)
2. ✅ **POSIX Message Queue** — Kernel-managed async messaging (true IPC)
3. ✅ **Named Pipes (FIFOs)** — File-based stream IPC (true IPC)

All implementations support:
- Multi-client communication
- Authentication
- Logging with timestamps
- Commands (/LIST, /TIME, /EXIT)
- Broadcast messaging

**📖 See [IPC_COMPARISON.md](IPC_COMPARISON.md) for detailed comparison of methods.**

## Features

- **Multi-client server** using pthreads for concurrent connections
- **Authentication** with username/password (hard-coded credentials)
- **Conversation logging** to log files with timestamps
- **Chat commands:**
  - `/LIST` — list all connected clients
  - `/TIME` — get current server time
  - `/EXIT` — disconnect from server
  - `/UPLOAD <localpath> [remotename]` — upload a file (Socket TCP only)
  - `/DOWNLOAD <filename>` — download a file (Socket TCP only)
- **Broadcast chat** — messages sent to all connected clients
- **Modular architecture** for maintainability and extensibility

## Project Structure

```
IPC_Project_Group5/
├── protocol.h                # Common protocol definitions
├── socket_utils.{h,c}        # Socket utility functions
├── socket_init.c             # Server socket initialization
├── server_thread.c           # Client thread handler
├── server.c / client.c       # Socket TCP implementation
├── client_ui.c               # Client UI logic
├── ipc_msgqueue_server.c     # Message Queue server
├── ipc_msgqueue_client.c     # Message Queue client
├── ipc_pipe_server.c         # Named Pipe server
├── ipc_pipe_client.c         # Named Pipe client
├── Makefile                  # Build all IPC variants
├── IPC_COMPARISON.md         # Detailed IPC method comparison
├── SETUP_UBUNTU.md           # Ubuntu VM setup instructions
└── README.md                 # This file
```

## Build Instructions (Linux)

### Prerequisites

- GCC compiler
- Make utility
- POSIX-compliant system (Linux, WSL, macOS)

### Compile All

```bash
make              # Build all IPC variants
make socket       # Build only Socket TCP version
make msgqueue     # Build only Message Queue version
make pipe         # Build only Named Pipe version
```

This will compile both `server` and `client` executables.

### Clean

```bash
make clean
```

## Usage

## Method 1: Socket TCP (Network IPC)

**Use when:** Client and server on different machines or over network

### Start Server
```bash
./server
```

### Connect Clients (same or different machines)
```bash
./client 127.0.0.1           # Local
./client 192.168.1.100       # Remote machine
```

### Authenticate
- Username: `user1` / Password: `pass1`
- Username: `user2` / Password: `pass2`
- Username: `alice` / Password: `wonder`

---

## Method 2: POSIX Message Queue (True IPC)

**Use when:** Client and server on same machine, need async messaging

### Start Server
```bash
./ipc_msgqueue_server
```

### Connect Clients (same machine only)
```bash
./ipc_msgqueue_client
```

Enter username when prompted (no password for IPC variants)

### View Queues
```bash
ls /dev/mqueue/
```

---

## Method 3: Named Pipes / FIFOs (True IPC)

**Use when:** Client and server on same machine, need stream-based communication

### Start Server
```bash
./ipc_pipe_server
```

### Connect Clients (same machine only)
```bash
./ipc_pipe_client
```

Enter username when prompted

### View Pipes
```bash
ls -l /tmp/chat_*
```

---

## Commands (All Methods)

After authentication / connection:

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
