# So sánh các phương pháp IPC

## Tổng quan

Project này triển khai **3 phương pháp IPC** khác nhau để giao tiếp client-server:

| Phương pháp | Phạm vi | Use case | Ưu điểm | Nhược điểm |
|------------|---------|----------|---------|------------|
| **Socket TCP** | Mạng (local/remote) | Client-server qua mạng | Linh hoạt, qua mạng được | Chậm hơn IPC nội bộ |
| **Message Queue** | Cùng máy (IPC) | Truyền tin bất đồng bộ | Async, FIFO, persistent | Giới hạn kích thước msg |
| **Named Pipe (FIFO)** | Cùng máy (IPC) | Stream data giữa processes | Đơn giản, như file | Blocking, một-một |

---

## 1. Socket TCP (client.c / server.c)

### Đặc điểm
- **Giao thức:** TCP/IP qua mạng
- **Phạm vi:** Local hoặc remote (qua IP)
- **Mô hình:** Multi-threaded server (pthread)
- **Bidirectional:** Có (full-duplex)

### Khi nào dùng
✅ Client và server trên máy khác nhau  
✅ Cần giao tiếp qua mạng LAN/Internet  
✅ Cần bảo mật có thể thêm SSL/TLS  

### Hạn chế
❌ Chậm hơn IPC nội bộ (overhead của TCP/IP stack)  
❌ Không phải "true IPC" theo nghĩa hệ điều hành  

### Chạy
```bash
# Terminal 1: Server
./server

# Terminal 2, 3, 4: Clients (có thể từ máy khác)
./client 127.0.0.1
./client 192.168.1.100  # từ máy khác
```

---

## 2. POSIX Message Queue (ipc_msgqueue_*.c)

### Đặc điểm
- **Cơ chế:** POSIX Message Queue (kernel-managed)
- **Phạm vi:** Cùng máy (inter-process)
- **Mô hình:** Asynchronous message passing
- **Bidirectional:** Có (mỗi bên có queue riêng)
- **Persistent:** Messages tồn tại trong kernel queue

### API chính
```c
mq_open()      // Mở/tạo queue
mq_send()      // Gửi message (non-blocking)
mq_receive()   // Nhận message (blocking/non-blocking)
mq_close()     // Đóng queue
mq_unlink()    // Xóa queue
```

### Khi nào dùng
✅ Client-server trên cùng máy  
✅ Cần gửi tin bất đồng bộ (producer-consumer)  
✅ Message có độ ưu tiên  
✅ Cần queue persistent (messages không mất khi process crash)  

### Ưu điểm
- **Nhanh:** Không qua network stack
- **Reliable:** Kernel đảm bảo delivery
- **Async:** Sender không block
- **Priority:** Hỗ trợ message priority

### Hạn chế
- **Kích thước message giới hạn** (thường 8KB)
- **Chỉ local:** Không qua mạng
- **Cần quyền:** `/dev/mqueue` permissions

### Chạy
```bash
# Terminal 1: Server
./ipc_msgqueue_server

# Terminal 2, 3, 4: Clients (cùng máy)
./ipc_msgqueue_client
```

### Xem queues
```bash
ls /dev/mqueue/
cat /proc/sys/fs/mqueue/msg_max  # Max messages per queue
```

---

## 3. Named Pipes / FIFOs (ipc_pipe_*.c)

### Đặc điểm
- **Cơ chế:** Named FIFO (file-based)
- **Phạm vi:** Cùng máy (inter-process)
- **Mô hình:** Stream-based, FIFO (First In First Out)
- **Bidirectional:** Cần 2 pipes (mỗi chiều 1 pipe)
- **Interface:** Như file thông thường (read/write)

### API chính
```c
mkfifo()    // Tạo named pipe
open()      // Mở pipe (O_RDONLY / O_WRONLY)
read()      // Đọc data
write()     // Ghi data
close()     // Đóng
unlink()    // Xóa pipe
```

### Khi nào dùng
✅ Client-server trên cùng máy  
✅ Stream data giữa processes  
✅ Đơn giản, không cần kernel queue  
✅ Tích hợp với shell scripts  

### Ưu điểm
- **Đơn giản:** API giống file I/O
- **Shell-friendly:** Dùng được với `cat`, `echo`, etc.
- **Không giới hạn message size:** Stream-based

### Hạn chế
- **Blocking:** Reader/writer phải cùng lúc mở pipe
- **Một-một:** Mỗi pipe chỉ cho 1 reader và 1 writer
- **Không persistent:** Data mất khi không có reader
- **Không có priority/ordering guarantees ngoài FIFO**

### Chạy
```bash
# Terminal 1: Server
./ipc_pipe_server

# Terminal 2, 3, 4: Clients (cùng máy)
./ipc_pipe_client
```

### Debug pipes
```bash
ls -l /tmp/chat_*        # Xem pipes đã tạo
file /tmp/chat_server_fifo  # Kiểm tra type
```

---

## So sánh hiệu năng (theoretical)

| Metric | Socket TCP | Message Queue | Named Pipe |
|--------|-----------|---------------|------------|
| **Latency** | ~50-100μs | ~5-10μs | ~10-20μs |
| **Throughput** | ~1-10 GB/s | ~100-500 MB/s | ~500 MB/s - 2 GB/s |
| **Context switches** | Nhiều | Ít | Ít |
| **Network overhead** | Có (TCP/IP) | Không | Không |

---

## Khi nào dùng phương pháp nào?

### Dùng **Socket TCP** khi:
- Client và server trên máy khác nhau
- Cần giao tiếp qua mạng
- Cần mở rộng ra Internet

### Dùng **Message Queue** khi:
- Client-server cùng máy
- Cần async messaging
- Cần message priority
- Cần queue persistent

### Dùng **Named Pipe** khi:
- Client-server cùng máy
- Streaming data lớn
- Tích hợp với shell/scripts
- Cần đơn giản, không cần features phức tạp

### Dùng **Shared Memory** khi:
- Cần tốc độ cực nhanh (zero-copy)
- Data lớn (MB-GB)
- Cùng máy, cần đồng bộ (semaphore)

---

## Thêm: Shared Memory (TODO)

Nếu cần thêm **Shared Memory + Semaphore**, project có thể bổ sung:

```c
// Shared memory IPC (fastest)
shm_open(), mmap(), munmap(), shm_unlink()
sem_open(), sem_wait(), sem_post(), sem_close()
```

**Use case:** High-frequency data sharing (e.g., video frames, large datasets)

---

## Kết luận

Project này minh họa **3 phương pháp IPC chính** trong Linux:

1. ✅ **Socket TCP** — giao tiếp qua mạng (network IPC)
2. ✅ **Message Queue** — async messaging (true IPC)
3. ✅ **Named Pipe** — stream-based (true IPC)

Tất cả đều hỗ trợ:
- Multi-client
- Chat broadcast
- Commands (/LIST, /TIME, /EXIT)
- Logging

**Đề tài "IPC Client-Server" đã được đáp ứng đầy đủ!** ✅
