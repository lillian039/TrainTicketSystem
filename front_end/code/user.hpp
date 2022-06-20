#ifndef USER_HPP
#define USER_HPP

#include <iostream>

#include "string.hpp"
#include "linked_hashmap.hpp"
#include "bptree.hpp"
#include "vector.hpp"
#include "database.hpp"

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
         const int &_privilege=0) : username{_username}, password{_password},
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

    sjtu::linked_hashmap<int, bool> log_in;

public:

    user_management() : bpt_users("bpt_users"){}


    void clean() {
        bpt_users.clear();
        log_in.clear();
    }

    bool is_log_in(const int &hashe) {
        return log_in.find(hashe) != log_in.end();
    }
    
    user find(const int& hashe){
        return bpt_users.find(hashe).second;
    }

    string add_user(
            const string &username, const string &password,
            const string &realname, const string &mailaddr) {
        if (bpt_users.size()) {
            sjtu::my_str<20> u = username;
            if (bpt_users.find(u.toint()).first) return "这个用户名被用过了呦~"; //新用户已经注册
        }
        user new_user(username, password, realname, mailaddr);
        // return "呆呆同学祝贺您：哇~~~~注册成功！！！\n";
        bpt_users.insert(std::pair<int, user>(new_user.hash_username, new_user));
        return "哇~~~~注册成功！！！";
    }

    string login(const string &username, const string &password) {
        sjtu::my_str<20> u = username;
        int uh = u.toint();
        if (is_log_in(uh)) return "你怎么好意思再登录一次呀！！！"; //用户已登录
        std::pair<bool, user> x = bpt_users.find(uh);
        if (!x.first) return "你到底注没注册？"; //用户不存在
        sjtu::my_str<30> p = password;
        if (x.second.password.toint() != p.toint()) return "你是不是忘记密码啦啊哈哈哈"; //密码错误
        log_in[uh] = true;
        return "哇~~~~登录成功！！！";
    }

    string logout(const string &username,const string& password) {
        sjtu::my_str<20> u = username;
        int uh = u.toint();
        if (!is_log_in(uh)) return "你好像本来就莫得登录捏"; //没有登录
        user uu=bpt_users.find(uh).second;
        if (uu.password.tostr()!=password)
            return "密码错啦~~";
        log_in.erase(log_in.find(uh));
        return "登出成功~~~";
    }

    // string query_profile(const string &cur_username, const string &username) {
    //     sjtu::my_str<20> cu = cur_username, u = username;
    //     int ch = cu.toint(), uh = u.toint();
    //     if (!is_log_in(ch)) return "-1\n"; //原用户未登录
    //     std::pair<bool, user> xc = bpt_users.find(ch), xu = bpt_users.find(uh);
    //     if (!xc.first || !xu.first) return "-1\n"; //原用户或新用户不存在
    //     if (xc.second.privilege < xu.second.privilege) return "-1\n";
    //     if (xc.second.privilege == xu.second.privilege && ch != uh) return "-1\n";
    //     return username + " " + xu.second.realname.tostr() + " " +
    //            xu.second.mailaddr.tostr() + " " + std::to_string(xu.second.privilege) + "\n";
    // }

    string modify_profile(
            const string &username, const string &password, const string &realname,
            const string &mailaddr, const string &password_pre) {
        sjtu::my_str<20> u = username;
        int uh = u.toint();
        if (!is_log_in(uh)) return "你好像没有登录捏"; //原用户未登录
        user xu = bpt_users.find(uh).second;
        user nu = xu;
        if (nu.password.tostr()!=password_pre) return "原来的密码错啦！！！";
        if (password != "") nu.password = (sjtu::my_str<30>) password;
        if (realname != "") nu.realname = (sjtu::my_str<20>) realname;
        if (mailaddr != "") nu.mailaddr = (sjtu::my_str<30>) mailaddr;
        bpt_users.modify(std::pair<int, user>(uh, xu), nu);
        return "修改成功~~~";
    }

    // void rollback(const int &pre) {
    //     while (data_roll.size()) {
    //         std::pair<int, user_roll> v = data_roll.back();
    //         if (v.first < pre) break;
    //         user_roll u = v.second;
    //         if (u.type == 0) bpt_users.remove(std::pair<int, user>(u.pre.hash_username, u.pre));
    //         else bpt_users.modify(std::pair<int, user>(u.pre.hash_username, u.now), u.pre);
    //         data_roll.pop_back();
    //     }
    //     log_in.clear();
    // }
} userManagement;

#endif