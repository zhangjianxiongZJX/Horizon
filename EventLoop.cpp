//
// Created by zhangjianxiong on 2019/12/14.
//

#include <cassert>
#include <unistd.h>

#include "Log.h"
#include "EventLoop.h"
#include "WatchEvent.h"
#include "Dispatcher.h"
#include "TimerEventInternal.h"

namespace Horizon
{
    constexpr int kDefaultTimeout=10000;
    uint64_t GetthreadID(std::thread::id tid)
    {
        std::stringstream ss;
        ss << tid;
        return std::stoull(ss.str());
    }

    EventLoop::EventLoop(const std::thread::id tid)
    : _tid(GetthreadID(tid)),
	  _dispatcher(new Dispatcher(this)),
	  _watcher(new WatchEvent(this)),
	  _timer(new TimerEventInternal(this)),
	  _activeEvent(),
	  _pendingTask(),
	  _tv_cache(),
	  _looping(false),
	  _quit(false),
	  _mutex(),
	  _callingTask(false)
    {
        _timer->enableRead();
        _watcher->enableRead();
    }
    EventLoop::~EventLoop()
    {
        _timer->remove();
        _watcher->remove();
		//delete _timer;
		//delete _watcher;
    }
    void EventLoop::loop()
    {
        assert(!_looping);
        assert(isThisThread());
        _looping=true;
        _quit= false;
        _activeEvent.clear();
        while(!_quit)
        {
            _activeEvent.clear();
            _tv_cache.Clear();
            _dispatcher->dispatch(-1,_activeEvent);
            _tv_cache.updateTime();
            for (const auto& event:_activeEvent)
            {
                event->handleEvent();
            }
            ExecutePendingTask();
        }
        _looping= false;
    }

    void EventLoop::update(std::shared_ptr<Eventbase> pEvent)
    {
        assert(pEvent->loop() == this);
        assert(isThisThread());
        _dispatcher->update(pEvent);
    }
    void EventLoop::remove(std::shared_ptr<Eventbase> pEvent)
    {
        assert(pEvent->loop() == this);
        assert(isThisThread());
        _dispatcher->remove(pEvent);
    }


    void EventLoop::quit()
    {
        _quit=true;
        if(!isThisThread())
        {
            wakeup();
        }
    }
    void EventLoop::RuninLoop(const std::function<void(void)>& callable)
    {
        if(isThisThread())
        {
            callable();
        }
        else
        {
            queueInLoop(callable);
        }
    }
    void EventLoop::queueInLoop(const std::function<void (void)>& callable)
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _pendingTask.push(callable);
        }
        if(!isThisThread() || _callingTask.load())
        {
            wakeup();
        }

    }
    void EventLoop::RunAt(TimeStamp timing,std::shared_ptr<TimerTask> task)
    {
        task->Set_timeout(timing);
        _timer->addTask(task);
    }
    void EventLoop::RunAfter(const int& delay,int timeflag,std::shared_ptr<TimerTask> task)
    {
        task->Set_timeout(delay,timeflag);
        _timer->addTask(task);
    }
    void EventLoop::RemoveTask(const std::shared_ptr<TimerTask>& task)
    {
        _timer->removeTask(task);
    }
    void EventLoop::ExecutePendingTask()
    {
        std::queue<std::function<void()>> tasks;
        _callingTask.store(true);
        {
            std::lock_guard<std::mutex> lock(_mutex);
            tasks.swap(_pendingTask);
        }
        while (!tasks.empty())
        {
            auto task=tasks.front();
            tasks.pop();
            task();
        }
        _callingTask= false;
    }
    void EventLoop::wakeup()
    {
        uint64_t data=1;
        int res=::write(_watcher->fd(),&data, sizeof(data));
        if(res<8)
        {
            LOG_WARN << "write" << res << "bytes data to watcher";
        }
    }
}