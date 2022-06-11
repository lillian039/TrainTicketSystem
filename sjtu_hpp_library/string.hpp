#ifndef STRING_HPP
#define STRING_HPP

#include <iostream>

using std::string;

namespace sjtu {

    template<int len = 10>
    class my_str {
    private:
        char s[len];
        int n;
        const int prime = 19260817;
        const int mod = 1e9 + 7;

    public:
        my_str() {
            for (int i = 0; i < len; i++)
                s[i] = 0;
            n = len;
        }

        my_str(const string &str) {
            n = str.length();
            for (int i = 0; i < n; i++)
                s[i] = str[i];
        }
        //进来的时候保证str的长度<=len

        string tostr() const {
            string u = "";
            for (int i = 0; i < n; i++)
                u += s[i];
            return u;
        }

        my_str &operator=(const my_str &A) {
            if (this == &A)
                return *this;
            n = A.n;
            for (int i = 0; i < n; i++) s[i] = A.s[i];
            return *this;
        }

        int toint() const {
            int u = 0;
            for (int i = 0; i < n; i++)
                u = (1ll * prime * u + s[i]) % mod;
            return (u + mod) % mod;
        }
    };

}

#endif
