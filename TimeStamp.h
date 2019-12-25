//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_TIMESTAMP_H
#define HORIZON_TIMESTAMP_H

#include <ctime>

namespace Horizon
{
    constexpr int kSecondflag = 0;
    constexpr int kMillisecondflag=1;
    constexpr int kMicrosecondflag = 2;
    class TimeStamp
    {
    public:
        TimeStamp();
        //time is Millisecond
        explicit TimeStamp(long time);
        explicit TimeStamp(struct timeval time);

        TimeStamp now();
        void setTime(int,int);
        void Clear();
        void updateTime();
        void Add_delay(const int delay,int timeflag);
        //void Add_delay(const int delay,int timeflag)const;
        __time_t GetSecond()const { return _time.tv_sec;}
        __suseconds_t GetMicroSecond()const{ return _time.tv_usec;}
        __useconds_t Gettime()const{return _time.tv_sec*1000000+_time.tv_usec;}
    private:
        struct timeval _time;
    };

}
#endif //HORIZON_TIMESTAMP_H
