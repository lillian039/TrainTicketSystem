#ifndef USER_HPP
#define USER_HPP

#include "string.hpp"
#include "map.hpp"
#include "bptree.hpp"
#include "error.hpp"

class user{
private:
	string<20> username;
	string<30> password;
	string<15> realname;
	string<30> mailaddr;
	int privilege=0;
	int hashe;
public:
	user(){}

	user(const string<20>& _username,const string<30>& _password,
		const string<15>& _realname,const string<30>& _mailaddr,
		const int& _privilege){}

	~user(){}

	friend bool operator < (const user& lhs,const user& rhs){
		return lhs.hashe<rhs.hashe;
	}

	friend bool operator == (const user& lhs,const user& rhs){
		return lhs.hashe==rhs.hashe;
	}

	friend bool operator != (const user& lhs,const user& rhs){
		return lhs.hashe!=rhs.hashe;
	}

	string<30> get_password() const {}

	string<15> get_realname() const {}

	string<30> get_mailaddr() const {}

	int get_pribilege() const {}

	void modify_password(const string<30>& _password){}

	void modify_realname(const string<15>& _realname){}

	void modify_mailaddr(const string<30>& _mailaddr){}

	void modify_privilege(const int& _privilege){}
};

BPTtree<int,user> bpt_station_train;
//哈希值对应用户

map<int,user> log_in;

void add_user(){}

void login(){}

void logout(){}

int query_profile(){}

void modify_profile(){}