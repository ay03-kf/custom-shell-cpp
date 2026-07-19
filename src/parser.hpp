#pragma once

#include <string>
#include <vector>

// Represents a single command with arguments and redirection files.
struct Command {
    std::vector<std::string> args;
    std::string input_file;     // File path for <
    std::string output_file;    // File path for > or >>
    bool append_output = false; // true if >>
    std::string error_file;     // File path for 2> or 2>>
    bool append_error = false;  // true if 2>>
};

// Represents a chain of commands connected by pipes (|).
struct Pipeline {
    std::vector<Command> commands;
};

// Parse an input command line into a Pipeline structure.
Pipeline parse_pipeline(const std::string& input);
