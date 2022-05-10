#ifndef STRING_HPP
#define STRING_HPP

template<int len=10>
class string{
private:
	char str[len];
	const int prime=793999;
	const int mod=1e9+7;
public:
	string(){}
	
	string(const std::string& str){}

	operator int(){}
	//转化为哈希值
};

#endif
