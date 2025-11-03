# Hướng dẫn chạy trên Ubuntu VM

## Bước 1: Đóng gói dự án trên Windows

### Cách 1: Dùng Git (Khuyên dùng)
Nếu bạn đã push code lên GitHub:
```bash
# Trên Ubuntu VM, clone repository
git clone https://github.com/trinhduyhieuVN/test-demo-upload.git
cd test-demo-upload
```

### Cách 2: Nén file thủ công
Trên Windows, nén các file sau thành `chat_project.zip`:
```
protocol.h
socket_utils.h
socket_utils.c
socket_init.c
server_thread.c
server.c
client_ui.c
client.c
Makefile
run.sh
test_clients.sh
demo_script.txt
README.md
```

Sau đó copy file zip vào Ubuntu VM (qua shared folder hoặc upload).

## Bước 2: Giải nén và chuẩn bị (trên Ubuntu VM)

```bash
# Giải nén (nếu dùng cách 2)
unzip chat_project.zip -d chat_project
cd chat_project

# Hoặc nếu copy thẳng folder
cd /path/to/chat_project

# Cấp quyền execute cho scripts
chmod +x run.sh test_clients.sh
```

## Bước 3: Cài đặt dependencies (nếu chưa có)

```bash
# Cập nhật package list
sudo apt update

# Cài đặt build tools
sudo apt install build-essential gcc make -y

# Kiểm tra
gcc --version
make --version
```

## Bước 4: Build chương trình

```bash
# Build server và client
make

# Hoặc dùng script
./run.sh
```

Nếu build thành công, bạn sẽ thấy:
- File executable `server`
- File executable `client`

## Bước 5: Chạy server

**Terminal 1:**
```bash
./server
```

Bạn sẽ thấy:
```
Server listening on port 9090
```

## Bước 6: Chạy clients (mở thêm terminals)

**Terminal 2 (Client 1):**
```bash
./client 127.0.0.1
```

Nhập credentials:
- Username: `user1`
- Password: `pass1`

**Terminal 3 (Client 2):**
```bash
./client 127.0.0.1
```

Nhập credentials:
- Username: `alice`
- Password: `wonder`

**Terminal 4 (Client 3):**
```bash
./client 127.0.0.1
```

Nhập credentials:
- Username: `user2`
- Password: `pass2`

## Bước 7: Test các tính năng

### Chat thông thường
Trong bất kỳ client nào, gõ:
```
Hello everyone!
```

Tin nhắn sẽ broadcast đến tất cả clients khác.

### Lệnh /LIST
```
/LIST
```
Hiển thị danh sách clients đang kết nối.

### Lệnh /TIME
```
/TIME
```
Hiển thị thời gian server.

### Upload file
```bash
# Tạo file test
echo "This is a test file" > testfile.txt

# Trong client, upload
/UPLOAD testfile.txt myfile.txt
```

### Download file
```
/DOWNLOAD myfile.txt
```

File sẽ được lưu vào `downloads/last_download`

### Thoát
```
/EXIT
```

## Bước 8: Xem log

```bash
cat server_log.txt
```

## Troubleshooting

### Lỗi "Address already in use"
```bash
# Tìm process đang dùng port 9090
sudo lsof -i :9090

# Kill process đó
kill -9 <PID>

# Hoặc đợi vài giây và thử lại
```

### Lỗi "Permission denied" khi chạy
```bash
# Cấp quyền execute
chmod +x server client
```

### Lỗi compile
```bash
# Clean và rebuild
make clean
make
```

## Mở nhiều terminal trong VS Code (Ubuntu VM)

1. Mở VS Code trong Ubuntu VM
2. Mở folder chứa project
3. Mở terminal: `Ctrl + `` (backtick)
4. Tạo terminal mới: Click nút `+` hoặc `Ctrl + Shift + `` 
5. Split terminal: Click nút split hoặc `Ctrl + Shift + 5`

Hoặc dùng tmux:
```bash
# Cài tmux
sudo apt install tmux -y

# Chạy tmux
tmux

# Split horizontal: Ctrl+b sau đó "
# Split vertical: Ctrl+b sau đó %
# Chuyển pane: Ctrl+b sau đó arrow keys
```

## Script tự động test (dùng tmux)

Tạo file `auto_test.sh`:
```bash
#!/bin/bash
# Start server in background
./server &
SERVER_PID=$!
sleep 2

# Open 3 clients in tmux windows
tmux new-session -d -s chat_test
tmux send-keys -t chat_test:0 './client 127.0.0.1' C-m
tmux split-window -h -t chat_test:0
tmux send-keys -t chat_test:0.1 './client 127.0.0.1' C-m
tmux split-window -v -t chat_test:0
tmux send-keys -t chat_test:0.2 './client 127.0.0.1' C-m

# Attach to tmux session
tmux attach -t chat_test

# Cleanup when done
kill $SERVER_PID
```

Chạy:
```bash
chmod +x auto_test.sh
./auto_test.sh
```

## Tips

- Dùng `Ctrl+C` để tắt server
- File log: `server_log.txt`
- File uploads: thư mục `uploads/`
- File downloads: thư mục `downloads/`
- Credentials có sẵn trong `demo_script.txt`

---

**Chúc bạn test thành công! 🚀**
