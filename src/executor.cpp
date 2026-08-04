#include "executor.hpp"
#include "builtins.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fcntl.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#else
#include <io.h>
#include <process.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif

// Set up file descriptor redirections using open() and dup2()
static void setup_redirections(const Command& cmd) {
    if (!cmd.input_file.empty()) {
        int fd = open(cmd.input_file.c_str(), O_RDONLY);
        if (fd < 0) {
            perror(("open " + cmd.input_file).c_str());
            std::exit(1);
        }
        dup2(fd, STDIN_FILENO); // Redirect stdin to input file
        close(fd);
    }

    if (!cmd.output_file.empty()) {
        int flags = O_WRONLY | O_CREAT | (cmd.append_output ? O_APPEND : O_TRUNC);
        int fd = open(cmd.output_file.c_str(), flags, 0644);
        if (fd < 0) {
            perror(("open " + cmd.output_file).c_str());
            std::exit(1);
        }
        dup2(fd, STDOUT_FILENO); // Redirect stdout to output file
        close(fd);
    }

    if (!cmd.error_file.empty()) {
        int flags = O_WRONLY | O_CREAT | (cmd.append_error ? O_APPEND : O_TRUNC);
        int fd = open(cmd.error_file.c_str(), flags, 0644);
        if (fd < 0) {
            perror(("open " + cmd.error_file).c_str());
            std::exit(1);
        }
        dup2(fd, STDERR_FILENO); // Redirect stderr to error file
        close(fd);
    }
}

// Executes external binary using execvp()
static void run_external(const Command& cmd) {
    std::string path = find_in_path(cmd.args[0]);
    if (path.empty()) {
        std::cerr << cmd.args[0] << ": command not found\n";
        std::exit(127);
    }

    std::vector<char*> argv;
    for (const auto& arg : cmd.args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    // execvp replaces current child process image with target binary
    execvp(path.c_str(), argv.data());
    perror("execvp");
    std::exit(127);
}

void execute_pipeline(const Pipeline& pipeline) {
    if (pipeline.commands.empty()) return;

    size_t num_cmds = pipeline.commands.size();

    // Single builtin command without redirection runs directly in shell process
    if (num_cmds == 1) {
        const Command& cmd = pipeline.commands[0];
        if (cmd.args.empty()) return;

        if (is_builtin(cmd.args[0]) && cmd.input_file.empty() && cmd.output_file.empty() && cmd.error_file.empty()) {
            execute_builtin(cmd);
            return;
        }
    }

#ifndef _WIN32
    // Pipeline execution using pipe(), fork(), dup2(), execvp(), and waitpid()
    std::vector<int[2]> pipes;
    if (num_cmds > 1) {
        pipes.resize(num_cmds - 1);
        for (size_t i = 0; i < num_cmds - 1; i++) {
            if (pipe(pipes[i]) < 0) {
                perror("pipe");
                return;
            }
        }
    }

    std::vector<pid_t> pids;

    for (size_t i = 0; i < num_cmds; i++) {
        const Command& cmd = pipeline.commands[i];
        pid_t pid = fork(); // Spawn child process

        if (pid < 0) {
            perror("fork");
            return;
        }

        if (pid == 0) { // Child process execution
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO); // Read from previous pipe
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO); // Write to next pipe
            }

            // Close unused pipe ends in child process
            for (size_t j = 0; j < pipes.size(); j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            setup_redirections(cmd);

            if (is_builtin(cmd.args[0])) {
                execute_builtin(cmd);
                _exit(0);
            } else {
                run_external(cmd);
            }
        } else {
            pids.push_back(pid);
        }
    }

    // Close pipe descriptors in parent process
    for (size_t j = 0; j < pipes.size(); j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }

    // Parent process waits for all children to complete
    for (pid_t pid : pids) {
        int status;
        waitpid(pid, &status, 0);
    }
#else
    // Fallback execution engine for Windows local development environment
    std::string full_cmd_str;
    for (size_t i = 0; i < num_cmds; i++) {
        const Command& cmd = pipeline.commands[i];
        if (cmd.args.empty()) continue;

        if (i > 0) full_cmd_str += " | ";

        if (is_builtin(cmd.args[0])) {
            std::string sub_cmd;
            if (cmd.args[0] == "echo") {
                sub_cmd = "echo";
                for (size_t a = 1; a < cmd.args.size(); a++) {
                    sub_cmd += " " + cmd.args[a];
                }
            } else if (cmd.args[0] == "pwd") {
                sub_cmd = "cd";
            }
            if (!cmd.output_file.empty()) {
                sub_cmd += (cmd.append_output ? " >> \"" : " > \"") + cmd.output_file + "\"";
            }
            full_cmd_str += sub_cmd;
        } else {
            std::string path = find_in_path(cmd.args[0]);
            if (path.empty()) {
                std::cerr << cmd.args[0] << ": command not found\n";
                continue;
            }

            std::string sub_cmd = "\"" + path + "\"";
            for (size_t a = 1; a < cmd.args.size(); a++) {
                sub_cmd += " \"" + cmd.args[a] + "\"";
            }

            if (!cmd.input_file.empty()) {
                sub_cmd += " < \"" + cmd.input_file + "\"";
            }
            if (!cmd.output_file.empty()) {
                sub_cmd += (cmd.append_output ? " >> \"" : " > \"") + cmd.output_file + "\"";
            }
            if (!cmd.error_file.empty()) {
                sub_cmd += (cmd.append_error ? " 2>> \"" : " 2> \"") + cmd.error_file + "\"";
            }

            full_cmd_str += sub_cmd;
        }
    }

    if (!full_cmd_str.empty()) {
        std::string system_cmd = "cmd /c \"" + full_cmd_str + "\"";
        std::system(system_cmd.c_str());
    }
#endif
}
