#ifndef ORDER_HPP
#define ORDER_HPP

#include "string.hpp"
#include "date.hpp"
#include "error.hpp"
#include "user.hpp"
#include "train.hpp"
#include "bptree.hpp"

class order{
	string<20> username;
	string<20> trainid;
	string<15> start_station;
	string<15> end_station;
	int price;
	int status;
	//0 succeeded; 1 pending; 2 refunded
	int ticket_num;
	date time;
};

BPTtree<int,order> btp_user_order;
//用户id的哈希值查询订单

BPTtree<int,order> btp_train_order;
//用户id的哈希值查询车次的候补队列

void buy_ticket(){}

void query_order(){}

void refund_ticket(){}

#endif
