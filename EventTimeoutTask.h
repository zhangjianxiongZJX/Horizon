//
// Created by zhangjianxiong on 2019/12/24.
//

#ifndef HORIZON_HORIZONSERVER_EVENTTIMEOUTTASK_H_
#define HORIZON_HORIZONSERVER_EVENTTIMEOUTTASK_H_

#include <memory>

#include "TimerTask.h"
//#include "IOevent.h"
#include "TimeStamp.h"
namespace Horizon
{
	class IOevent;
	class EventTimeoutTask:public TimerTask
	{
	 public:
		explicit EventTimeoutTask(std::shared_ptr<IOevent> event);
		~EventTimeoutTask()final= default;

		void handleTask()override ;
	 private:
		std::shared_ptr<IOevent> _event;
	};
}

#endif //HORIZON_HORIZONSERVER_EVENTTIMEOUTTASK_H_
