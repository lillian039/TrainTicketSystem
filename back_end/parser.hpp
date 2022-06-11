#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>

#include "user.hpp"
#include "train.hpp"
#include "order.hpp"
#include "date.hpp"
#include "vector.hpp"

using std::string;

sjtu::vector<string> parser(const string &command_line) {
    sjtu::vector<string> words;
    string word = "";
    int len = command_line.length();
    for (int i = 0; i < len; i++) {
        if (command_line[i] == ' ') {
            while (i + 1 < len && command_line[i + 1] == ' ')
                i++;
            if (word != "")
                words.push_back(word);
            word = "";
            continue;
        }
        word += command_line[i];
    }
    if (word != "")
        words.push_back(word);
    return words;
}

int toint(const string &u) {
    int v = 0, len = u.length();
    for (int i = 0; i < len; i++)
        if (std::isdigit(u[i]))
            v = (v << 3) + (v << 1) + (u[i] ^ 48);
    return v;
}

sjtu::vector<string> divi(const string &u) {
    int len = u.length();
    string v = "";
    sjtu::vector<string> w;
    for (int i = 0; i < len; i++) {
        if (u[i] == '|')
            w.push_back(v), v = "";
        else
            v += u[i];
    }
    if (v != "")
        w.push_back(v);
    return w;
}

date today(const string &u) {
    return date(toint(u.substr(0, 2)), toint(u.substr(3, 2)), 0, 0);
}

date tomin(const string &u) {
    return date(6, 1, toint(u.substr(0, 2)), toint(u.substr(3, 2)));
}

string parse_command(const string &command_line) {
    sjtu::vector<string> words = parser(command_line);
//    if (toint(words[0]) == 7197) {
//        std::cout << "!";
//    }
//    if (toint(words[0]) >= 13261) std::cerr << command_line << std::endl;
    int z = words.size();
    if (words[1] == "add_user") {
        string cur_username = "", username = "", password = "", realname = "", mailaddr = "";
        int privilege = -1;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-c")
                cur_username = words[i + 1];
            else if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-p")
                password = words[i + 1];
            else if (words[i] == "-n")
                realname = words[i + 1];
            else if (words[i] == "-m")
                mailaddr = words[i + 1];
            else if (words[i] == "-g")
                privilege = toint(words[i + 1]);
        }
        return words[0] + " " +
               userManagement.add_user(cur_username, username,
                                       password, realname, mailaddr, privilege);
    }
    if (words[1] == "login") {
        string username = "", password = "";
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-p")
                password = words[i + 1];
        }
        return words[0] + " " + userManagement.login(username, password);
    }
    if (words[1] == "logout") {
        string username = "";
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-u")
                username = words[i + 1];
        }
        return words[0] + " " + userManagement.logout(username);
    }
    if (words[1] == "query_profile") {
        string cur_username = "", username = "";
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-c")
                cur_username = words[i + 1];
        }
        return words[0] + " " + userManagement.query_profile(cur_username, username);
    }
    if (words[1] == "modify_profile") {
        string cur_username = "", username = "", password = "", realname = "", mailaddr = "";
        int privilege = -1;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-c")
                cur_username = words[i + 1];
            else if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-p")
                password = words[i + 1];
            else if (words[i] == "-n")
                realname = words[i + 1];
            else if (words[i] == "-m")
                mailaddr = words[i + 1];
            else if (words[i] == "-g")
                privilege = toint(words[i + 1]);
        }
        return words[0] + " " +
               userManagement.modify_profile(cur_username, username,
                                             password, realname, mailaddr, privilege);
    }
    if (words[1] == "add_train") {
        string trainid = "", type = "";
        sjtu::vector<string> station_name;
        int station_num = 0, seat_num = 0;
        sjtu::vector<int> prices, travel_time, stopover_time;
        date start_time, start_sale, end_sale;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-i")
                trainid = words[i + 1];
            else if (words[i] == "-n")
                station_num = toint(words[i + 1]);
            else if (words[i] == "-m")
                seat_num = toint(words[i + 1]);
            else if (words[i] == "-s")
                station_name = divi(words[i + 1]);
            else if (words[i] == "-p") {
                sjtu::vector<string> u = divi(words[i + 1]);
                int uz = u.size();
                for (int j = 0; j < uz; j++)
                    prices.push_back(toint(u[j]));
            } else if (words[i] == "-t") {
                sjtu::vector<string> u = divi(words[i + 1]);
                int uz = u.size();
                for (int j = 0; j < uz; j++)
                    travel_time.push_back(toint(u[j]));
            } else if (words[i] == "-o") {
                sjtu::vector<string> u = divi(words[i + 1]);
                int uz = u.size();
                for (int j = 0; j < uz; j++)
                    stopover_time.push_back(toint(u[j]));
            } else if (words[i] == "-y")
                type = words[i + 1];
            else if (words[i] == "-x")
                start_time = tomin(words[i + 1]);
            else if (words[i] == "-d") {
                sjtu::vector<string> u = divi(words[i + 1]);
                start_sale = today(u[0]);
                end_sale = today(u[1]);
            }
        }
        return words[0] + " " +
               trainManagement.add_train(trainid, station_num,
                                         seat_num, station_name, prices,
                                         start_time, travel_time, stopover_time,
                                         start_sale, end_sale, type, toint(words[0]));
    }
    if (words[1] == "delete_train") {
        string trainid = "";
        for (int i = 2; i < z; i += 2)
            if (words[i] == "-i")
                trainid = words[i + 1];
        return words[0] + " " + trainManagement.delete_train(trainid);
    }
    if (words[1] == "release_train") {
        string trainid = "";
        for (int i = 2; i < z; i += 2)
            if (words[i] == "-i")
                trainid = words[i + 1];
        return words[0] + " " + trainManagement.release_train(trainid);
    }
    if (words[1] == "query_train") {
        string trainid = "";
        date depart;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-i")
                trainid = words[i + 1];
            else if (words[i] == "-d")
                depart = today(words[i + 1]);
        }
        return words[0] + " " + trainManagement.query_train(trainid, depart, toint(words[0]));
    }
    if (words[1] == "query_ticket") {
        string start_station = "", end_station = "", compare = "time";
        date depart;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-s")
                start_station = words[i + 1];
            else if (words[i] == "-t")
                end_station = words[i + 1];
            else if (words[i] == "-d")
                depart = today(words[i + 1]);
            else if (words[i] == "-p")
                compare = words[i + 1];
        }
        return words[0] + " " +
               trainManagement.query_ticket(start_station, end_station, depart, compare == "cost", toint(words[0]));
    }
    if (words[1] == "query_transfer") {
        string start_station = "", end_station = "", compare = "time";
        date depart;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-s")
                start_station = words[i + 1];
            else if (words[i] == "-t")
                end_station = words[i + 1];
            else if (words[i] == "-d")
                depart = today(words[i + 1]);
            else if (words[i] == "-p")
                compare = words[i + 1];
        }
        return words[0] + " " +
               trainManagement.query_transfer(start_station, end_station, depart, compare == "cost", toint(words[0]));
    }
    if (words[1] == "buy_ticket") {
//        std::cerr << toint(words[0]) << std::endl;
        string username = "", trainid = "", start = "", end = "", will = "false";
        date start_date;
        int ticket_num = 0;
        for (int i = 2; i < z; i += 2) {
//            std::cerr << i << std::endl;
            if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-i")
                trainid = words[i + 1];
            else if (words[i] == "-d")
                start_date = today(words[i + 1]);
            else if (words[i] == "-n")
                ticket_num = toint(words[i + 1]);
            else if (words[i] == "-f")
                start = words[i + 1];
            else if (words[i] == "-t")
                end = words[i + 1];
            else if (words[i] == "-q")
                will = words[i + 1];
        }
//        std::cerr << "enter" << std::endl;
        return words[0] + " " +
               orderManagement.buy_ticket(username, trainid, start_date, ticket_num,
                                          start, end, will == "true", toint(words[0]));
    }
    if (words[1] == "query_order") {
        string username = "";
        for (int i = 2; i < z; i += 2)
            if (words[i] == "-u")
                username = words[i + 1];
        return words[0] + " " + orderManagement.query_order(username);
    }
    if (words[1] == "refund_ticket") {
        string username = "";
        int num = 1;
        for (int i = 2; i < z; i += 2) {
            if (words[i] == "-u")
                username = words[i + 1];
            else if (words[i] == "-n")
                num = toint(words[i + 1]);
        }
        return words[0] + " " + orderManagement.refund_ticket(username, num);
    }
    if (words[1] == "rollback") {
    }
    if (words[1] == "clean") {
        userManagement.clean();
        trainManagement.clean();
        orderManagement.clean();
        return words[0] + " 0\n";
    }
    if (words[1] == "exit") {
        std::cout << words[0] << " bye\n";
        /* remove("bpt_users_file_tree");
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
        exit(0);
    }
}

#endif
