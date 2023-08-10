#ifndef DATE_H
#define DATE_H
#include <iostream>
class date{
    private: 
        int day;
        int month;
        int year;
    public:
    
        date(int day, int month, int year);
        date(const date *obj);
        int getDay();
        int getMonth();
        int getYear();
        int compare(date*& date1, date*& date2);
        void printDate();
        int checkDate();
        int empty();
        int lessEqThan(const date& obj);
        int greaterEqThan(const date& obj);
        bool operator <=(const date& obj);
        bool operator >=(const date& obj);
};
#endif
