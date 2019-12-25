//
// Created by zhangjianxiong on 2019/12/14.
//
#include <sys/time.h>
#include <cstring>
#include "TimeStamp.h"
#include "Log.h"
namespace Horizon
{
    TimeStamp::TimeStamp():_time{0,0}
    {
    }
    TimeStamp::TimeStamp(long time)
    {
        _time.tv_sec = time / 1000;
        _time.tv_usec = (time%1000)*1000;
    }
    TimeStamp::TimeStamp(struct timeval time):_time(time)
    {}
    TimeStamp TimeStamp::now()
    {
        struct timeval time{0,0};
        int ret = ::gettimeofday(&time,NULL);
        if(ret<0)
        {
            LOG_ERR << "gettime error";
        }
        return TimeStamp(time);
    }
    void TimeStamp::Clear()
    {
        _time.tv_sec=0;
        _time.tv_usec=0;
    }

    void TimeStamp::updateTime()
    {
        struct timeval time{0,0};
        int ret=::gettimeofday(&time,NULL);
        if(ret<0)
        {
            LOG_ERR << "gettime error";
        }
        _time=time;
    }
    void TimeStamp::Add_delay(const int delay,int timeflag)
    {
        updateTime();
        switch(timeflag)
        {
            case kSecondflag:
                _time.tv_sec+=delay;
                break;
            case kMillisecondflag:
                _time.tv_sec+=delay/1000;
                _time.tv_usec+=(delay%1000)*1000;
                break;
            case kMicrosecondflag:
                _time.tv_usec+=delay;
                break;
            default:
                break;
        }
    }

    void TimeStamp::setTime(const int time,const int flag)
    {
        switch (flag)
        {
            case kSecondflag:
                _time.tv_sec=time;
                break;
            case kMillisecondflag:
                _time.tv_sec=time/1000;
                _time.tv_usec=time*1000-(time/1000)*1000000;
                break;
            case kMicrosecondflag:
                _time.tv_usec=time;
                break;
            default:
                break;
        }
    }
}