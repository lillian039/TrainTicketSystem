#ifndef DATE_HPP
#define DATE_HPP

class date
{
private:
    int month;
    int day;
    int hour;
    int minute;
    date work(date u){
        if (u.minute<)
    }

    int to_day(date u){
        
    }
public:
    date() {}

    date(const int& _month,const int& _day,const int& _hour,const int& _minute){
        month=_month,day=_day,hour=_hour,minute=_minute;
    }

    friend date operator + (const date &u, const int &v)
    {
        date w=u;w.minute+=v;return work(w);
    } //分钟相加

    friend date operator ^ (const date &u, const int &v)
    {
        date w=u;w.day+=v;return work(w);
    } //日期相加

    friend int operator - (const date &u, const date &v)
    {
        return u.to_min()-v.to_min();
    } //分钟之差

    friend int operator & (const date &u, const date &v)
    {
        return u.to_day()-v.to_day();
    } //日期之差，直接减 不是一共几天是差几天
};

#endif