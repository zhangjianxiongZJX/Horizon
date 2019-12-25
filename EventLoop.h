//
// Created by zhangjianxiong on 2019/12/13.
//

#ifndef HORIZON_EVENTLOOP_H
#define HORIZON_EVENTLOOP_H

#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include "Eventbase.h"
#include "TimerTask.h"

namespace Horizon
{
    uint64_t GetthreadID(std::thread::id tid);

    class Eventbase;
    class Dispatcher;
    class TimerEventInternal;
    class WatchEvent;
    class EventLoop
    {
    public:
        explicit EventLoop(std::thread::id tid);
        EventLoop(const EventLoop&)=delete;
        EventLoop(const EventLoop&&)=delete;
        EventLoop& operator=(const EventLoop&)=delete;
        virtual ~EventLoop();
        void loop();
        void quit();
        void update(std::shared_ptr<Eventbase>);
        void remove(std::shared_ptr<Eventbase>);
        void RuninLoop(const std::function<void()>&);
        TimeStamp GetTimeCache(){ return _tv_cache;}
        void RunAt(TimeStamp timing,std::shared_ptr<TimerTask> task);
        void RunAfter(const int& delay,int timeflag,std::shared_ptr<TimerTask> task);
        void RemoveTask(const std::shared_ptr<TimerTask>& task);
        bool isThisThread()const
        {return _tid == GetthreadID(std::this_thread::get_id());}
    private:
        void queueInLoop(const std::function<void()>&);
        void ExecutePendingTask();
        void wakeup();

		uint64_t _tid;
		std::unique_ptr<Dispatcher> _dispatcher;
        std::shared_ptr<WatchEvent> _watcher;
        std::shared_ptr<TimerEventInternal> _timer;
        std::vector<std::shared_ptr<Eventbase>> _activeEvent;
        std::queue<std::function<void()>> _pendingTask;
        TimeStamp _tv_cache;
        bool _looping;
        std::atomic<bool> _quit;
        std::mutex _mutex;
        std::atomic<bool> _callingTask;
    };
}

#endif //HORIZON_EVENTLOOP_H
