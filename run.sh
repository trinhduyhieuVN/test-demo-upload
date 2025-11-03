#!/usr/bin/env bash
# Build and start the server (Linux)
set -e
echo "Building..."
make server
echo "Starting server (foreground) on port 9090"
./server
