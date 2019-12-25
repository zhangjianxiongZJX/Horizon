//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_WATCHEVENT_H
#define HORIZON_WATCHEVENT_H


#include "Eventbase.h"
#include "Socket.h"

namespace Horizon
{
    int Create_eventfd();

    class EventLoop;

    //wakeup from Epoll_wait
    class WatchEvent:public Eventbase
    {
    public:
        explicit WatchEvent(EventLoop*);
        void handleEvent()override ;
    private:

    };
}

#endif //HORIZON_WATCHEVENT_H
