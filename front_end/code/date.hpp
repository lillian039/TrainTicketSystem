#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>

struct date {
    int month;
    int day;
    int hour;
    int minute;

    date() : month{6}, day{1}, hour{0}, minute{0} {}

    date(const int &_month, const int &_day, const int &_hour, const int &_minute) {
        month = _month, day = _day, hour = _hour, minute = _minute;
    }

    int to_day() const {
        int x = 0;
        if (month == 6) x = day;
        else if (month == 7) x = day + 30;
        else if (month == 8) x = day + 61;
        else if (month == 9) x = day + 92;
        return x;
    }

    int to_min() const {
        int x = 0;
        if (month == 6) x = day;
        else if (month == 7) x = day + 30;
        else if (month == 8) x = day + 61;
        else if (month == 9) x = day + 92;
        return (x - 1) * 1440 + hour * 60 + minute;
    }

    string tostr() const {
        // xx-xx xx:xx
        string x = "0" + std::to_string(month) + "-";
        if (day < 10) x += "0";
        x += std::to_string(day) + " ";
        if (hour < 10) x += "0";
//        std::cerr << hour << std::endl;
        x += std::to_string(hour) + ":";
        if (minute < 10) x += "0";
        x += std::to_string(minute);
        return x;
    }
};

date work(date u) {
    if (u.minute < 0) {
        int dhr = (u.minute - 59) / 60;
        u.hour += dhr, u.minute -= dhr * 60;
    }

    if (u.minute >= 60) {
        int dhr = u.minute / 60;
        u.hour += dhr, u.minute -= dhr * 60;
    }

    if (u.hour < 0) {
        int ddy = (u.hour - 23) / 24;
        u.day += ddy, u.hour -= ddy * 24;
    }

    if (u.hour >= 24) {
        int ddy = u.hour / 24;
        u.day += ddy, u.hour -= ddy * 24;
    }

    while (u.day <= 0) {
        u.month--;
        u.day += 30 + (u.month > 6);
    }

    while (u.day > 31) {
        u.day -= 31 - (u.month == 6);
        u.month++;
    }

    if (u.month == 6 && u.day == 31)
        u.month++, u.day = 1;

    return u;
}

date operator+(const date &u, const int &v) {
    date w = u;
    w.minute += v;
    return work(w);
} //分钟相加

date operator^(const date &u, const int &v) {
    date w = u;
    w.day += v;
    return work(w);
} //日期相加

int operator-(const date &u, const date &v) {
    return u.to_min() - v.to_min();
} //分钟之差

int operator&(const date &u, const date &v) {
    return u.to_day() - v.to_day();
} //日期之差，直接减 不是一共几天是差几天

#endif