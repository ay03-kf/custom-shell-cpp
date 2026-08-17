#include "builtins.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define chdir _chdir
#define getgetcwd _getcwd
#else
#include <unistd.h>
#endif

// Check if a path exists and is an executable file
static bool is_executable(const std::string& path) {
#ifndef _WIN32
    return access(path.c_str(), X_OK) == 0;
#else
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return (st.st_mode & S_IFREG) != 0;
    }
    return false;
#endif
}

bool is_builtin(const std::string& cmd) {
    return cmd == "exit" || cmd == "echo" || cmd == "type" || cmd == "pwd" || cmd == "cd";
}

std::string find_in_path(const std::string& cmd) {
    if (cmd.find('/') != std::string::npos || cmd.find('\\') != std::string::npos) {
        if (is_executable(cmd)) return cmd;
        return "";
    }

    const char* path_env = std::getenv("PATH");
    if (!path_env) return "";

    std::stringstream ss(path_env);
    std::string dir;

    char delimiter = ':';
#ifdef _WIN32
    delimiter = ';';
#endif

    while (std::getline(ss, dir, delimiter)) {
        if (dir.empty()) continue;
        std::string full_path = dir + "/" + cmd;
        if (is_executable(full_path)) {
            return full_path;
        }
#ifdef _WIN32
        std::string exe_path = dir + "/" + cmd + ".exe";
        if (is_executable(exe_path)) {
            return exe_path;
        }
#endif
    }
    return "";
}

bool execute_builtin(const Command& cmd) {
    if (cmd.args.empty()) return false;

    const std::string& name = cmd.args[0];

    if (name == "exit") {
        int exit_code = 0;
        if (cmd.args.size() > 1) {
            exit_code = std::stoi(cmd.args[1]);
        }
        std::exit(exit_code);
    } else if (name == "echo") {
        for (size_t i = 1; i < cmd.args.size(); i++) {
            std::cout << cmd.args[i] << (i + 1 < cmd.args.size() ? " " : "");
        }
        std::cout << "\n";
    } else if (name == "pwd") {
        char cwd[1024];
#ifndef _WIN32
        if (getcwd(cwd, sizeof(cwd))) {
            std::cout << cwd << "\n";
        }
#else
        if (_getcwd(cwd, sizeof(cwd))) {
            std::cout << cwd << "\n";
        }
#endif
    } else if (name == "cd") {
        std::string path;
        if (cmd.args.size() < 2 || cmd.args[1] == "~") {
            const char* home = std::getenv("HOME");
#ifdef _WIN32
            if (!home) home = std::getenv("USERPROFILE");
#endif
            if (home) path = home;
        } else {
            path = cmd.args[1];
        }

        if (!path.empty()) {
            if (chdir(path.c_str()) != 0) {
                std::cout << "cd: " << path << ": No such file or directory\n";
            }
        }
    } else if (name == "type") {
        if (cmd.args.size() < 2) return false;
        const std::string& target = cmd.args[1];
        if (is_builtin(target)) {
            std::cout << target << " is a shell builtin\n";
        } else {
            std::string resolved = find_in_path(target);
            if (!resolved.empty()) {
                std::cout << target << " is " << resolved << "\n";
            } else {
                std::cout << target << ": not found\n";
            }
        }
    }
    return false;
}
