#include <iostream>

#include "parser.hpp"

using std::string;

int main() {

//    freopen("10.in", "r", stdin);
//    freopen("me.out", "w", stdout);
    string commandLine;
    while (true) {
        getline(std::cin, commandLine);
//        if (std::cin.eof())
//            std::exit(0);
        std::cout<< parse_command(commandLine);
//        if (commandLine == "exit") break;
//        else std::cout << commandLine;
    }
    /*remove("bpt_users_file_tree");
    remove("bpt_users_file_leaf");
    remove("bpt_station_train_file_leaf");
    remove("bpt_station_train_file_tree");
    remove("bpt_train_information_file_leaf");
    remove("bpt_train_information_file_tree");
    remove("btp_train_order_file_leaf");
    remove("btp_train_order_file_tree");
    remove("btp_user_order_file_leaf");
    remove("btp_user_order_file_tree");
    remove("train_ticket_database");
    remove("train_information_database");*/
}
