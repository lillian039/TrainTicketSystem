#ifndef USER_HPP
#define USER_HPP

#include <iostream>

#include "string.hpp"
#include "linked_hashmap.hpp"
#include "bptree.hpp"
#include "error.hpp"

using std::string;

class user
{
private:
	sjtu::my_str<20> username;
	sjtu::my_str<30> password;
	sjtu::my_str<15> realname;
	sjtu::my_str<30> mailaddr;
	int privilege = 0;
	int hash_username, hash_password;

public:
	user() {}

	user(const string &_username, const string &_password,
		 const string &_realname, const string &_mailaddr,
		 const int &_privilege) : username{_username}, password{_password},
								  realname{_realname}, mailaddr{_mailaddr},
								  privilege{_privilege}
	{
		hash_username = username.toint();
		hash_password = password.toint();
	}

	~user() {}

	friend bool operator<(const user &lhs, const user &rhs)
	{
		return lhs.hash_username < rhs.hash_username;
	}

	friend bool operator>(const user &lhs, const user &rhs)
	{
		return lhs.hash_username > rhs.hash_username;
	}

	friend bool operator==(const user &lhs, const user &rhs)
	{
		return lhs.hash_username == rhs.hash_username;
	}

	friend bool operator!=(const user &lhs, const user &rhs)
	{
		return lhs.hash_username != rhs.hash_username;
	}

	string get_password() const { return username.tostr(); }

	string get_realname() const { return realname.tostr(); }

	string get_mailaddr() const { return mailaddr.tostr(); }

	int get_privilege() const { return privilege; }

	int get_hash_username() const { return hash_username; }

	int get_hash_password() const { return hash_password; }

	void modify_password(const string &_password) { password = _password; }

	void modify_realname(const string &_realname) { realname = _realname; }

	void modify_mailaddr(const string &_mailaddr) { mailaddr = _mailaddr; }

	void modify_privilege(const int &_privilege) { privilege = _privilege; }
};

class user_management
{
private:
	BPTtree<int, user> bpt_users;
	//哈希值对应用户

	linked_hashmap<int, bool> log_in;

public:
	user_management()
	{
		bpt_users("bpt_users");
	}

	bool is_log_in(const int &hashe)
	{
		return log_in.find(hashe) != log_in.end();
	}

	//成功返回1 否则为0
	//每个函数进去之前都要判一下长度是否合法
	std::pair<bool, string> add_user(
		const string &cur_username, const string &username,
		const string &password, const string &realname,
		const string &mailaddr, int privilege)
	{
		if (!bpt_users.size())
			privilege = 10;
		//还没有任何用户注册
		else
		{
			sjtu::my_str<20> cu = cur_username;
			int ch = cu.toint();
			std::pair<bool, user> x = bpt_users.find(ch);
			if (!x.first)
				return std::pair<bool, string>(false, "-1\n");
			//没有原用户
			if (log_in.find(ch) == log_in.end())
				return std::pair<bool, string>(false, "-1\n");
			//原用户未登录
			if (x.second.get_privilege() <= privilege)
				return std::pair<bool, string>(false, "-1\n");
			//权限限制
			sjtu::my_str<20> u = username;
			if (bpt_users.find(u.toint()).first)
				return std::pair<bool, string>(false, "-1\n");
			//新用户已经注册
		}
		user new_user{username, password, realname, mailaddr, privilege};
		bpt_users.insert(std::pair<int, user>(new_user.get_hashe(), new_user));
		return std::pair<bool, string>(true, "0\n");
	}

	std::pair<bool, string> login(const string &username, const string &password)
	{
		sjtu::my_str<20> u = username;
		int uh = u.toint();
		if (is_log_in(uh))
			return std::pair<bool, string>(false, "-1\n");
		//用户已登录
		std::pair<bool, user> x = bpt_users.find(uh);
		if (!x.first)
			return std::pair<bool, string>(false, "-1\n");
		//用户不存在
		sjtu::my_str<30> p = password;
		if (x.second.get_hash_password() != p.toint())
			return std::pair<bool, string>(false, "-1\n");
		//密码错误
		log_in[uh] = true;
		return std::pair<bool, string>(true, "0\n");
	}

	std::pair<bool, string> logout(const string &username)
	{
		sjtu::my_str<20> u = username;
		int uh = u.toint();
		if (!is_log_in(uh))
			return std::pair<bool, string>(false, "-1\n");
		log_in.erase(log_in.find(uh));
		return std::pair<bool, string>(true, "0\n");
	}

	std::pair<bool, string> query_profile(const string &cur_username, const string &username)
	{
		sjtu::my_str<20> cu = cur_username, u = username;
		int ch = cu.toint(), uh = u.toint();
		if (!is_log_in(ch))
			return std::pair<bool, string>(false, "-1\n");
		//原用户未登录
		std::pair<bool, user> xc = bpt_users.find(ch), xu = bpt_users.find(uh);
		if (!xc.first || !xu.first)
			return std::pair<bool, string>(false, "-1\n");
		//原用户或新用户不存在
		if (xc.second.get_privilege() < xu.second.get_privilege())
			return std::pair<bool, string>(false, "-1\n");
		//原用户权限<新用户
		return std::pair<bool, string>(true, username + " " + xu.second.get_realname() +
												 " " + xu.second.get_mailaddr() +
												 " " + to_String(xu.second.get_privilege()) + "\n");
	}

	std::pair<bool, string> modify_profile(
		const string &cur_username, const string &username,
		const string &password, const string &realname,
		const string &mailaddr, const int &privilege)
	{
		sjtu::my_str<20> cu = cur_username, u = username;
		int ch = cu.toint(), uh = u.toint();
		if (!is_log_in(ch))
			return std::pair<bool, string>(false, "-1\n");
		//原用户未登录
		std::pair<bool, user> xc = bpt_users.find(ch), xu = bpt_users.find(uh);
		if (!xc.first || !xu.first)
			return std::pair<bool, string>(false, "-1\n");
		//原用户或新用户不存在
		if (xc.second.get_privilege() < xu.second.get_privilege())
			return std::pair<bool, string>(false, "-1\n");
		//原用户权限<新用户
		if (xc.second.get_privilege() <= privilege)
			return std::pair<bool, string>(false, "-1\n");
		if (password != "")
			xu.second.modify_password(password);
		if (realname != "")
			xu.second.modify_realname(realname);
		if (mailaddr != "")
			xu.second.modify_mailaddr(mailaddr);
		if (privilege >= 0)
			xu.second.modify_privilege(privilege);
		bpt_users.modify(std::pair<int, uset>(uh, xu));
		return std::pair<bool, string>(true, username + " " + xu.second.get_realname() +
												 " " + xu.second.get_mailaddr() +
												 " " + to_String(xu.second.get_privilege()) + "\n");
	}
} userManagement;