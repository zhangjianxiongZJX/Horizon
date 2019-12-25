//
// Created by zhangjianxiong on 2019/12/14.
//

#include <unistd.h>
#include <sys/eventfd.h>

#include "Log.h"
#include "WatchEvent.h"
#include "EventLoop.h"
#include "Eventbase.h"
#include "Socket.h"
namespace Horizon
{
    int Create_eventfd()
    {
        int res=::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
        if(res < 0)
        {
            LOG_ERR << "create eventfd failed";
            abort();
        }
        return res;
    }
    WatchEvent::WatchEvent(EventLoop* loop)
    :Eventbase(loop,Socket(Create_eventfd(),0))
    {}

    void WatchEvent::handleEvent()
    {
        uint64_t data=0;
        int res = ::read(this->fd(),&data, sizeof(data));
        if(res<8)
        {
            LOG_WARN << "read" << res << "bytes data from watchfd";
        }
    }

}