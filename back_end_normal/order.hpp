#ifndef ORDER_HPP
#define ORDER_HPP

#include <iostream>

#include "string.hpp"
#include "date.hpp"
#include "user.hpp"
#include "train.hpp"
#include "bptree.hpp"

using std::string;

struct order {
    sjtu::my_str<20> username;
    sjtu::my_str<20> trainid;
    sjtu::my_str<40> start_station;
    sjtu::my_str<40> end_station;
    int price;
    int status;
    // 0 success; 1 pending; 2 refunded
    int ticket_num;
    date depart_time;
    date arrive_time;
    int dfn;
    int day;
    int tk_addr;
    int sth, eth;

    order() {}

    order(
            const string &_username, const string &_trainid,
            const string &start, const string &end,
            const int &_price, const int &_status,
            const int &_ticket_num, const date &depart,
            const date &arrive, const int &_dfn,
            const int &_day, const int &_tk_addr,
            const int &_sth, const int &_eth) : username{_username}, trainid{_trainid},
                                                start_station{start}, end_station{end},
                                                price{_price}, status{_status},
                                                ticket_num{_ticket_num}, depart_time{depart},
                                                arrive_time{arrive}, dfn{_dfn}, day{_day},
                                                tk_addr{_tk_addr}, sth{_sth}, eth{_eth} {}

    friend bool operator<(const order &lhs, const order &rhs) {
        return lhs.dfn < rhs.dfn;
    }

    friend bool operator>(const order &lhs, const order &rhs) {
        return lhs.dfn > rhs.dfn;
    }

    friend bool operator==(const order &lhs, const order &rhs) {
        return lhs.dfn == rhs.dfn;
    }

    friend bool operator>=(const order &lhs, const order &rhs) {
        return lhs.dfn >= rhs.dfn;
    }

    friend bool operator<=(const order &lhs, const order &rhs) {
        return lhs.dfn <= rhs.dfn;
    }

    friend bool operator!=(const order &lhs, const order &rhs) {
        return lhs.dfn != rhs.dfn;
    }
};

class order_management {
private:
    BPTree<int, order> bpt_user_order;
    //用户id的哈希值查询订单

    BPTree<std::pair<int, int>, order> bpt_train_order;
    // trainid的哈希值查询车次的候补队列
public:
    order_management() : bpt_user_order("bpt_user_order"),
                         bpt_train_order("bpt_train_order") {}

    void clean() {
        bpt_user_order.clear();
        bpt_train_order.clear();
    }

    string buy_ticket(
            const string &username, const string &trainid,
            const date &start_date, const int &ticket_num,
            const string &start, const string &end,
            const bool &will, const int &dfn) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "-1\n"; //未登录
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = trainManagement.find_train_information(th);
        if (!x.first) return "-1\n"; //没有这辆车
        train tr = trainManagement.find_train(x.second.first);
        if (!tr.release) return "-1\n"; //未发布车次
        sjtu::my_str<40> ss = start, es = end;
        int sh = ss.toint(), eh = es.toint(), sth = -1, eth = -1;
        for (int i = 0; i < tr.station_num; i++) {
            if (tr.stations[i].hashe == sh) sth = i;
            if (tr.stations[i].hashe == eh) eth = i;
        }
        if (sth == -1 || eth == -1 || sth >= eth) return "-1\n";
        int day = (start_date & tr.get_station_depart_time(sth));
        if (day < 0 || day > (tr.end_sale & tr.start_sale)) return "-1\n";
        date depart = tr.get_station_depart_time(sth) ^ day;
        date arrive = tr.get_station_arrive_time(eth) ^ day;
        ticket tk = trainManagement.find_ticket(x.second.second, day);
        if (tk.num < ticket_num) return "-1\n";
        int price = tr.stations[eth].price - tr.stations[sth].price;
        if (tk.qry(sth, eth - 1) >= ticket_num) {
            order o(username, trainid, start, end, price, 0, ticket_num,
                    depart, arrive, dfn, day, x.second.second, sth, eth);
            bpt_user_order.insert(std::pair<int, order>(uh, o));
            //bpt_train_order.insert(std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), o));
            tk.upd(sth, eth - 1, -ticket_num);
            trainManagement.modify(x.second.second, tk, day);
            return std::to_string(1ll * ticket_num * price) + "\n";
        }//满足
        else {
            if (will) {
                order o(username, trainid, start, end, price, 1, ticket_num,
                        depart, arrive, dfn, day, x.second.second, sth, eth);
                bpt_user_order.insert(std::pair<int, order>(uh, o));
                bpt_train_order.insert(std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), o));
                return "queue\n";
            } else return "-1\n"; //票太少了 不愿意加入候补队列
        }
    }

    string query_order(const string &username) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "-1\n"; //未登录
        sjtu::vector<order> u = bpt_user_order.Find(uh);
        int z = u.size();
        string ans = std::to_string(z) + "\n";
        for (int i = z - 1; i >= 0; i--) {
            order x = u[i];
            if (x.status == 0) ans += "[success] ";
            if (x.status == 1) ans += "[pending] ";
            if (x.status == 2) ans += "[refunded] ";
            ans += x.trainid.tostr() + " " + x.start_station.tostr() + " " + x.depart_time.tostr() + " -> ";
            ans += x.end_station.tostr() + " " + x.arrive_time.tostr() + " ";
            ans += std::to_string(x.price) + " " + std::to_string(x.ticket_num) + "\n";
        }
        return ans;
    }

    string refund_ticket(const string &username, const int &num) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "-1\n";
        sjtu::vector<order> u = bpt_user_order.Find(uh);
        int z = u.size();
        if (num > z) return "-1\n"; //超过订单数量
        if (u[z - num].status == 2) return "-1\n"; //已经退了
        order x = u[z - num];
        bool is_change = (u[z - num].status == 0);
        x.status = 2;
        bpt_user_order.modify(std::pair<int, order>(uh, u[z - num]), x);
        int th = x.trainid.toint(), day = x.day;
        if (!is_change)
            bpt_train_order.remove(
                    std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), u[z - num]));
        else {
            u = bpt_train_order.Find(std::pair<int, int>(th, day));
            z = u.size();
            ticket tk = trainManagement.find_ticket(x.tk_addr, day);
            tk.upd(x.sth, x.eth - 1, x.ticket_num);
            for (int i = 0; i < z; i++) {
                if (tk.qry(u[i].sth, u[i].eth - 1) >= u[i].ticket_num) {
                    int uh = u[i].username.toint();
                    tk.upd(u[i].sth, u[i].eth - 1, -u[i].ticket_num);
                    order y = u[i];
                    y.status = 0;
                    bpt_user_order.modify(std::pair<int, order>(uh, u[i]), y);
                    bpt_train_order.remove(
                            std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), u[i]));
                }
            }
            trainManagement.modify(x.tk_addr, tk, day);
        }
        return "0\n";
    }
} orderManagement;

#endif
