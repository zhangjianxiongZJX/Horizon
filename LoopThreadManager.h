//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_LOOPTHREADPOOL_H
#define HORIZON_LOOPTHREADPOOL_H

#include <vector>
#include <mutex>
#include <memory>
#include "LoopThread.h"
namespace Horizon
{
    class EventLoop;
    class LoopThreadManager
    {
    public:
        explicit LoopThreadManager(EventLoop* mainloop,int size);
        LoopThreadManager(const LoopThreadManager&)=delete;
        LoopThreadManager(const LoopThreadManager&&)=delete;
        LoopThreadManager& operator=(const LoopThreadManager&)=delete;
        void Start();
        EventLoop* get_NextLoop();
        std::vector<EventLoop*> get_allLoop(){ return _loops;}
    private:
        EventLoop* _mainLoop;
        int _size;
		std::vector<EventLoop*> _loops;
        int _nowIndex;
        std::mutex _mutex;
        std::vector<LoopThread*> _subThreads;

    };
}

#endif //HORIZON_LOOPTHREADPOOL_H
