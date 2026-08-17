# Unix Shell

A lightweight, POSIX-compatible Unix shell written in standard C++.

## Features
- **Interactive REPL:** Prompts `$ ` and executes user commands line by line.
- **Built-in Commands:** Native support for `exit`, `echo`, `type`, `pwd`, and `cd` (including `cd ~`).
- **External Command Execution:** Spawns child processes using POSIX process management APIs.
- **PATH Resolution:** Resolves external executable paths from `$PATH` and checks executable permissions via `access(..., X_OK)`.
- **Environment Variable Expansion:** Expands `$VAR` (such as `$HOME` and `$PATH`) in unquoted and double-quoted strings.
- **File Descriptor Redirection:** Redirects standard streams (`<`, `>`, `>>`, `2>`, `2>>`) via POSIX file descriptor calls.
- **Pipelines:** Inter-process communication for command chains (`|`).

## Architecture
```
main (REPL Loop) ---> parser (Tokenization & Pipeline Parsing) ---> executor / builtins
```

### Key POSIX APIs Used
- `fork()`: Process creation
- `execvp()`: Executable image replacement
- `waitpid()`: Synchronous process waiting
- `pipe()`: Inter-process pipe creation
- `dup2()`: File descriptor duplication / stream redirection
- `open()` & `close()`: File descriptor management

*Note: Linux/WSL/POSIX is the primary target runtime environment. Windows compatibility logic is included as a local development fallback.*

## Build & Run
```bash
# Build using CMake
cmake -B build -S .
cmake --build ./build

# Run shell
./build/shell
```

## Limitations
Advanced shell features such as job control, background jobs (`&`), process groups, command substitution (`$(...)` / `` `...` ``), conditional operators (`&&`, `||`), subshells, globbing, and autocompletion are intentionally not implemented to keep the codebase simple and understandable.
