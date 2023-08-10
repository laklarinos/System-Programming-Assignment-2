#ifndef DATE_C
#define DATE_C
#include <iostream>
#include "date.h"
using namespace std;
    date::date(int day, int month, int year){
        this->day=day;
        this->month=month;
        this->year=year;
    }

    date::date(const date *obj){
        this->day = obj->day;
        this->month = obj->month;
        this->year = obj->year;
    }

    int date::empty(){
        if(this->day == 0 && this->month == 0 && this->year == 0000) return 1;
        return 0;
    }

    bool date:: operator <= (const date& obj){
        if(this->year < obj.year) return 1;
        if(this->year > obj.year) return 0;
        if(this->month < obj.month) return 1;
        if(this->month > obj.month) return 0;
        if(this->day < obj.day) return 1;
        if(this->day > obj.day) return 0;
        return 1;
    }

    int date:: lessEqThan(const date& obj){
        if(this->year < obj.year) return 1;
        if(this->year > obj.year) return 0;
        if(this->month < obj.month) return 1;
        if(this->month > obj.month) return 0;
        if(this->day < obj.day) return 1;
        if(this->day > obj.day) return 0;
        return 1;
    }

    int date:: greaterEqThan(const date& obj){
        if(this->year > obj.year) return 1;
        if(this->year < obj.year) return 0;
        if(this->month > obj.month) return 1;
        if(this->month < obj.month) return 0;
        if(this->day > obj.day) return 1;
        if(this->day < obj.day) return 0;
        return 1;
    }

    bool date:: operator >= (const date& obj){
        
        if(this->year > obj.year) return 1;
        if(this->year < obj.year) return 0;
        if(this->month > obj.month) return 1;
        if(this->month < obj.month) return 0;
        if(this->day > obj.day) return 1;
        if(this->day < obj.day) return 0;
        return 1;
    }

    void date::printDate(){
        cout<<day<<"-"<<month<<"-"<<year<<endl;
    }

    int date::checkDate(){
        if(this->day < 1 || this->day > 31) return 1;
        if(this->month < 1 || this->month > 12) return 1;
        if(this->year < 1900 || this->year > 2021) return 1;
        return 0;
    }

    int date:: getDay(){
        return this->day;
    }
    int date:: getMonth(){
        return this->month;
    }
    int date:: getYear(){
        return this->year;
    }
#endif