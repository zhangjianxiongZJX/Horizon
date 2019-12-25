//
// Created by zhangjianxiong on 2019/12/15.
//

#include <cassert>
#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
namespace Horizon
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop* mainLoop,int size)
    :_mainLoop(mainLoop),
     _size(size),
     _loops(),
	 _nowIndex(-1),
     _mutex(),
     _subThreads()
    {
        for(int i=0;i<_size;++i)
        {
            //std::unique_ptr<EventLoopThread> subThread(new EventLoopThread);
            _subThreads.emplace_back(new EventLoopThread);
        }
    }
    void EventLoopThreadPool::Start()
    {
        assert(_mainLoop->isThisThread());
        for (const auto& subthread:_subThreads) {
            subthread->Start();
            auto loop = subthread->Getloop();
            _loops.push_back(loop);
        }
    }

    EventLoop* EventLoopThreadPool::get_NextLoop()
    {
        EventLoop* loop =nullptr;
        _nowIndex=(_nowIndex+1)%_size;
        loop=_loops[_nowIndex];
        return loop;
    }
}