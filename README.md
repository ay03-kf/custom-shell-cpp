# Unix Shell (POSIX-Compatible Command Line Interpreter)

**Domain:** Operating Systems | Systems Programming | C++ | Linux

A clean, resume-ready Unix-like shell implemented in standard C++ and POSIX system APIs.

## 🚀 Key Features & Implementation
- **Interactive REPL Loop:** Prompts `$ ` and reads user input until EOF or `exit`.
- **Command Parsing & Quoting:** Tokenizer handling single quotes (`'...'`), double quotes (`"..."`), and backslash escapes (`\`).
- **Environment Variable Expansion:** Expands `$VAR` (such as `$HOME`, `$PATH`) in unquoted and double-quoted contexts.
- **Built-in Commands:** Native implementations of `exit`, `echo`, `type`, `pwd`, and `cd` (including `cd ~`).
- **PATH Resolution:** Searches `$PATH` directories to resolve executable binary paths.
- **Process Management:** Spawns and synchronizes child processes using POSIX system calls (`fork()`, `execvp()`, `waitpid()`).
- **I/O & Error Redirection:** File descriptor redirection for `<`, `>`, `>>`, `2>`, `2>>` using `open()`, `dup2()`, and `close()`.
- **Multi-stage Pipelines:** Inter-process communication pipelines (`|`) using `pipe()`, `fork()`, `dup2()`, and `waitpid()`.

## 🏗️ Architecture
- `src/main.cpp`: REPL entry point and main event loop.
- `src/parser.hpp` / `src/parser.cpp`: Data structures (`Command`, `Pipeline`), tokenization, quoting, variable expansion, and AST parsing.
- `src/builtins.hpp` / `src/builtins.cpp`: Builtin command implementations and `$PATH` executable lookup.
- `src/executor.hpp` / `src/executor.cpp`: Process creation, file descriptor manipulation, and pipeline execution engine.

## 🛠️ Build & Run
```bash
# Build using CMake
cmake -B build -S .
cmake --build ./build

# Run executable
./build/shell
```
