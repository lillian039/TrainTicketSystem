#include <iostream>

#include "parser.hpp"

using std::string;

int main() {
    std::ios::sync_with_stdio(false);
    string commandLine;
    while (true) {
        getline(std::cin, commandLine);
        std::cout<< parse_command(commandLine);
    }
}
