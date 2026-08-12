#include <iostream>
#include <string>
#include "parser.hpp"
#include "executor.hpp"

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string line;
    while (true) {
        std::cout << "$ ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) {
            continue;
        }

        Pipeline pipeline = parse_pipeline(line);
        execute_pipeline(pipeline);
    }

    return 0;
}