#!/usr/bin/env bash
# test_clients.sh
# Demo script to test multiple clients connecting to server

echo "=== Multi-client Test Script ==="
echo "Make sure the server is running first!"
echo ""
echo "This script will open 3 client terminals."
echo "Press Enter to continue..."
read

# Open 3 clients in background terminals (adjust for your terminal emulator)
# For gnome-terminal:
if command -v gnome-terminal &> /dev/null; then
    gnome-terminal -- bash -c "./client 127.0.0.1; exec bash" &
    sleep 1
    gnome-terminal -- bash -c "./client 127.0.0.1; exec bash" &
    sleep 1
    gnome-terminal -- bash -c "./client 127.0.0.1; exec bash" &
# For xterm:
elif command -v xterm &> /dev/null; then
    xterm -e "./client 127.0.0.1" &
    sleep 1
    xterm -e "./client 127.0.0.1" &
    sleep 1
    xterm -e "./client 127.0.0.1" &
else
    echo "No supported terminal found. Please open clients manually:"
    echo "  Terminal 1: ./client 127.0.0.1"
    echo "  Terminal 2: ./client 127.0.0.1"
    echo "  Terminal 3: ./client 127.0.0.1"
fi

echo "Test clients launched!"
