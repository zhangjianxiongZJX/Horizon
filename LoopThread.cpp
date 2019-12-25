//
// Created by zhangjianxiong on 2019/12/15.
//

#include <functional>
#include "EventLoop.h"
#include "LoopThread.h"

namespace Horizon
{
    LoopThread::LoopThread()
    :_thread(nullptr),
    _tid(),
    _loop(nullptr),
    _mutex(),
    _cond()
    {}
    LoopThread::~LoopThread()
    {
        _loop->quit();
        _cond.notify_all();
        _thread->join();
    }
    EventLoop* LoopThread::Getloop()
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
    void LoopThread::Start()
    {
        auto factor = std::bind(&LoopThread::EntranceFunc,this);
        std::thread* thd=new std::thread(factor);
        _thread.reset(thd);
        _tid=_thread->get_id();
    }

    void LoopThread::EntranceFunc()
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