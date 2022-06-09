#ifndef STRING_HPP
#define STRING_HPP

#include <iostream>
using std::string;

namespace sjtu
{

	template <int len = 10>
	class my_str
	{
	private:
		char s[len];
		const int prime = 793999;
		const int mod = 1e9 + 7;

	public:
		my_str()
		{
			for (int i = 0; i < len; i++)
				s[i] = 0;
		}

		my_str(const string &str)
		{
			int u = str.length();
			for (int i = 0; i < u; i++)
				s[i] = str[i];
			for (int i = u; i < len; i++)
				s[i] = 0;
		}
		//进来的时候保证str的长度<=len

		string tostr() const
		{
			string u = "";
			for (int i = 0; i < len && s[i]; i++)
				u += s[i];
			return u;
		}

		int toint() const
		{
			int u = 0;
			for (int i = len - 1; i >= 0; i--)
				u = (1ll * prime * u + s[i]) % mod;
			return u;
		}
	};

}

#endif
