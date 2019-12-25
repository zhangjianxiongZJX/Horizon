//
// Created by zhangjianxiong on 2019/12/15.
//

#include <cassert>
#include "LoopThreadManager.h"
#include "LoopThread.h"
#include "EventLoop.h"
namespace Horizon
{
    LoopThreadManager::LoopThreadManager(EventLoop* mainLoop,int size)
    :_mainLoop(mainLoop),
     _size(size),
     _loops(),
	 _nowIndex(-1),
     _mutex(),
     _subThreads()
    {
        for(int i=0;i<_size;++i)
        {
            //std::unique_ptr<LoopThread> subThread(new LoopThread);
            _subThreads.emplace_back(new LoopThread);
        }
    }
    void LoopThreadManager::Start()
    {
        assert(_mainLoop->isThisThread());
        for (const auto& subthread:_subThreads) {
            subthread->Start();
            auto loop = subthread->Getloop();
            _loops.push_back(loop);
        }
    }

    EventLoop* LoopThreadManager::get_NextLoop()
    {
        EventLoop* loop =nullptr;
        _nowIndex=(_nowIndex+1)%_size;
        loop=_loops[_nowIndex];
        return loop;
    }
}