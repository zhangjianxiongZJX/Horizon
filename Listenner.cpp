//
// Created by zhangjianxiong on 2019/12/15.
//

#include <cassert>
#include <cstring>

#include "Listenner.h"
#include "EventLoop.h"
#include "Eventbase.h"
#include "EventFactory.h"
#include "Log.h"
#include "Socket.h"
#include "IOevent.h"
#include "LoopThreadManager.h"

namespace Horizon {
    Listenner::Listenner(EventLoop *loop,LoopThreadManager* connectpool,
            const Socket& sock,const SockAddr& addr,const std::shared_ptr<EventFactory>& factory)
    : Eventbase(loop,sock),
   	 _connectPool(connectpool),
   	 _eventFactory(factory)
    {
    	_sock.Bind(addr);
    }

    Listenner::~Listenner()
    {
        this->remove();
    }
    void Listenner::StartListen()
    {
        int res = _sock.Listen(2048);
        if(res < 0)
        {
            LOG_ERR << "Listenner Start failed";
            abort();
        }
        this->enableRead();
    }

    void Listenner::handleEvent()
    {
        assert(_loop->isThisThread());
        EventLoop* ioloop = _connectPool->get_NextLoop();
        struct sockaddr_in clientAddr;
        int clientfd=0;
        ::memset(&clientAddr,0, sizeof(clientAddr));
        while((clientfd = _sock.Accept(clientAddr))>0)
		{
        	Socket clientsock(clientfd,1);
			std::shared_ptr<IOevent> ioEvent = _eventFactory->CreateEvent(ioloop,clientsock);
			int timeout = ioEvent->timeout();
			if(timeout > 0)
			{
				std::shared_ptr<TimerTask> timeTask(new EventTimeoutTask(ioEvent));
				ioEvent->LinkTimer(timeTask);
				ioloop->RuninLoop(std::bind(&EventLoop::RunAfter,ioloop,timeout,kMillisecondflag,timeTask));
			}
			ioloop->RuninLoop(std::bind(&IOevent::EnableRead,ioEvent));
		}
        if(clientfd ==-1)
		{
        	if(errno != EAGAIN && errno != ECONNABORTED
        	&& errno != EPROTO && errno != EINTR)
			{
        		LOG_ERR << "listenner accept error";
        		abort();
			}
		}
    }
}