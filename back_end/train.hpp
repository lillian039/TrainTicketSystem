#ifndef TRAIN_HPP
#define TRAIN_HPP

#include <iostream>

#include "string.hpp"
#include "date.hpp"
#include "bptree_roll.hpp"
#include "database.hpp"
#include "vector.hpp"

using std::min;
using std::string;

struct station {
    sjtu::my_str<40> name;
    int hashe;
    int price;//从起始站到这个站票价总和
    int arrive_time; //从起始站出发到这个站所用的时间 分钟
    int depart_time; //从起始站出发到这个站开始出发所用的时间 分钟

    station() {}

    friend bool operator<(const station &lhs, const station &rhs) {
        return lhs.hashe < rhs.hashe;
    }

    friend bool operator>(const station &lhs, const station &rhs) {
        return lhs.hashe > rhs.hashe;
    }

    friend bool operator==(const station &lhs, const station &rhs) {
        return lhs.hashe == rhs.hashe;
    }

    friend bool operator!=(const station &lhs, const station &rhs) {
        return lhs.hashe != rhs.hashe;
    }

    friend bool operator<=(const station &lhs, const station &rhs) {
        return lhs.hashe <= rhs.hashe;
    }

    friend bool operator>=(const station &lhs, const station &rhs) {
        return lhs.hashe >= rhs.hashe;
    }
};

struct train {
    sjtu::my_str<20> trainid;
    int station_num;
    date start_sale;
    date end_sale;
    int start_time; //分钟
    sjtu::my_str<1> type;
    station stations[100];
    bool release;
    int hashe;

    train() {}

    train(
            const string &_trainid, const int &_station_num,
            const sjtu::vector<string> &_station_name,
            const sjtu::vector<int> &prices, const date &_start_time,
            const sjtu::vector<int> &travel_time,
            const sjtu::vector<int> &stopover_time, const date &_start_sale,
            const date &_end_sale, const string &_type) : trainid{_trainid}, station_num{_station_num},
                                                          start_sale{_start_sale}, end_sale{_end_sale}, type{_type} {
        hashe = trainid.toint();
        release = 0;
        start_time = _start_time.to_min();
        int price = 0, arrive_time = 0, depart_time = 0;
        for (int i = 0; i < station_num; i++) {
            if (i > 0 && i < station_num - 1)
                depart_time += stopover_time[i - 1];
            stations[i].name = _station_name[i];
            stations[i].hashe = stations[i].name.toint();
            stations[i].price = price;
            stations[i].arrive_time = arrive_time;
            stations[i].depart_time = depart_time;
            if (i < station_num - 1)
                price += prices[i], depart_time += travel_time[i];
            arrive_time = depart_time;
        }
    }

    date get_station_depart_time(int i) const {
        return start_sale + (start_time + stations[i].depart_time);
    }

    date get_station_arrive_time(int i) const {
        return start_sale + (start_time + stations[i].arrive_time);
    }

    friend bool operator<(const train &lhs, const train &rhs) {
        return lhs.hashe < rhs.hashe;
    }

    friend bool operator>(const train &lhs, const train &rhs) {
        return lhs.hashe > rhs.hashe;
    }

    friend bool operator==(const train &lhs, const train &rhs) {
        return lhs.hashe == rhs.hashe;
    }

    friend bool operator!=(const train &lhs, const train &rhs) {
        return lhs.hashe != rhs.hashe;
    }

    friend bool operator<=(const train &lhs, const train &rhs) {
        return lhs.hashe <= rhs.hashe;
    }

    friend bool operator>=(const train &lhs, const train &rhs) {
        return lhs.hashe >= rhs.hashe;
    }
};

struct ticket {
    int a[100], num;

    ticket() {}

    ticket(const int &station_num, const int &seat_num) {
        num = seat_num;
        for (int i = 0; i < station_num - 1; i++)
            a[i] = num;
    }

    void upd(int l, int r, int v) {
        for (int i = l; i <= r; i++)
            a[i] += v;
    }

    int qry(int l, int r) {
        int v = 1e9;
        for (int i = l; i <= r; i++)
            v = min(a[i], v);
        return v;
    }
};

struct train_roll {
    int type, tr, tk;

    friend bool operator<(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type < rhs.type;
    }

    friend bool operator>(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type > rhs.type;
    }

    friend bool operator==(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type == rhs.type;
    }

    friend bool operator!=(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type != rhs.type;
    }

    friend bool operator<=(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type <= rhs.type;
    }

    friend bool operator>=(const train_roll &lhs, const train_roll &rhs) {
        return lhs.type >= rhs.type;
    }
};

class train_management {
private:
    BPTree<int, std::pair<int, int>> bpt_station_train;
    //每个车站的哈希值对应哪些车次id，以及它在这个车次中是第几个站

    BPTree<int, std::pair<int, int>> bpt_train_information;
    //每个车次的哈希值对应数据库中位置 fisrt 对应火车信息，second对应车票信息

    Database<train> train_information;
    //找到对应的火车信息

    Database<ticket> train_ticket;
    //找到对应的车票信息

    BPTree<int, train_roll> bpt_train_roll;
    //操作类型，车次的idx，票的idx

    void sort(
            int l, int r, sjtu::vector<int> &id,
            const sjtu::vector<string> trainid,
            const sjtu::vector<int> &judge) {
        if (l == r) return;
        int mid = ((l + r) >> 1);
        sort(l, mid, id, trainid, judge);
        sort(mid + 1, r, id, trainid, judge);
        int *arr = new int[r - l + 1];
        for (int i = l, j = mid + 1, k = l; k <= r; k++) {
            if (j > r || (i <= mid && (judge[id[i]] < judge[id[j]] ||
                                       (judge[id[i]] == judge[id[j]] &&
                                        trainid[id[i]] < trainid[id[j]]))))
                arr[k - l] = id[i++];
            else arr[k - l] = id[j++];
        }
        for (int i = l; i <= r; i++) id[i] = arr[i - l];
        delete[] arr;
    }

public:
    train_management() : bpt_station_train("bpt_station_train"),
                         bpt_train_information("bpt_train_information"),
                         train_information("train_information"),
                         train_ticket("train_ticket"),
                         bpt_train_roll("bpt_train_roll") {}

    void clean() {
        bpt_station_train.clear();
        bpt_train_information.clear();
        train_information.clear();
        train_ticket.clear();
    }

    std::pair<bool, std::pair<int, int>> find_train_information(const int &th) {
        return bpt_train_information.find(th);
    }

    train find_train(const int &tr_addr) {
        return train_information.find(tr_addr);
    }

    int insert_ticket(const ticket &tk) {
        return train_ticket.insert(tk);
    }

    ticket find_ticket(const int &tk_addr, const int &day = 0) {
        return train_ticket.find(tk_addr, day);
    }

    void modify(const int &tk_addr, const ticket &tk, const int &day) {
        train_ticket.modify(tk, tk_addr, day);
    }

    string add_train(
            const string &trainid, const int &station_num,
            const int &seat_num, const sjtu::vector<string> &station_name,
            const sjtu::vector<int> &prices, const date &start_time,
            const sjtu::vector<int> &travel_time, const sjtu::vector<int> &stopover_time,
            const date &start_sale, const date &end_sale, const string &type, const int &dfn) {
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
        if (x.first) return "-1\n"; // trainid已经存在了
        train u(trainid, station_num, station_name, prices, start_time,
                travel_time, stopover_time, start_sale, end_sale, type);
        int tr = train_information.insert(u);
        ticket v(station_num, seat_num);
        int tk = train_ticket.insert(v);
        for (int i = (end_sale & start_sale); i > 0; i--) train_ticket.insert(v);
        bpt_train_information.insert(std::pair<int, std::pair<int, int>>(u.hashe, std::pair<int, int>(tr, tk)));
        for (int i = 0; i < station_num; i++)
            bpt_station_train.insert(
                    std::pair<int, std::pair<int, int>>(u.stations[i].hashe, std::pair<int, int>(u.hashe, i)));
        bpt_train_roll.insert(std::pair<int, train_roll>(dfn, (train_roll) {0, tr, tk}));
        return "0\n";
    }

    string delete_train(const string &trainid, const int &dfn) {
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
        if (!x.first) return "-1\n"; // trainid不存在
        train u = train_information.find(x.second.first);
        if (u.release) return "-1\n"; //车次已发布
        bpt_train_information.remove(std::pair<int, std::pair<int, int>>(th, x.second));
        for (int i = 0; i < u.station_num; i++)
            bpt_station_train.remove(
                    std::pair<int, std::pair<int, int>>(u.stations[i].hashe, std::pair<int, int>(u.hashe, i)));
        bpt_train_roll.insert(std::pair<int, train_roll>(dfn, (train_roll) {1, x.second.first, x.second.second}));
        return "0\n";
    }

    string release_train(const string &trainid, const int &dfn) {
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
        if (!x.first) return "-1\n"; // trainid 不存在
        train u = train_information.find(x.second.first);
        if (u.release) return "-1\n"; //车次已发布
        u.release = 1;
        train_information.modify(u, x.second.first);
        bpt_train_roll.insert(std::pair<int, train_roll>(dfn, (train_roll) {2, x.second.first, x.second.second}));
        return "0\n";
    }

    //发布前的车次，不可发售车票，无法被query_ticket和query_transfer操作所查询到
    string query_train(const string &trainid, const date &depart) {
        sjtu::my_str<20> ti = trainid;
        int th = ti.toint();
        std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
        if (!x.first) return "-1\n"; // trainid 不存在
        train u = train_information.find(x.second.first); //未发布
        int day = (depart & u.start_sale);
        int allday = (u.end_sale & u.start_sale);
        if (day < 0 || day > allday) return "-1\n"; //没有从这一天出发
        ticket v = train_ticket.find(x.second.second, day);
        string ans = u.trainid.tostr() + " " + u.type.tostr() + "\n";
        for (int i = 0; i < u.station_num; i++) {
            ans += u.stations[i].name.tostr() + " ";
            if (i == 0) ans += "xx-xx xx:xx -> ";
            else ans += (u.get_station_arrive_time(i) ^ day).tostr() + " -> ";
            if (i == u.station_num - 1) ans += "xx-xx xx:xx ";
            else ans += (u.get_station_depart_time(i) ^ day).tostr() + " ";
            ans += std::to_string(u.stations[i].price) + " ";
            if (i == u.station_num - 1) ans += "x\n";
            else ans += std::to_string(v.a[i]) + "\n";
        }
        //格式：中院 07-02 05:19 -> 07-02 05:24 114 1000
        return ans;
    }

    string query_ticket(
            const string &start_station, const string &end_station,
            const date &depart, const bool &compare) // compare=0按照time排序，compare=1按照cost排序
    {
        sjtu::my_str<40> sn(start_station), en(end_station);
        int sh = sn.toint(), eh = en.toint();
        sjtu::vector<std::pair<int, int>> st = bpt_station_train.Find(sh);//车次 它在车次中的位置
        sjtu::vector<std::pair<int, int>> et = bpt_station_train.Find(eh);
        sjtu::vector<string> trainid;
        sjtu::vector<std::pair<date, date>> time;
        sjtu::vector<int> price, dtime, seat_num, id;
        int sz = st.size(), ez = et.size(), z = 0;
        for (int si = 0, ei = 0; si < sz && ei < ez;) {
            if (st[si].first < et[ei].first) si++;
            else if (st[si].first > et[ei].first) ei++;
            else {
                if (st[si].second < et[ei].second) {
                    std::pair<int, int> x = bpt_train_information.find(st[si].first).second;
                    train u = train_information.find(x.first);
                    int day = (depart & u.get_station_depart_time(st[si].second));
                    if (u.release && day <= (u.end_sale & u.start_sale) && day >= 0) {
                        seat_num.push_back(
                                train_ticket.find(x.second, day).qry(st[si].second, et[ei].second - 1));
                        id.push_back(z++);
                        price.push_back(u.stations[et[ei].second].price - u.stations[st[si].second].price);
                        time.push_back(std::pair<date, date>(
                                u.get_station_depart_time(st[si].second) ^ day,
                                u.get_station_arrive_time(et[ei].second) ^ day));
                        dtime.push_back(
                                u.get_station_arrive_time(et[ei].second) - u.get_station_depart_time(st[si].second));
                        trainid.push_back(u.trainid.tostr());
                    }
                }
                si++, ei++;
            }
        }
        if (z) sort(0, z - 1, id, trainid, compare ? price : dtime);
        string ans = std::to_string(z) + "\n";
        for (int i = 0, u; i < z; i++) {
            u = id[i];
            ans += trainid[u] + " " + start_station + " " + time[u].first.tostr() + " -> " +
                   end_station + " " + time[u].second.tostr() + " " +
                   std::to_string(price[u]) + " " + std::to_string(seat_num[u]) + "\n";
        }
        return ans;
        //格式：HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
    }

    string query_transfer(
            const string &start_station, const string &end_station,
            const date &depart, const bool &compare) {
        sjtu::my_str<40> sn(start_station), en(end_station);
        int sh = sn.toint(), eh = en.toint();
        sjtu::vector<std::pair<int, int>> st = bpt_station_train.Find(sh);
        sjtu::vector<std::pair<int, int>> et = bpt_station_train.Find(eh);
        string trainid[2], _start_station[2], _end_station[2];
        _start_station[0] = start_station, _end_station[1] = end_station;
        std::pair<date, date> time[2];
        int price[2], seat_num[2], sz = st.size(), ez = et.size();
        bool fl = 0;
        sjtu::vector<std::pair<train, int>> etr; //合法的车次，在车次中是第几个站
        sjtu::vector<std::pair<int, int>> eti; //车次的idx和车票的idx
        for (int ei = 0; ei < ez; ei++) {
            std::pair<int, int> y = bpt_train_information.find(et[ei].first).second;
            train v = train_information.find(y.first);
            if (v.release && et[ei].second > 0)
                eti.push_back(y), etr.push_back(std::pair<train, int>(v, et[ei].second));
        }
        ez = eti.size();
        for (int si = 0; si < sz; si++) {
            std::pair<int, int> x = bpt_train_information.find(st[si].first).second;
            train u = train_information.find(x.first);
            if (!u.release || st[si].second == u.station_num - 1) continue;
            sjtu::linked_hashmap<int, std::pair<int, int>> mp;//车站hashe 映射到前一个车次的第几站和位置个数
            int day = (depart & u.get_station_depart_time(st[si].second));
            if (day < 0 || day > (u.end_sale & u.start_sale)) continue;
            ticket tk = train_ticket.find(x.second, day);
            int _seat_num = tk.a[st[si].second];
            for (int i = st[si].second + 1; i < u.station_num; i++) {
                mp[u.stations[i].hashe] = std::pair<int, int>(i, _seat_num);
                if (i < u.station_num - 1)
                    _seat_num = min(_seat_num, tk.a[i]);
            }
            for (int ei = 0; ei < ez; ei++) {
                if (u == etr[ei].first) continue;
                std::pair<int, int> y = eti[ei];
                train v = etr[ei].first;
                int day_ = -1;
                ticket tk;
                for (int i = etr[ei].second - 1; i >= 0; i--) {
                    if (mp.find(v.stations[i].hashe) == mp.end()) continue;
                    std::pair<int, int> z = mp[v.stations[i].hashe]; //中间站在前一个车次的第几站，位置个数
                    date arrive_time = (u.get_station_arrive_time(z.first) ^ day);
                    date last_depart_time = v.get_station_depart_time(i) ^ (v.end_sale & v.start_sale);
                    if (arrive_time.to_min() > last_depart_time.to_min()) continue;
                    date depart_time = last_depart_time ^ (arrive_time & last_depart_time);
                    if (depart_time.to_min() < arrive_time.to_min())
                        depart_time = depart_time ^ 1;
                    if (depart_time.to_min() < v.get_station_depart_time(i).to_min())
                        depart_time = v.get_station_depart_time(i);
                    int _day = (depart_time & v.get_station_depart_time(i));
                    if (_day != day_) tk = train_ticket.find(y.second, _day);
                    _seat_num = tk.qry(i, etr[ei].second - 1);
                    int price1 = u.stations[z.first].price - u.stations[st[si].second].price;
                    int price2 = v.stations[etr[ei].second].price - v.stations[i].price;
                    int _time = (v.get_station_arrive_time(etr[ei].second) ^ _day) -
                                (u.get_station_depart_time(st[si].second) ^ day);
                    if (fl) {
                        if (!compare) {
                            if ((time[1].second - time[0].first) < _time) continue;
                            if ((time[1].second - time[0].first) == _time &&
                                price[0] + price[1] < price1 + price2)
                                continue;
                        } else {
                            if (price[0] + price[1] < price1 + price2) continue;
                            if (price[0] + price[1] == price1 + price2 &&
                                (time[1].second - time[0].first) < _time)
                                continue;
                        }
                        if (price[0] + price[1] == price1 + price2 && (time[1].second - time[0].first) == _time &&
                            trainid[0] < u.trainid.tostr())
                            continue;
                        if (price[0] + price[1] == price1 + price2 && (time[1].second - time[0].first) == _time &&
                            trainid[0] == u.trainid.tostr() && trainid[1] < v.trainid.tostr())
                            continue;
                    }
                    price[0] = price1, price[1] = price2;
                    seat_num[0] = z.second, seat_num[1] = _seat_num;
                    time[0] = std::pair<date, date>(u.get_station_depart_time(st[si].second) ^ day,
                                                    u.get_station_arrive_time(z.first) ^ day);
                    time[1] = std::pair<date, date>(v.get_station_depart_time(i) ^ _day,
                                                    v.get_station_arrive_time(etr[ei].second) ^ _day);
                    trainid[0] = u.trainid.tostr(), trainid[1] = v.trainid.tostr();
                    _start_station[1] = _end_station[0] = v.stations[i].name.tostr();
                    fl = 1;
                }
            }
        }
        string ans = "";
        if (!fl) ans = "0\n";
        else
            for (int u = 0; u < 2; u++)
                ans += trainid[u] + " " + _start_station[u] + " " + time[u].first.tostr() + " -> " +
                       _end_station[u] + " " + time[u].second.tostr() + " " +
                       std::to_string(price[u]) + " " + std::to_string(seat_num[u]) + "\n";
        return ans;
        //格式：HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
    }

    void rollback(const int &pre) {
        while(bpt_train_roll.size()){
            std::pair<int, train_roll> v=bpt_train_roll.find_max();
            if (v.first<pre) break;
            train_roll u = v.second;
            if (u.type == 0) {
                train x = train_information.find(u.tr);
                for (int j = 0; j < x.station_num; j++)
                    bpt_station_train.remove(
                            std::pair<int, std::pair<int, int>>(x.stations[j].hashe, std::pair<int, int>(x.hashe, j)));
                bpt_train_information.remove(
                        std::pair<int, std::pair<int, int>>(x.hashe, std::pair<int, int>(u.tr, u.tk)));
            } else if (u.type == 1) {
                train x = train_information.find(u.tr);
                for (int j = 0; j < x.station_num; j++)
                    bpt_station_train.insert(
                            std::pair<int, std::pair<int, int>>(x.stations[j].hashe, std::pair<int, int>(x.hashe, j)));
                bpt_train_information.insert(
                        std::pair<int, std::pair<int, int>>(x.hashe, std::pair<int, int>(u.tr, u.tk)));
            } else {
                train x = train_information.find(u.tr);
                x.release = 0;
                train_information.modify(x, u.tr);
            }
            bpt_train_roll.remove(v);
        }
    }
} trainManagement;

#endif
