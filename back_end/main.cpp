#include <iostream>

#include "parser.hpp"

using std::string;

int main() {

    freopen("2.in", "r", stdin);
    freopen("me.out", "w", stdout);
    string commandLine;
    while (true) {
        getline(std::cin, commandLine);
        std::cout<< parse_command(commandLine);
    }
}
