//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_EVENTFACTORY_H
#define HORIZON_EVENTFACTORY_H

#include <memory>
#include "TimeStamp.h"
#include "Socket.h"
namespace Horizon
{
    class IOevent;
    class EventLoop;
    class EventFactory
    {
    public:
        virtual std::shared_ptr<IOevent> CreateEvent(EventLoop*,const Socket&)=0;
    };
}

#endif //HORIZON_EVENTFACTORY_H
