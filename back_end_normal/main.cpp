#include <iostream>

#include "parser.hpp"

using std::string;

int main() {
    std::ios::sync_with_stdio(false);
//    freopen("5.in", "r", stdin);
//    freopen("me.out", "w", stdout);
    string commandLine;
    while (true) {
        getline(std::cin, commandLine);
        std::cout<< parse_command(commandLine);
    }
}
