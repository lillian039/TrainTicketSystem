#ifndef TRAIN_HPP
#define TRAIN_HPP

#include <iostream>

#include "string.hpp"
#include "date.hpp"
#include "vector.hpp"
#include "bptree.hpp"
#include "error.hpp"

using std::min;
using std::string;

class station
{
private:
	sjtu::my_str<30> name;
	int hashe;
	int price;
	int travel_time;
	int stop_time;

public:
	station() {}

	station(
		const string &_name, const int &_price, const int &_travel_time,
		const int &_stop_time) : name{_name}, price{_price},
								 travel_time(_travel_time), stop_time{_stop_time}
	{
		hashe = name.toint();
	}

	int get_hash() const { return hashe; }

	friend bool operator<(const station &lhs, const station &rhs)
	{
		return lhs.hashe < rhs.hashe;
	}

	friend bool operator>(const station &lhs, const station &rhs)
	{
		return lhs.hashe > rhs.hashe;
	}

	friend bool operator==(const station &lhs, const station &rhs)
	{
		return lhs.hashe == rhs.hashe;
	}

	friend bool operator!=(const station &lhs, const station &rhs)
	{
		return lhs.hashe != rhs.hashe;
	}
};

class train
{
private:
	sjtu::my_str<20> trainid;
	int station_num;
	date start_sale;
	date end_sale;
	date start_time;
	sjtu::my_str<1> type;
	station stations[100];
	bool is_release;
	int hashe;

public:
	train() {}

	train(
		const string &_trainid, const int &_station_num,
		const sjtu::vector<string> &_station_name,
		const sjtu::vector<int> &prices, const date &_start_time,
		const sjtu::vector<int> &travel_time,
		const sjtu::vector<int> &stopover_time, const date &_start_sale,
		const date &_end_sale, const string _type) : trainid{_trainid}, station_num{_station_num},
													 start_time{_start_time}, start_sale{_start_sale},
													 end_sale{_end_sale}, type{_type}
	{
		hashe = trainid.toint();
		is_release = 0;
		for (int i = 0; i < station_num; i++)
		{
			if (i == 0)
				stations[i] = station(_station_name[i], prices[i], travel_time[i], 0);
			else if (i == station_num - 1)
				stations[i] = station(_station_name[i], 0, 0, 0);
			else
				stations[i] = station(_station_name[i], prices[i], travel_time[i], stopover_time[i - 1]);
		}
	}

	int get_hash() const { return hashe; }

	int get_station_hash(int i) const { return stations[i].get_hash(); }

	void modify_is_release() { is_release = 1; }

	friend bool operator<(const train &lhs, const train &rhs)
	{
		return lhs.hashe < rhs.hashe;
	}

	friend bool operator>(const train &lhs, const train &rhs)
	{
		return lhs.hashe > rhs.hashe;
	}

	friend bool operator==(const train &lhs, const train &rhs)
	{
		return lhs.hashe == rhs.hashe;
	}

	friend bool operator!=(const train &lhs, const train &rhs)
	{
		return lhs.hashe != rhs.hashe;
	}
};

class ticket
{
#define Ls k << 1
#define Rs k << 1 | 1
#define mid ((l + r) >> 1)
private:
	int ticket_num[92][400], tag[92][400], totday;

	void build(int i, int k, int l, int r, int v)
	{
		ticket_num[i][k] = v;
		tag[i][k] = 0;
		if (l == r)
			return;
		build(i, Ls, l, mid, v);
		build(i, Rs, mid + 1, r, v);
	}

	void push(int i, int k, int v)
	{
		tag[i][k] += v;
		ticket_num[i][k] += v;
	}

	void down(int i, int k)
	{
		push(i, Ls, tag[i][k]);
		push(i, Rs, tag[i][k]);
		tag[i][k] = 0;
	}

	void upd(int i, int k, int l, int r, int L, int R, int v)
	{
		if (L <= l && r <= R)
			return push(i, k, v);
		if (tg[i][k])
			down(i, k);
		if (mid >= L)
			upd(i, Ls, l, mid, L, R, v);
		if (mid < R)
			upd(i, Rs, mid + 1, r, L, R, v);
		ticket_num[i][k] = min(ticket_num[i][Ls], ticket_num[i][Rs]);
	}

	int qry(int i, int k, int l, int r, int L, int R)
	{
		if (L <= l && r <= R)
			return ticket_num[i][k];
		if (tg[i][k])
			down(i, k);
		if (mid >= R)
			return qry(i, Ls, l, mid, L, R);
		if (mid < L)
			return qry(i, Rs, mid + 1, r, L, R);
		return min(qry(i, Ls, l, mid, L, mid), qry(i, Rs, mid + 1, r, mid + 1, R));
	}

public:
	ticket() {}

	ticket(int tot_day, int station_num, int seat_num)
	{
		totday = tot_day;
		for (int i = 0; i < totday; i++)
			build(i, 1, 1, station_num - 1, seat_num);
	}

#undef Ls
#undef Rs
#undef mid
};

class train_management
{
private:
	BPTtree<int, int> bpt_station_train;
	//每个车站的哈希值对应哪些车次id位置

	BPTtree<int, std::pair<int, int>> bpt_train_information;
	//每个车次的哈希值对应数据库中位置 fisrt 对应火车信息，second对应车票信息

	database<train> train_information;
	//找到对应的火车信息

	database<ticket> train_ticket;
	//找到对应的车票信息
public:
	train_management()
	{
		bpt_station_train("bpt_station_train");
		bpt_train_information("bpt_train_information");
		train_information("train_information");
		train_ticket("train_ticket");
	}
	std::pair<bool, string> add_train(
		const string &trainid, const int &station_num,
		const int &seat_num, const sjtu::vector<string> &station_name,
		const sjtu::vector<int> &prices, const date &start_time,
		const sjtu::vector<int> &travel_time, const sjtu::vector<int> &stopover_time,
		const date &start_sale, const date &end_sale, const string &type)
	{
		sjtu::my_str<20> ti = trainid;
		int th = ti.toint();
		std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
		if (x.first)
			return std::pair<bool, string>(false, "-1\n");
		// trainid已经存在了
		train u = (trainid, station_num, seat_num, station_name, prices, start_time,
				   travel_time, stopover_time, start_sale, end_sale, type);
		int tr = train_information.insert(u);
		ticket v(end_sale - start_sale + 1, station_num, seat_num);
		int tk = train_ticket.insert(v);
		bpt_train_information.insert(std::pair<int, std::pair<int, int>>(u.get_hash(),
																		 std::pair<int, int>(tr, tk)));
		for (int i = 0; i < station_num; i++)
			bpt_station_train.insert(std::pair<int, int>(u.get_station_hash(i), u.get_hash()));
		return std::pair<bool, string>(true, "0\n");
	}

	std::pair<bool, string> delete_train(const string &trainid)
	{
		sjtu::my_str<20> ti = trainid;
		int th = ti.toint();
		std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
		if (!x.first)
			return std::pair<bool, string>(false, "-1\n");
		// trainid不存在
		train u = train_information.find(x.second.first);
		if (u.release)
			return std::pair<bool, string>(false, "-1\n");
		//车次已发布
		train_information.remove(x.second.first);
		train_ticket.remove(x.second.second);
		bpt_train_information.remove(std::pair<int, std::pair<int, int>>(th, x));
		int station_num = u.get_station_num();
		for (int i = 0; i < station_num; i++)
			bpt_station_train.remove(std::pair<int, int>(u.get_station_hash(i), u.get_hash()));
		return std::pair<bool, string>(true, "0\n");
	}

	std::pair<bool, string> release_train(const string &trainid)
	{
		sjtu::my_str<20> ti = trainid;
		int th = ti.toint();
		std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
		if (!x.first)
			return std::pair<bool, string>(false, "-1\n");
		// trainid 不存在
		train u = train_information.find(x.second.first);
		if (u.release)
			return std::pair<bool, string>(false, "-1\n");
		//车次已发布
		u.modify_is_release();
		train_ticket.modify(x.second.second, u);
		return std::pair<bool, string>(true, "0\n");
	}
	//发布前的车次，不可发售车票，无法被query_ticket和query_transfer操作所查询到

	std::pair<bool, string> query_train(
		const string &trainid, const date &depart)
	{

	}

	void query_ticket() {}

	void query_transfer() {}
};

#endif
