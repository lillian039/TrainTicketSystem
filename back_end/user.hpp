#ifndef USER_HPP
#define USER_HPP

#include <iostream>

#include "string.hpp"
#include "linked_hashmap.hpp"
#include "bptree.hpp"
#include "vector.hpp"

using std::string;

struct user {
    sjtu::my_str<20> username;
    sjtu::my_str<30> password;
    sjtu::my_str<20> realname;
    sjtu::my_str<30> mailaddr;
    int privilege = 0;
    int hash_username;

    user() {}

    user(const string &_username, const string &_password,
         const string &_realname, const string &_mailaddr,
         const int &_privilege) : username{_username}, password{_password},
                                  realname{_realname}, mailaddr{_mailaddr},
                                  privilege{_privilege} {
        hash_username = username.toint();
    }

    ~user() {}

    friend bool operator<(const user &lhs, const user &rhs) {
        return lhs.hash_username < rhs.hash_username;
    }

    friend bool operator>(const user &lhs, const user &rhs) {
        return lhs.hash_username > rhs.hash_username;
    }

    friend bool operator==(const user &lhs, const user &rhs) {
        return lhs.hash_username == rhs.hash_username;
    }

    friend bool operator!=(const user &lhs, const user &rhs) {
        return lhs.hash_username != rhs.hash_username;
    }

    friend bool operator<=(const user &lhs, const user &rhs) {
        return lhs.hash_username <= rhs.hash_username;
    }

    friend bool operator>=(const user &lhs, const user &rhs) {
        return lhs.hash_username >= rhs.hash_username;
    }
};

class user_management {
private:
    BPTree<int, user> bpt_users;
    //哈希值对应用户
    BPTree<int, std::pair<int, std::pair<user, user>>> bpt_roll;
    //操作类型，前后user

    sjtu::linked_hashmap<int, bool> log_in;

public:

    user_management() : bpt_users("bpt_users"), bpt_roll("bpt_roll") {}


    void clean() {
        bpt_users.clear();
        log_in.clear();
    }

    bool is_log_in(const int &hashe) {
        return log_in.find(hashe) != log_in.end();
    }

    string add_user(
            const string &cur_username, const string &username,
            const string &password, const string &realname,
            const string &mailaddr, int privilege, const int &dfn) {
        if (!bpt_users.size()) privilege = 10; //还没有任何用户注册
        else {
            sjtu::my_str<20> cu = cur_username;
            int ch = cu.toint();
            if (!is_log_in(ch)) return "-1\n"; //原用户未登录
            user x = bpt_users.find(ch).second;
            if (x.privilege <= privilege) return "-1\n"; //权限限制
            sjtu::my_str<20> u = username;
            if (bpt_users.find(u.toint()).first) return "-1\n"; //新用户已经注册
        }
        user new_user(username, password, realname, mailaddr, privilege);
        bpt_users.insert(std::pair<int, user>(new_user.hash_username, new_user));
        bpt_roll.insert(std::pair<int, std::pair<int, std::pair<user, user>>>(
                dfn, std::pair<int, std::pair<user, user>>(0, std::pair<user, user>(new_user, new_user))));
        return "0\n";
    }

    string login(const string &username, const string &password) {
        sjtu::my_str<20> u = username;
        int uh = u.toint();
        if (is_log_in(uh)) return "-1\n"; //用户已登录
        std::pair<bool, user> x = bpt_users.find(uh);
        if (!x.first) return "-1\n"; //用户不存在
        sjtu::my_str<30> p = password;
        if (x.second.password.toint() != p.toint()) return "-1\n"; //密码错误
        log_in[uh] = true;
        return "0\n";
    }

    string logout(const string &username) {
        sjtu::my_str<20> u = username;
        int uh = u.toint();
        if (!is_log_in(uh)) return "-1\n"; //没有登录
        log_in.erase(log_in.find(uh));
        return "0\n";
    }

    string query_profile(const string &cur_username, const string &username) {
        sjtu::my_str<20> cu = cur_username, u = username;
        int ch = cu.toint(), uh = u.toint();
        if (!is_log_in(ch)) return "-1\n"; //原用户未登录
        std::pair<bool, user> xc = bpt_users.find(ch), xu = bpt_users.find(uh);
        if (!xc.first || !xu.first) return "-1\n"; //原用户或新用户不存在
        if (xc.second.privilege < xu.second.privilege) return "-1\n";
        if (xc.second.privilege == xu.second.privilege && ch != uh) return "-1\n";
        return username + " " + xu.second.realname.tostr() + " " +
               xu.second.mailaddr.tostr() + " " + std::to_string(xu.second.privilege) + "\n";
    }

    string modify_profile(
            const string &cur_username, const string &username,
            const string &password, const string &realname,
            const string &mailaddr, const int &privilege, const int &dfn) {
        sjtu::my_str<20> cu = cur_username, u = username;
        int ch = cu.toint(), uh = u.toint();
        if (!is_log_in(ch)) return "-1\n"; //原用户未登录
        std::pair<bool, user> xc = bpt_users.find(ch), xu = bpt_users.find(uh);
        if (!xc.first || !xu.first) return "-1\n"; //原用户或新用户不存在
        if (xc.second.privilege < xu.second.privilege) return "-1\n";
        if (xc.second.privilege == xu.second.privilege && ch != uh) return "-1\n";
        if (xc.second.privilege <= privilege) return "-1\n"; //修改后的权限要小于原用户
        user nu = xu.second;
        if (password != "") nu.password = (sjtu::my_str<30>) password;
        if (realname != "") nu.realname = (sjtu::my_str<20>) realname;
        if (mailaddr != "") nu.mailaddr = (sjtu::my_str<30>) mailaddr;
        if (privilege >= 0) nu.privilege = privilege;
        bpt_users.modify(std::pair<int, user>(uh, xu.second), nu);
        bpt_roll.insert(std::pair<int, std::pair<int, std::pair<user, user>>>(
                dfn, std::pair<int, std::pair<user, user>>(1, std::pair<user, user>(xu.second, nu))));
        return username + " " + nu.realname.tostr() +
               " " + nu.mailaddr.tostr() + " " + std::to_string(nu.privilege) + "\n";
    }

    void rollback(const int &pre) {
        sjtu::vector<std::pair<int, std::pair<int, std::pair<user, user>>>> v = bpt_roll.roll(pre);
        int z = v.size();
        for (int i = z - 1; i >= 0; i--) {
            std::pair<int, std::pair<user, user>> u = v[i].second;
            if (u.first == 0) bpt_users.remove(std::pair<int, user>(u.second.first.hash_username, u.second.first));
            else bpt_users.modify(std::pair<int, user>(u.second.first.hash_username, u.second.second), u.second.first);
            bpt_roll.remove(v[i]);
        }
        log_in.clear();
    }
} userManagement;

#endif