#include <iostream>
#include "parser.hpp"

int main() {
    std::string commandLine;
    while (true) {
        try {
            getline(cin, commandLine);
            if (cin.eof())std::exit(0);
            parseCommand(commandLine);
        }
        catch (my_error &ex) {
            std::cout << ex.what();
        }
    }
}
