//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_EVENTLOOPTHREAD_H
#define HORIZON_EVENTLOOPTHREAD_H

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace Horizon
{
    class EventLoop;
    class EventLoopThread
    {
    public:
        EventLoopThread();
        EventLoopThread(const EventLoopThread&)=delete;
        EventLoopThread(const EventLoopThread&&)=delete;
        EventLoopThread& operator=(const EventLoopThread&)=delete;
        ~EventLoopThread();
        void Start();
        EventLoop* Getloop();
    private:
        void EntranceFunc();

        std::unique_ptr<std::thread> _thread;
        std::thread::id _tid;
        EventLoop* _loop;
        std::mutex _mutex;
        std::condition_variable _cond;

    };
}


#endif //HORIZON_EVENTLOOPTHREAD_H
