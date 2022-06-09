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
	int arrive_time; //从起始站出发到这个站所用的时间
	int depart_time; //从起始站出发到这个站开始出发所用的时间

public:
	station() {}

	station(
		const string &_name, const int &_price,
		const int &_arrive_time,
		const int &_depart_time) : name{_name}, price{_price},
								   arrive_time(_arrive_time), depart_time{_depart_time}
	{
		hashe = name.toint();
	}

	int get_hash() const { return hashe; }

	int get_arrive_time() const { return arrive_time; }

	int get_depart_time() const { return depart_time; }

	int get_price() const { return price; }

	string get_name() const { return name.tostr(); }

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
	int start_time;
	sjtu::my_str<1> type;
	station stations[100];
	bool release;
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
													 start_sale{_start_sale}, end_sale{_end_sale}, type{_type}
	{
		hashe = trainid.toint();
		release = 0;
		start_time = _start_time.toint(),
		int price = 0, arrive_time = 0, depart_time = 0;
		for (int i = 0; i < station_num; i++)
		{
			stations[i](_station_name[i], price, arrive_time, depart_time);
			if (i < station_num - 1)
				price += prices[i], depart_time += travel_time[i];
			arrive_time = depart_time;
			if (i > 0 && i < station_num - 1)
				depart_time += stopover_time[i - 1];
		}
	}

	int get_hash() const { return hashe; }

	int get_station_hash(int i) const { return stations[i].get_hash(); }

	int get_station_num() const { return station_num; }

	int get_price(int i) const { return stations[i].get_price(); }

	void modify_release() { release = 1; }

	bool is_release() const { return release; }

	string get_trainid() const { return trainid.tostr(); }

	string get_type() const { return type.tostr(); }

	string get_station_name(int i) const { return stations[i].get_name(); }

	date get_start_time() const { return start_sale; }

	date get_end_time() const { return end_sale; }

	station get_station(int i) const { return stations[i]; }

	date get_station_depart_time(int i) const
	{
		return start_sale + start_time + stations[i].get_depart_time();
	}

	date get_station_arrive_time(int i) const
	{
		return start_sale + start_time + stations[i].get_arrive_time();
	}

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

class ticket_single
{
#define Ls k << 1
#define Rs k << 1 | 1
#define mid ((l + r) >> 1)
private:
	int ticket_num[400], tag[400], n;

	void build(int k, int l, int r, int v)
	{
		ticket_num[k] = v;
		tag[k] = 0;
		if (l == r)
			return;
		build(Ls, l, mid, v);
		build(Rs, mid + 1, r, v);
	}

	void push(int k, int v)
	{
		tag[k] += v;
		ticket_num[k] += v;
	}

	void down(int k)
	{
		push(Ls, tag[k]);
		push(Rs, tag[k]);
		tag[k] = 0;
	}

	void upd(int k, int l, int r, int L, int R, int v)
	{
		if (L <= l && r <= R)
			return push(k, v);
		if (tag[k])
			down(k);
		if (mid >= L)
			upd(Ls, l, mid, L, R, v);
		if (mid < R)
			upd(Rs, mid + 1, r, L, R, v);
		ticket_num[k] = min(ticket_num[Ls], ticket_num[Rs]);
	}

	int qry(int k, int l, int r, int L, int R)
	{
		if (L <= l && r <= R)
			return ticket_num[k];
		if (tag[k])
			down(k);
		if (mid >= R)
			return qry(Ls, l, mid, L, R);
		if (mid < L)
			return qry(Rs, mid + 1, r, L, R);
		return min(qry(Ls, l, mid, L, mid), qry(Rs, mid + 1, r, mid + 1, R));
	}

	void tranverse(int k, int l, int r, sjtu::vector<int> &u)
	{
		if (l == r)
		{
			u.push_back(ticket_num[k]);
			return;
		}
		if (tag[k])
			down(k);
		tranverse(Ls, l, mid, u);
		tranverse(Rs, mid + 1, r, u);
	}

public:
	ticket_single() {}

	ticket_single(const int &station_num, const int &seat_num)
	{
		n = station_num - 1;
		build(1, 1, n, seat_num);
	}

	sjtu::vector<int> tranverse()
	{
		sjtu::vector<int> u;
		tranverse(1, 1, n, u);
		return u;
	}

	int qry(int l, int r) { return qry(1, 1, n, l, r); }

#undef Ls
#undef Rs
#undef mid
};

class ticket
{
private:
	ticket_single arr[92];

public:
	ticket() {}

	ticket(const int &day, const int &station_num, const int &seat_num)
	{
		for (int i = 0; i < day; i++)
			arr[i](station_num, seat_num);
	}
};

class train_management
{
private:
	BPTtree<int, std::pair<int, int>> bpt_station_train;
	//每个车站的哈希值对应哪些车次id，以及它在这个车次中是第几个站

	BPTtree<int, std::pair<int, int>> bpt_train_information;
	//每个车次的哈希值对应数据库中位置 fisrt 对应火车信息，second对应车票信息

	database<train> train_information;
	//找到对应的火车信息

	database<ticket> train_ticket;
	//找到对应的车票信息

	void sort(
		int l, int r, sjtu::vector<int> &id, const sjtu::vector<string> trainid,
		const sjtu::vector<std::pair<date, date>> &time,
		const sjtu::vector<int> &price, const bool &compare)
	{
		if (l == r)
			return;
		int mid = ((l + r) >> 1);
		sort(l, mid, id, time, price, compare);
		sort(mid + 1, r, id, time, price, compare);
		int *arr = new int[r - l + 1];
		for (int i = l, j = mid + 1, k = l; k <= r; k++)
		{
			if (compare == 0)
			{
				if (j > r || (i <= mid &&
								  time[id[i]].second - time[id[i]].first <
									  time[id[j]].second - time[id[j]].first ||
							  (time[id[i]].second - time[id[i]].first ==
								   time[id[j]].second - time[id[j]].first &&
							   trainid[id[i]] < trainid[id[j]])))
					arr[k - l] = id[i];
				else
					arr[k - l] = id[j];
			}
			else
			{
				if (j > r || (i <= mid && (price[id[i]] < price[id[j]] ||
										   (price[id[i]] == price[id[j]] &&
											trainid[id[i]] < trainid[id[j]]))))
					arr[k - l] = id[i];
				else
					arr[k - l] = id[j];
			}
		}
		for (int i = l; i <= r; i++)
			id[i] = arr[i - l];
		delete[] arr;
	}

public:
	train_management()
	{
		bpt_station_train("bpt_station_train");
		bpt_train_information("bpt_train_information");
		train_information("train_information");
		train_ticket("train_ticket");
	}

	std::pair<bool, std::pair<int, int>> find_train_information(const int &th)
	{
		return bpt_train_information.find(th);
	}

	train find_train(const int &tr_addr)
	{
		return train_information.find(tr_addr);
	}

	ticket_single find_ticket(const int &tk_addr, const int &day)
	{
		return train_ticket.find(tk_addr, day);
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
		ticket v(end_sale & start_sale, station_num, seat_num);
		int tk = train_ticket.insert(v);
		bpt_train_information.insert(std::pair<int, std::pair<int, int>>(u.get_hash(),
																		 std::pair<int, int>(tr, tk)));
		for (int i = 0; i < station_num; i++)
			bpt_station_train.insert(std::pair<int, std::pair<int, int>>(u.get_station_hash(i),
																		 std::pair<int, int>(u.get_hash(), i)));
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
		if (u.is_release())
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
		if (u.is_release())
			return std::pair<bool, string>(false, "-1\n");
		//车次已发布
		u.modify_release();
		train_ticket.modify(x.second.second, u);
		return std::pair<bool, string>(true, "0\n");
	}

	//发布前的车次，不可发售车票，无法被query_ticket和query_transfer操作所查询到
	std::pair<bool, string> query_train(
		const string &trainid, const date &depart)
	{
		sjtu::my_str<20> ti = trainid;
		int th = ti.toint();
		std::pair<bool, std::pair<int, int>> x = bpt_train_information.find(th);
		if (!x.first)
			return std::pair<bool, string>(false, "-1\n");
		// trainid 不存在
		train u = train_information.find(x.second.first).second;
		if (!u.is_release())
			return std::pair<bool, string>(false, "-1\n");
		//
		int day = (depart & u.get_start_time());
		if (day <= 0 ||)
			return std::pair<bool, string>(false, "-1\n");
		//没有从这一天出发
		ticket_single v = train_ticket.find(x.second.second, (depart & u.get_start_time()) - 1).second;
		sjtu::vector<int> seat_num = v.tranverse();
		string ans = u.get_trainid() + u.get_type() + "\n";
		for (int i = 0; i < station_num; i++)
		{
			ans += u.get_station_name(i) + " ";
			if (i == 0)
				ans += "xx-xx xx:xx -> ";
			else
				ans += (u.get_station_depart_time(i) ^ (depart & u.get_start_sale)).tostr() + " -> ";
			time += u.get_stop_time(i);
			if (i == station_num - 1)
				ans += "xx-xx xx:xx ";
			else
				ans += (u.get_station_arrive_time(i) ^ (depart & u.get_start_sale)).tostr() + " ";
			ans += to_String(u.get_price(i)) + " ";
			if (i == station_num - 1)
				ans += "x\n";
			else
				ans += to_String(seat_num[i]) + "\n";
		}
		//格式：中院 07-02 05:19 -> 07-02 05:24 114 1000
		return std::pair<bool, string>(true, ans);
	}

	string query_ticket(
		const string &start_station, const string &end_station,
		const date &depart, const bool &compare) // compare=0按照time排序，compare=1按照cost排序
	{
		my_str<30> sn = start_staion, en = end_station;
		int sh = sn.toint(), eh = en.toint();
		sjtu::vector<std::pair<int, int>> st = bpt_station_train.Find(sh);
		sjtu::vector<std::pair<int, int>> et = bpt_station_train.Find(eh);
		sjtu::vector<string> trainid;
		sjtu::vector<std::pair<date, date>> time;
		sjtu::vector<int> price, seat_num, id;
		int sz = st.size(), ez = et.size(), z = 0;
		for (int si = 0, ei = 0; si < sz && ei < ez;)
		{
			if (st[si].first < et[ei].first)
				si++;
			else if (st[si].first > et[ei].first)
				ei++;
			else
			{
				if (st[si].second < et[ei].second)
				{
					std::pair<int, int> x = bpt_train_information.find(st[si].first).second;
					train u = train_information.find(x.first);
					int day = (depart & u.get_station_depart_time(st[si].second));
					if (u.is_release() && day > 0 && day <= (u.get_end_time() & u.get_start_time()))
					{
						seat_num.push_back(
							train_ticket.find(x.second, day - 1).qry(st[si].second, et[ei].second - 1));
						id.push_back(z++);
						price.push_back(u.get_price(et[ei].second) - u.get_price(st[si].second));
						time.push_back(std::pair<date, date>(
							u.get_station_depart_time(st[si].second) ^ day,
							u.get_station_arrive_time(et[ei].second) ^ day));
						trainid.push_back(u.get_trainid());
					}
				}
				si++, ei++;
			}
		}
		sort(0, z - 1, id, trainid, time, price, compare);
		string ans = to_String(z) + "\n";
		for (int i = 0, u; i < z; i++)
		{
			u = id[i];
			ans += trainid[u] + " " + start_station + " " + time[u].first.tostr() + " -> " +
				   end_station + " " + time[u].second.tostr() + " " +
				   to_String(price[u]) + " " + to_String(seat_num[u]) + "\n";
		}
		return ans;
		//格式：HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
	}

	string query_transfer(
		const string &start_station, const string &end_station,
		const date &depart, const bool &compare)
	{
		my_str<30> sn = start_staion, en = end_station;
		int sh = sn.toint(), eh = en.toint();
		sjtu::vector<std::pair<int, int>> st = bpt_station_train.Find(sh);
		sjtu::vector<std::pair<int, int>> et = bpt_station_train.Find(eh);
		string trainid[2], _start_station[2], _end_station[2];
		_start_station[0] = start_station, _end_station[1] = end_station;
		std::pair<date, date> time[2];
		int price[2], seat_num[2];
		int sz = st.size(), ez = et.size();
		bool fl = 0;
		sjtu::vector<train> etr;
		sjtu::vector<std::pair<int, int>> eti;
		for (int ei = 0; ei < ez; ei++)
		{
			std::pair<int, int> y = bpt_train_information.find(et[ei].first).second;
			train v = train_information.find(y.first);
			if (v.is_release())
				eti.push_back(y), etr.push_back(v);
		}
		ez = eti.size();
		for (int si = 0; si < sz; si++)
		{
			std::pair<int, int> x = bpt_train_information.find(st[si].first).second;
			train u = train_information.find(x.first);
			if (!u.is_release())
				continue;
			linked_hashmap<int, std::pair<int, int>> mp;
			int day = (depart & u.get_station_depart_time(st[si].second));
			if (day <= 0 || day > (u.get_end_time() & u.get_start_time()))
				continue;
			for (int i = st[si].second + 1; i < u.get_station_num(); i++)
			{
				int _seat_num = train_ticket.find(x.second, day - 1).qry(st[si].second, i - 1);
				mp[u.get_station_hash(i)] = std::pair<int, int>(i, _seat_num);
			}
			for (int ei = 0; ei < ez; ei++)
			{
				if (st[si].first == et[ei].first)
					continue;
				std::pair<int, int> y = eti[ei];
				train v = etr[ei];
				for (int i = 0; i < et[ei].second; i++)
				{
					if (mp.find(v.get_station_hash(i)) == mp.end())
						continue;
					std::pair<int, int> z = mp[v.get_station_hash(i)];
					date arrive_time = u.get_station_arrive_time(z.first);
					date last_depart_time = v.get_station_depart_time(i) ^
											(v.get_end_sale_time() & v.get_start_sale_time());
					if (arrive_time <= last_depart_time)
					{
						date depart_time = last_depart_time ^ (arrive_time & last_depart_time);
						if (depart_time < arrive_time)
							depart_time = depart_time ^ 1;
						if (depart_time < v.get_station_depart_time(i))
							depart_time = v.get_station_depart_time(i);
						int _day = (depart_time & v.get_station_depart_time(i));
						int _seat_num = train_ticket.find(y.second, _day - 1).qry(i, et[et].second - 1);
						int price1 = u.get_price(z.first) - u.get_price(st[si].second);
						int price2 = v.get_price(et[ei].second) - v.get_price(i);
						int time1 = u.get_station_arrive_time(z.first) -
									u.get_station_depart_time(st[si].second);
						int time2 = v.get_station_arrive_time(et[ei].second) -
									v.get_station_depart_time(i);
						if (!compare)
						{
							if ((time[1].second - time[1].first) + (time[0].second - time[0].first) <
								tim1 + time2)
								continue;
							if (price[0] + price[1] < price1 + price2)
								continue;
						}
						else
						{
							if (price[0] + price[1] < price1 + price2)
								continue;
							if ((time[1].second - time[1].first) + (time[0].second - time[0].first) <
								tim1 + time2)
								continue;
						}
						if (train[0] < u.get_trainid())
							continue;
						if (train[1] < v.get_trainid())
							continue;
						price[0] = price1, price[1] = price2;
						seat_num[0] = z.second, seat_num[1] = _seat_num;
						time[0] = std::pair<date, date>(u.get_station_depart_time(st[si].second) ^ day,
														u.get_station_arrive_time(z.first) ^ day);
						time[1] = std::pair<date, date>(v.get_station_depart_time(i) ^ _day,
														v.get_station_arrive_time(et[ei].second) ^ _day);
						trainid[0] = u.get_trainid, trainid[1] = v.get_trainid;
						_start_station[1] = _end_station[0] = v.get_station_name(i);
					}
				}
			}
		}
		string ans = "";
		if (!fl)
			ans = "0\n";
		else
			for (int u = 0; u < 2; u++)
				ans += trainid[u] + " " + start_station[u] + " " + time[u].first.tostr() + " -> " +
					   end_station[u] + " " + time[u].second.tostr() + " " +
					   to_String(price[u]) + " " + to_String(seat_num[u]) + "\n";
		return ans;
		//格式：HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000
	}
} trainManagement;

#endif
