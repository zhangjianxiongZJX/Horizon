//
// Created by zhangjianxiong on 2019/12/15.
//

#ifndef HORIZON_LISTENNER_H
#define HORIZON_LISTENNER_H

#include <set>
#include "Eventbase.h"

namespace Horizon
{
    class EventFactory;
    class LoopThreadManager;
    class Listenner: public Eventbase
    {
    public:
        Listenner(EventLoop *loop,LoopThreadManager* connectpool,
                const Socket& sock,const SockAddr& addr,const std::shared_ptr<EventFactory>& factory);
        ~Listenner()final ;
        void StartListen();
        void handleEvent()override ;
    private:
        LoopThreadManager* _connectPool;
        std::shared_ptr<EventFactory> _eventFactory;
    };
}

#endif //HORIZON_LISTENNER_H
