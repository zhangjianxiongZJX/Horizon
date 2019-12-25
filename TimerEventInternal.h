//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_TIMEREVENTINTERNAL_H
#define HORIZON_TIMEREVENTINTERNAL_H

#include <set>
#include <memory>

#include "TimeStamp.h"
#include "Eventbase.h"
#include "TimerTask.h"
namespace Horizon
{
    int create_timerfd();
    int set_timerfd(const int&,TimeStamp);

    class EventLoop;
    class TimerTask;
    struct comparator
    {
        bool operator()(const std::pair<TimeStamp,std::shared_ptr<TimerTask>>& left,
            const std::pair<TimeStamp,std::shared_ptr<TimerTask>>& right)
        {
            if(left.first.Gettime() != right.first.Gettime())
            {
                return left.first.Gettime() < right.first.Gettime();
            }
            else
            {
                return left.second.get() < right.second.get();
            }
        }
    };
    class TimerEventInternal: public Eventbase
    {
    public:
        explicit TimerEventInternal(EventLoop*);

        //add timeout task
        void addTask(std::shared_ptr<TimerTask> task);
        //remove timeout task
        void removeTask(const std::shared_ptr<TimerTask>& task);

        void handleEvent()override;
    private:
        void addTaskInLoop(std::shared_ptr<TimerTask> task);
        void removeTaskInLoop(const std::shared_ptr<TimerTask>& task);

        void readTimerfd(const int&);
        // it can optimize
        std::set<std::pair<TimeStamp,std::shared_ptr<TimerTask>>,comparator> _timerSet;
    };
}

#endif //HORIZON_TIMEREVENTINTERNAL_H
