//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_TIMERTASK_H
#define HORIZON_TIMERTASK_H

#include "Eventbase.h"
#include "TimeStamp.h"
namespace Horizon
{
    class EventLoop;
    class TimerTask
    {
    public:
        TimerTask()= default;
		explicit TimerTask(TimeStamp);

        TimerTask(const TimerTask&)=delete;
        TimerTask(const TimerTask&&)=delete;
        TimerTask& operator=(const TimerTask&)=delete;
        virtual ~TimerTask()= default;

        /*need override*/
        virtual void handleTask()=0;

        void Set_timeout(const int& delay,int flag);
        void Set_timeout(TimeStamp timeout){_timeout=timeout;}
        TimeStamp GetTimeout(){ return  _timeout;}
    protected:
        TimeStamp _timeout;
    };
}

#endif //HORIZON_TIMERTASK_H
