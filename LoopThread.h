//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_LOOPTHREAD_H
#define HORIZON_LOOPTHREAD_H

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace Horizon
{
    class EventLoop;
    class LoopThread
    {
    public:
        LoopThread();
        LoopThread(const LoopThread&)=delete;
        LoopThread(const LoopThread&&)=delete;
        LoopThread& operator=(const LoopThread&)=delete;
        ~LoopThread();
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


#endif //HORIZON_LOOPTHREAD_H
