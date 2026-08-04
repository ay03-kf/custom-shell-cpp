#pragma once

#include "parser.hpp"

// Executes a single pipeline of commands with support for builtins,
// external executables, file redirection, and multi-stage pipes.
void execute_pipeline(const Pipeline& pipeline);
