//
// Created by zhangjianxiong on 2019/12/17.
//

#include "Server.h"

#include "EventFactory.h"
#include "EventLoop.h"
#include "LoopThreadManager.h"
#include "Log.h"
namespace Horizon
{
    Server::Server(const std::string& servername,EventLoop* loop,
            const SockAddr& sockAddr,int connPoolSize,
            std::shared_ptr<EventFactory> factory)
            :_servername(servername),
            _log(new Log()),
            _sock(),
            _mainloop(loop),
            _connectPool(new LoopThreadManager(_mainloop,connPoolSize)),
            _listenner(new Listenner(_mainloop, _connectPool,_sock,sockAddr,factory))
    {
    	_log->set_gLog();
    	_log->start();
		_connectPool->Start();
    }
	Server::~Server()
	{
    	delete _connectPool;
	}

    void Server::Start()
    {
        _mainloop->RuninLoop(std::bind(&Listenner::StartListen,_listenner.get()));
    }
}