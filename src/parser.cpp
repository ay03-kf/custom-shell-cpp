#include "parser.hpp"
#include <cstdlib>
#include <cctype>
#include <iostream>

// Helper to look up environment variable with fallback for HOME on Windows
static const char* get_env_var(const std::string& name) {
    const char* val = std::getenv(name.c_str());
    if (!val && name == "HOME") {
        val = std::getenv("USERPROFILE");
    }
    return val;
}

// Tokenizes command line input into raw tokens respecting quotes, escapes, and operators.
static std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool in_single = false;
    bool in_double = false;

    size_t i = 0;
    while (i < input.length()) {
        char c = input[i];

        if (in_single) {
            if (c == '\'') {
                in_single = false;
            } else {
                current += c;
            }
            i++;
        } else if (in_double) {
            if (c == '"') {
                in_double = false;
                i++;
            } else if (c == '\\' && i + 1 < input.length()) {
                char next = input[i + 1];
                if (next == '"' || next == '\\' || next == '$' || next == '\n') {
                    current += next;
                    i += 2;
                } else {
                    current += c;
                    i++;
                }
            } else if (c == '$') {
                // Expand environment variables inside double quotes
                size_t start = i + 1;
                size_t end = start;
                while (end < input.length() && (std::isalnum(input[end]) || input[end] == '_')) {
                    end++;
                }
                if (end > start) {
                    std::string var_name = input.substr(start, end - start);
                    const char* val = get_env_var(var_name);
                    if (val) current += val;
                    i = end;
                } else {
                    current += c;
                    i++;
                }
            } else {
                current += c;
                i++;
            }
        } else {
            // Outside quotes
            if (std::isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                i++;
            } else if (c == '\'') {
                in_single = true;
                i++;
            } else if (c == '"') {
                in_double = true;
                i++;
            } else if (c == '\\' && i + 1 < input.length()) {
                current += input[i + 1];
                i += 2;
            } else if (c == '$') {
                size_t start = i + 1;
                size_t end = start;
                while (end < input.length() && (std::isalnum(input[end]) || input[end] == '_')) {
                    end++;
                }
                if (end > start) {
                    std::string var_name = input.substr(start, end - start);
                    const char* val = get_env_var(var_name);
                    if (val) current += val;
                    i = end;
                } else {
                    current += c;
                    i++;
                }
            } else if (c == '|' || c == '<' || c == '>') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                if (c == '>' && i + 1 < input.length() && input[i + 1] == '>') {
                    tokens.push_back(">>");
                    i += 2;
                } else {
                    tokens.push_back(std::string(1, c));
                    i++;
                }
            } else if (c == '2' && i + 1 < input.length() && input[i + 1] == '>') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                if (i + 2 < input.length() && input[i + 2] == '>') {
                    tokens.push_back("2>>");
                    i += 3;
                } else {
                    tokens.push_back("2>");
                    i += 2;
                }
            } else {
                current += c;
                i++;
            }
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

Pipeline parse_pipeline(const std::string& input) {
    Pipeline pipeline;
    std::vector<std::string> tokens = tokenize(input);
    Command cmd;

    for (size_t i = 0; i < tokens.size(); i++) {
        const std::string& tok = tokens[i];

        if (tok == "|") {
            pipeline.commands.push_back(cmd);
            cmd = Command();
        } else if (tok == "<" && i + 1 < tokens.size()) {
            cmd.input_file = tokens[++i];
        } else if (tok == ">" && i + 1 < tokens.size()) {
            cmd.output_file = tokens[++i];
            cmd.append_output = false;
        } else if (tok == ">>" && i + 1 < tokens.size()) {
            cmd.output_file = tokens[++i];
            cmd.append_output = true;
        } else if (tok == "2>" && i + 1 < tokens.size()) {
            cmd.error_file = tokens[++i];
            cmd.append_error = false;
        } else if (tok == "2>>" && i + 1 < tokens.size()) {
            cmd.error_file = tokens[++i];
            cmd.append_error = true;
        } else {
            cmd.args.push_back(tok);
        }
    }

    if (!cmd.args.empty() || !cmd.input_file.empty() || !cmd.output_file.empty() || !cmd.error_file.empty()) {
        pipeline.commands.push_back(cmd);
    }

    return pipeline;
}
