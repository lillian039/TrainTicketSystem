#ifndef DATE_HPP
#define DATE_HPP

class date{
private:
    int month;
    int day;
    int hour;
    int minute;
public:
    date(){}

    date(const std::string& _date){}

    date(const date& other){}

    operator std::string(){}
    //转化为string类，方便输出

    operator int(){}
    //转化为int类，方便比较

    ~date(){}
};

#endif