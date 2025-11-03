# Copilot Instructions for AI Coding Agents

## Project Overview
This repository contains C and C++ source code organized by topic and purpose. The structure is primarily educational or for algorithmic practice, with directories for different types of programs and experiments.

### Key Directories
- `main/` — Main C++ programs, often using file I/O (e.g., `input.txt`, `output.txt`).
- `C/` — C language client/server examples.
- `Trash/` — Experimental or draft C++ code, not always production-ready.
- `bin/` — Compiled binaries for quick execution/testing.

## Build & Run Workflows
- **Default build:** Use the VS Code build task (`Ctrl+Shift+B`) to compile the current file with `g++`.
- **Build and run:** Use the `build and run` task to compile and immediately execute a C++ file. This creates a temporary `temp.exe` and deletes it after execution.
- **Manual:** For C files, use `gcc` or `g++` as appropriate. Output binaries are placed in the same directory as the source or in `bin/`.

## Project Conventions
- C++ files use `.cpp` extension; C files use `.c`.
- Input/output for many programs is handled via `input.txt` and `output.txt` in the relevant directory.
- Temporary or test code is often placed in `Trash/`.
- Binaries are not version-controlled and can be safely overwritten.

## Patterns & Examples
- File I/O: Programs in `main/` often read from `input.txt` and write to `output.txt`.
- Networking: `C/client.c` and `C/server.c` demonstrate basic client-server communication in C.
- Each `.cpp` or `.c` file is typically a standalone program.

## AI Agent Guidance
- Prefer placing new experiments in `Trash/` unless they are production-ready.
- When adding new main programs, use the `main/` directory and follow the file I/O pattern.
- Use the provided build tasks for compilation; avoid hardcoding output paths.
- Do not commit binaries in `bin/`.

## External Integrations
- No external dependencies or package managers are used.
- No automated tests are present; manual execution is the norm.

---

**Enable Claude Sonnet 3.5 for all clients.**
