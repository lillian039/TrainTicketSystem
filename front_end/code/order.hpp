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
            const bool &will) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "你是不是忘了登录呀"; //未登录
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = trainManagement.find_train_information(th);
        if (!x.first) return "好像没有这辆车捏"; //没有这辆车
        train tr = trainManagement.find_train(x.second.first);
        if (!tr.release) return "这辆车没有发布！无法购买"; //未发布车次
        sjtu::my_str<40> ss = start, es = end;
        int sh = ss.toint(), eh = es.toint(), sth = -1, eth = -1;
        for (int i = 0; i < tr.station_num; i++) {
            if (tr.stations[i].hashe == sh) sth = i;
            if (tr.stations[i].hashe == eh) eth = i;
        }
        if (sth == -1 || eth == -1) return "这辆车没有这两站www";
        if (sth >= eth) return "你好像买反了噢~";
        int day = (start_date & tr.get_station_depart_time(sth));
        if (day < 0 || day > (tr.end_sale & tr.start_sale)) return "这辆车没有在这天出发噢~";
        date depart = tr.get_station_depart_time(sth) ^ day;
        date arrive = tr.get_station_arrive_time(eth) ^ day;
        std::cout<<"????"<<std::endl;
        ticket tk = trainManagement.find_ticket(x.second.second, day);
        if (tk.num < ticket_num) return "总的都没有这么多票给你买辣！";
        int price = tr.stations[eth].price - tr.stations[sth].price;
        if (tk.qry(sth, eth - 1) >= ticket_num) {
            int dfn=bpt_user_order.size();
            order o(username, trainid, start, end, price, 0, ticket_num,
                    depart, arrive, dfn, day, x.second.second, sth, eth);
            bpt_user_order.insert(std::pair<int, order>(uh, o));
            tk.upd(sth, eth - 1, -ticket_num);
            trainManagement.modify(x.second.second, tk, day);
            return "恭喜购票成功！！！，支付了"+std::to_string(1ll * ticket_num * price) + "元";
        } else {
            if (will) {
                int dfn=bpt_user_order.size();
                order o(username, trainid, start, end, price, 1, ticket_num,
                        depart, arrive, dfn, day, x.second.second, sth, eth);
                bpt_user_order.insert(std::pair<int, order>(uh, o));
                bpt_train_order.insert(std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), o));
                return "呜呜呜票数不够，加入候补队列喽";
            } else return "票数不够啦~下次再来啊哈哈"; //票太少了 不愿意加入候补队列
        }
    }

    string query_order(const string &username,const string& password) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "您未登录噢~<br>"; //未登录
        user uu=userManagement.find(uh);
        if (uu.password.tostr()!=password) return "你是不是忘了密码呀~<br>";
        sjtu::vector<order> u = bpt_user_order.Find(uh);
        int z = u.size();
        string ans = "哇~~查询成功！下面是你的订单信息<br>"+std::to_string(z) + "<br>";
        for (int i = z - 1; i >= 0; i--) {
            order x = u[i];
            if (x.status == 0) ans += "[success] ";
            if (x.status == 1) ans += "[pending] ";
            if (x.status == 2) ans += "[refunded] ";
            ans += x.trainid.tostr() + " " + x.start_station.tostr() + " " + x.depart_time.tostr() + " -> ";
            ans += x.end_station.tostr() + " " + x.arrive_time.tostr() + " ";
            ans += std::to_string(x.price) + " " + std::to_string(x.ticket_num) + "<br>";
        }
        return ans;
    }

    string refund_ticket(const string &username, const int &num) {
        sjtu::my_str<20> un = username;
        int uh = un.toint();
        if (!userManagement.is_log_in(uh)) return "你好像没有登录噢~";
        sjtu::vector<order> u = bpt_user_order.Find(uh);
        int z = u.size();
        if (num > z) return "没有这么多的订单噢~"; //超过订单数量
        if (u[z - num].status == 2) return "你是不是忘了这个订单已经退过了呀~"; //已经退了
        order x = u[z - num];
        bool is_change = (u[z - num].status == 0);
        x.status = 2;
        bpt_user_order.modify(std::pair<int, order>(uh, u[z - num]), x);
        int th = x.trainid.toint(), day = x.day;
        if (!is_change) {
            bpt_train_order.remove(std::pair<std::pair<int, int>, order>(std::pair<int, int>(th, day), u[z - num]));
        } else {
            u = bpt_train_order.Find(std::pair<int, int>(th, day));
            z = u.size();
            ticket tk = trainManagement.find_ticket(x.tk_addr, day);
            tk.upd(x.sth, x.eth - 1, x.ticket_num);
            for (int i = 0; i < z; i++) {
                if (tk.qry(u[i].sth, u[i].eth - 1) >= u[i].ticket_num) {
                    uh = u[i].username.toint();
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
        return "恭喜您：退票成功~~~";
    }

    // void rollback(const int &pre) {
    //     while (data_ticket_roll.size()) {
    //         std::pair<int, ticket_roll> u = data_ticket_roll.back();
    //         if (u.first < pre) break;
    //         ticket_roll x = u.second;
    //         trainManagement.modify(x.idx, trainManagement.find_ticket(x.idxpre), x.day);
    //         data_ticket_roll.pop_back();
    //     }
    //     while (data_user_order_roll.size()) {
    //         std::pair<int, user_order_roll> v = data_user_order_roll.back();
    //         if (v.first < pre) break;
    //         user_order_roll x = v.second;
    //         if (x.type == 0) bpt_user_order.remove(std::pair<int, order>(x.hashe, x.pre));
    //         else bpt_user_order.modify(std::pair<int, order>(x.hashe, x.now), x.pre);
    //         data_user_order_roll.pop_back();
    //     }
    //     while (data_train_order_roll.size()) {
    //         std::pair<int, train_order_roll> w = data_train_order_roll.back();
    //         if (w.first < pre) break;
    //         train_order_roll x = w.second;
    //         if (x.type == 0)
    //             bpt_train_order.remove(std::pair<std::pair<int, int>, order>(std::pair<int, int>(x.hashe, x.day), x.o));
    //         else
    //             bpt_train_order.insert(std::pair<std::pair<int, int>, order>(std::pair<int, int>(x.hashe, x.day), x.o));
    //         data_train_order_roll.pop_back();
    //     }
    // }

} orderManagement;

#endif
