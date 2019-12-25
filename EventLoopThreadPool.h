//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_EVENTLOOPTHREADPOOL_H
#define HORIZON_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <mutex>
#include <memory>
#include "EventLoopThread.h"
namespace Horizon
{
    class EventLoop;
    class EventLoopThreadPool
    {
    public:
        explicit EventLoopThreadPool(EventLoop* mainloop,int size);
        EventLoopThreadPool(const EventLoopThreadPool&)=delete;
        EventLoopThreadPool(const EventLoopThreadPool&&)=delete;
        EventLoopThreadPool& operator=(const EventLoopThreadPool&)=delete;
        void Start();
        EventLoop* get_NextLoop();
        std::vector<EventLoop*> get_allLoop(){ return _loops;}
    private:
        EventLoop* _mainLoop;
        int _size;
		std::vector<EventLoop*> _loops;
        int _nowIndex;
        std::mutex _mutex;
        std::vector<EventLoopThread*> _subThreads;

    };
}

#endif //HORIZON_EVENTLOOPTHREADPOOL_H
