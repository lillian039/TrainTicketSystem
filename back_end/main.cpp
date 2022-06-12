#include <iostream>

#include "parser.hpp"

using std::string;

int main() {
    string commandLine;
    while (true) {
        getline(std::cin, commandLine);
        std::cout<< parse_command(commandLine);
    }
}
