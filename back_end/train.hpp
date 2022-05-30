#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "string.hpp"
#include "date.hpp"
#include "map.hpp"
#include "bptree.hpp"
#include "error.hpp"

struct station{
	string<30> name;
	int hashe;
	int seatnum;
	int price;
	int travel_time;
	int stop_time;
	
	friend bool operator < (const station& lhs,const station& rhs){
		return lhs.hashe<rhs.hashe;
	}
	
	friend bool operator > (const station& lhs,const station& rhs){
		return lhs.hashe>rhs.hashe;
	}

	friend bool operator == (const station& lhs,const station& rhs){
		return lhs.hashe==rhs.hashe;
	}

	friend bool operator != (const station& lhs,const station& rhs){
		return lhs.hashe!=rhs.hashe;
	}
};

class train{
private:
	string<20> trainid;
	int station_num;
	date start_sale;
	date end_sale;
	date start_time;
	string<1> type;
	station stations[100];
	bool is_release=0;
	int hashe;
public:
	train(){}
	
	~train(){}

	friend bool operator < (const train& lhs,const train& rhs){
		return lhs.hashe<rhs.hashe;
	}

	friend bool operator == (const train& lhs,const train& rhs){
		return lhs.hashe==rhs.hashe;
	}

	friend bool operator != (const train& lhs,const train& rhs){
		return lhs.hashe!=rhs.hashe;
	}
};

class ticket{
	int ticket_num[402],tag[402];

	void build(){}
	//线段树建树

	void push(int k,int v){}
	//区间维护，打标记
	
	void down(int k){}
	//下放标记
	
	void update(int k,int l,int r,int L,int R,int v){}
	//区间增加多少
	
	int query_min(int k,int l,int r,int L,int R){}
	//区间的最小值

};

BPTtree<int,int> bpt_station_train;
//每个车站的哈希值对应哪些车次id位置

BPTtree<int,std::pair<int,int>> bpt_train_information;
//每个车次的哈希值对应数据库中位置 fisrt 对应火车信息，second对应车票信息

database<train> train_information;
//找到对应的火车信息

database<ticket> train_ticket;
//找到对应的车票信息

void add_train(){}

void release_train(){}

void query_train(){}

void query_ticket(){}

void query_transfer(){}

#endif
