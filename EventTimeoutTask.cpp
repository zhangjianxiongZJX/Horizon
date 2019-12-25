//
// Created by zhangjianxiong on 2019/12/24.
//
//#include "Eventbase.h"
#include "EventTimeoutTask.h"
#include "EventLoop.h"
#include "IOevent.h"
namespace Horizon
{
	EventTimeoutTask::EventTimeoutTask(std::shared_ptr<IOevent> event)
	:TimerTask(),
	_event(event)
	{
	}

	void EventTimeoutTask::handleTask()
	{
		EventLoop* loop = _event->loop();
		TimeStamp time = loop->GetTimeCache();
		if(_timeout.Gettime() < time.Gettime())
		{
			_event->ForceClose();
		}

	}
}