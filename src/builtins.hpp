#pragma once

#include "parser.hpp"
#include <string>

// Checks if a command name is a shell builtin.
bool is_builtin(const std::string& cmd);

// Searches the PATH environment variable for an executable file.
std::string find_in_path(const std::string& cmd);

// Executes a shell builtin command. Returns true if shell should exit.
bool execute_builtin(const Command& cmd);
