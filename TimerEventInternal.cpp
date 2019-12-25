//
// Created by zhangjianxiong on 2019/12/14.
//
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include <functional>

#include "TimerEventInternal.h"
#include "EventLoop.h"
#include "Log.h"
#include "TimerTask.h"
#include "Socket.h"
namespace Horizon
{
    int create_timerfd()
    {
        int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
        if(timerfd < 0)
        {
			LOG_WARN << errno;
			timerfd=-1;
        }
        return timerfd;
    }
    int set_timerfd(const int& timerfd,TimeStamp timeout)
    {
        if(timerfd > 0) {
            struct itimerspec itime;
            itime.it_interval.tv_sec = 0;
            itime.it_interval.tv_nsec = 0;

            if (timeout.Gettime() ==  0) {
                itime.it_value.tv_sec = 0;
                itime.it_value.tv_nsec = 0;
            } else {
                itime.it_value.tv_sec = timeout.GetSecond();
                itime.it_value.tv_nsec = timeout.GetMicroSecond()*1000;
            }
            if (::timerfd_settime(timerfd, 0, &itime, NULL) < 0) {
                LOG_WARN << "timerfd_settime error";
                return -1;
            }
        }
        return timerfd;
    }

    TimerEventInternal::TimerEventInternal(EventLoop* loop)
    :Eventbase(loop,Socket(create_timerfd(),0))
    {}

    void TimerEventInternal::readTimerfd(const int& timerfd)
    {
        uint64_t howmany;
        ssize_t n=::read(timerfd,&howmany, sizeof(howmany));
        if(n != sizeof(howmany))
        {
            LOG_ERR << "read "<< n <<"byte from Timerfd";
        }
    }
    void TimerEventInternal::addTask(std::shared_ptr<TimerTask> task)
    {
        _loop->RuninLoop(std::bind(&TimerEventInternal::addTaskInLoop, this, task));
    }

    void TimerEventInternal::addTaskInLoop(std::shared_ptr<TimerTask> task)
    {
        assert(_loop->isThisThread());
        auto iter=_timerSet.begin();
        if(iter == _timerSet.end() || task->GetTimeout().Gettime() < (*iter).first.Gettime())
        {
            set_timerfd(this->fd(),task->GetTimeout());
        }
        auto result=_timerSet.insert(std::pair<TimeStamp,std::shared_ptr<TimerTask>>(task->GetTimeout(), task));
        assert(result.second);
    }
    void TimerEventInternal::removeTask(const std::shared_ptr<TimerTask>& task)
    {
        _loop->RuninLoop(std::bind(&TimerEventInternal::removeTaskInLoop,this,task));
    }
    void TimerEventInternal::removeTaskInLoop(const std::shared_ptr<TimerTask>& task)
    {
        assert(_loop->isThisThread());
        TimeStamp time = task->GetTimeout();
        auto result = _timerSet.erase(std::pair<TimeStamp,std::shared_ptr<TimerTask>>(time,task));
        assert(result == 1);
    }


    void TimerEventInternal::handleEvent(){
        assert(_loop->isThisThread());
        readTimerfd(this->fd());
        auto time=_loop->GetTimeCache();
        std::pair<TimeStamp,std::shared_ptr<TimerTask>> timer(time,
            std::shared_ptr<TimerTask>(reinterpret_cast<TimerTask*>(UINTPTR_MAX)));
        auto iter = _timerSet.lower_bound(timer);
        if(iter==_timerSet.end() || iter->first.Gettime() < timer.first.Gettime())
        {
            LOG_ERR << "handleRead error";
            return;
        }
        std::vector<std::shared_ptr<TimerTask>> activeEvents;
        for(auto iter1=_timerSet.begin();iter1 != iter; iter1++)
        {
            activeEvents.emplace_back(iter1->second);
        }
        _timerSet.erase(_timerSet.begin(),iter);
        for(const auto pEvent : activeEvents)
        {
            pEvent->handleTask();
        }
        set_timerfd(this->fd(),_timerSet.begin()->first);
    }
}