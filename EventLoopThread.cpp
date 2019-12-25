//
// Created by zhangjianxiong on 2019/12/15.
//

#include <functional>
#include "EventLoop.h"
#include "EventLoopThread.h"

namespace Horizon
{
    EventLoopThread::EventLoopThread()
    :_thread(nullptr),
    _tid(),
    _loop(nullptr),
    _mutex(),
    _cond()
    {}
    EventLoopThread::~EventLoopThread()
    {
        _loop->quit();
        _cond.notify_all();
        _thread->join();
    }
    EventLoop* EventLoopThread::Getloop()
    {
        EventLoop* loop= nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            while(_loop == nullptr)
            {
                _cond.wait(lock);
            }
            loop = _loop;
        }

        return loop;
    }
    void EventLoopThread::Start()
    {
        auto factor = std::bind(&EventLoopThread::EntranceFunc,this);
        std::thread* thd=new std::thread(factor);
        _thread.reset(thd);
        _tid=_thread->get_id();
    }

    void EventLoopThread::EntranceFunc()
    {
      	_tid=std::this_thread::get_id();
        EventLoop* loop(new EventLoop(_tid));
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _loop=loop;
            _cond.notify_all();
        }
        _loop->loop();
        {
            std::lock_guard<std::mutex> lock(_mutex);
            delete _loop;
        }

    }
}