# 🔌 IPC Client-Server Chat System

[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Standard: POSIX](https://img.shields.io/badge/Standard-POSIX-green.svg)](https://en.wikipedia.org/wiki/POSIX)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> A comprehensive Inter-Process Communication (IPC) demonstration project implementing client-server chat using **three different IPC methods**: Socket TCP, POSIX Message Queue, and Named Pipes (FIFOs).

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [IPC Methods](#-ipc-methods-comparison)
- [Project Structure](#-project-structure)
- [Installation](#-installation)
- [Usage](#-usage)
- [Commands](#-available-commands)
- [Examples](#-usage-examples)
- [Documentation](#-documentation)
- [Contributing](#-contributing)

---

## 🎯 Overview

This project demonstrates **Inter-Process Communication (IPC)** concepts by implementing a multi-client chat system using three distinct IPC mechanisms:

| Method | Type | Scope | Best For |
|--------|------|-------|----------|
| **Socket TCP** | Network IPC | Local + Remote | Cross-machine communication |
| **Message Queue** | True IPC | Same machine | Async messaging, message priority |
| **Named Pipes** | True IPC | Same machine | Stream data, simple IPC |

All implementations support:
- ✅ Multi-client concurrent connections
- ✅ User authentication
- ✅ Real-time broadcast messaging
- ✅ Timestamped logging
- ✅ Command interface

---

## ✨ Features

### Core Features
- **Multi-client support** — Multiple clients can connect simultaneously
- **Thread-based concurrency** — Each client handled in separate pthread
- **Authentication** — Username/password validation (Socket TCP)
- **Real-time chat** — Broadcast messages to all connected clients
- **Command interface** — Built-in commands for server interaction
- **Logging** — All conversations logged with timestamps

### Socket TCP Exclusive
- **File transfer** — Upload and download files
- **Network communication** — Connect from different machines
- **Full authentication** — Secure login system

### IPC Variants
- **Message Queue** — Kernel-managed async messaging
- **Named Pipes** — Stream-based file-like IPC

---

## 🔌 IPC Methods Comparison

### 1. Socket TCP (Network IPC)

```bash
./server              # Start TCP server
./client 127.0.0.1    # Connect client
```

**Characteristics:**
- ✅ Works across network (LAN/Internet)
- ✅ Full authentication system
- ✅ File upload/download support
- ❌ Slower than local IPC (TCP/IP overhead)

**Use when:** Clients on different machines, need network communication

---

### 2. POSIX Message Queue (True IPC)

```bash
./ipc_msgqueue_server   # Start MQ server
./ipc_msgqueue_client   # Connect client
```

**Characteristics:**
- ✅ Fast (no network overhead)
- ✅ Asynchronous messaging
- ✅ Message priority support
- ✅ Persistent queues (kernel-managed)
- ❌ Same machine only
- ❌ Message size limited (~8KB)

**Use when:** Same machine, async messaging, message priorities needed

---

### 3. Named Pipes / FIFOs (True IPC)

```bash
./ipc_pipe_server     # Start pipe server
./ipc_pipe_client     # Connect client
```

**Characteristics:**
- ✅ Fast (kernel IPC)
- ✅ Simple file-like API
- ✅ Shell-friendly (`cat`, `echo`, etc.)
- ✅ No size limits (stream-based)
- ❌ Same machine only
- ❌ Blocking by default

**Use when:** Same machine, streaming data, simple IPC needs

📖 **Detailed comparison:** See [IPC_COMPARISON.md](IPC_COMPARISON.md)

---

## 📁 Project Structure

```
IPC_Project_Group5/
├── 📄 Core Socket TCP Implementation
│   ├── protocol.h              # Protocol definitions & constants
│   ├── socket_utils.{h,c}      # Socket helper functions
│   ├── socket_init.c           # Server initialization
│   ├── server_thread.c         # Client handler logic
│   ├── server.c                # TCP server main
│   ├── client.c                # TCP client main
│   └── client_ui.c             # Client UI logic
│
├── 📄 Message Queue IPC
│   ├── ipc_msgqueue_server.c   # MQ server implementation
│   └── ipc_msgqueue_client.c   # MQ client implementation
│
├── 📄 Named Pipe IPC
│   ├── ipc_pipe_server.c       # FIFO server implementation
│   └── ipc_pipe_client.c       # FIFO client implementation
│
├── 🔧 Build & Configuration
│   ├── Makefile                # Build system
│   ├── run.sh                  # Quick start script
│   └── test_clients.sh         # Multi-client test script
│
└── 📚 Documentation
    ├── README.md               # This file
    ├── IPC_COMPARISON.md       # Detailed IPC comparison
    ├── SETUP_UBUNTU.md         # Ubuntu setup guide
    └── demo_script.txt         # Usage examples
```

---

## 🚀 Installation

### Prerequisites

- **OS:** Linux, macOS, or WSL (Windows Subsystem for Linux)
- **Compiler:** GCC 4.8+ or Clang
- **Tools:** Make, pthread library

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential gcc make -y
```

### macOS

```bash
xcode-select --install
```

### Build

```bash
# Clone repository
git clone https://github.com/trinhduyhieuVN/test-demo-upload.git
cd test-demo-upload

# Build all IPC variants
make

# Or build specific variants
make socket      # Socket TCP only
make msgqueue    # Message Queue only
make pipe        # Named Pipe only
```

---

## 💻 Usage

### Method 1: Socket TCP (Network IPC)

**Use when:** Client and server on different machines

```bash
# Terminal 1: Start server
./server

# Terminal 2+: Connect clients
./client 127.0.0.1           # Local connection
./client 192.168.1.100       # Remote machine
```

**Authentication (Socket TCP only):**
- Username: `user1` / Password: `pass1`
- Username: `user2` / Password: `pass2`
- Username: `alice` / Password: `wonder`

---

### Method 2: Message Queue IPC

**Use when:** Same machine, async messaging needed

```bash
# Terminal 1: Start server
./ipc_msgqueue_server

# Terminal 2+: Connect clients (same machine)
./ipc_msgqueue_client
# Enter username when prompted
```

**View active queues:**
```bash
ls /dev/mqueue/
```

---

### Method 3: Named Pipes IPC

**Use when:** Same machine, stream-based communication

```bash
# Terminal 1: Start server
./ipc_pipe_server

# Terminal 2+: Connect clients (same machine)
./ipc_pipe_client
# Enter username when prompted
```

**View active pipes:**
```bash
ls -l /tmp/chat_*
```

---

## 🎮 Available Commands

All methods support these commands:

| Command | Description | Example |
|---------|-------------|---------|
| `<message>` | Broadcast message to all clients | `Hello everyone!` |
| `/LIST` | Show all connected clients | `/LIST` |
| `/TIME` | Get current server time | `/TIME` |
| `/EXIT` | Disconnect from server | `/EXIT` |
| `/UPLOAD` | Upload file (Socket TCP only) | `/UPLOAD file.txt remote.txt` |
| `/DOWNLOAD` | Download file (Socket TCP only) | `/DOWNLOAD remote.txt` |

---

## 📖 Usage Examples

### Quick Start (Socket TCP)

```bash
# Terminal 1: Server
make socket && ./server

# Terminal 2: Client 1
./client 127.0.0.1
# Username: user1
# Password: pass1
> Hello world!

# Terminal 3: Client 2  
./client 127.0.0.1
# Username: alice
# Password: wonder
> /LIST
Connected clients:
 - user1
 - alice
```

### Message Queue Example

```bash
# Terminal 1
make msgqueue && ./ipc_msgqueue_server

# Terminal 2
./ipc_msgqueue_client
# Username: bob
> Fast IPC messaging!

# Terminal 3
./ipc_msgqueue_client
# Username: carol
> /TIME
Server time: 2025-11-04 10:30:00
```

### Named Pipe Example

```bash
# Terminal 1
make pipe && ./ipc_pipe_server

# Terminal 2
./ipc_pipe_client
# Username: dave
> Pipes are simple!

# Check active pipes
ls -l /tmp/chat_*
```

**📝 See [demo_script.txt](demo_script.txt) for detailed step-by-step examples of all three methods.**

---

## 📚 Documentation

| File | Description |
|------|-------------|
| [README.md](README.md) | Main documentation (this file) |
| [IPC_COMPARISON.md](IPC_COMPARISON.md) | Detailed comparison of IPC methods |
| [SETUP_UBUNTU.md](SETUP_UBUNTU.md) | Ubuntu VM setup instructions |
| [demo_script.txt](demo_script.txt) | Step-by-step usage examples |

---

## 🏗️ Architecture

### Modular Design

```
┌─────────────────────────────────────────┐
│           Application Layer             │
│  server.c / client.c / ipc_*_*.c       │
└─────────────────────────────────────────┘
           │                    │
           ▼                    ▼
┌──────────────────┐  ┌──────────────────┐
│  Socket Layer    │  │    IPC Layer     │
│  socket_utils.c  │  │  MQ / Pipe APIs  │
│  socket_init.c   │  │                  │
│  server_thread.c │  │                  │
└──────────────────┘  └──────────────────┘
           │                    │
           ▼                    ▼
┌─────────────────────────────────────────┐
│         Operating System Layer          │
│    TCP/IP Stack  |  IPC Mechanisms      │
└─────────────────────────────────────────┘
```

### Threading Model
- **Server:** One thread per client (pthread)
- **Client:** Main thread + reader thread for async receiving

### Logging
- Socket TCP: `server_log.txt`
- Message Queue: `server_log_mq.txt`
- Named Pipe: `server_log_pipe.txt`

---

## ⚠️ Known Limitations

- **Security:** No encryption, hard-coded credentials
- **File transfer:** Socket TCP only, no resume support
- **Download naming:** Files saved as `downloads/last_download`
- **Error handling:** Basic implementation for educational purposes
- **Platform:** POSIX-compliant systems only (Linux, macOS, WSL)

---

## 🚀 Future Enhancements

- [ ] SSL/TLS encryption for Socket TCP
- [ ] Shared memory IPC variant
- [ ] Dynamic credential management
- [ ] File transfer with original filenames
- [ ] Server admin console
- [ ] Automated test suite
- [ ] Docker containerization
- [ ] GUI client (GTK/Qt)

---

## 🤝 Contributing

This is an educational project. Contributions, issues, and feature requests are welcome!

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👨‍💻 Author

**trinhduyhieuVN**
- GitHub: [@trinhduyhieuVN](https://github.com/trinhduyhieuVN)
- Repository: [test-demo-upload](https://github.com/trinhduyhieuVN/test-demo-upload)

---

## 🙏 Acknowledgments

- POSIX standards documentation
- Linux IPC programming resources
- Stevens' "UNIX Network Programming"
- Educational IPC examples from various open-source projects

---

**⭐ If you find this project helpful, please consider giving it a star!**
