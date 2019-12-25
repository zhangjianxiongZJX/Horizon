//
// Created by zhangjianxiong on 2019/12/17.
//

#ifndef HORIZON_SERVER_H
#define HORIZON_SERVER_H

#include <memory>
#include <string>
#include <thread>
#include "Listenner.h"
#include "Socket.h"

namespace Horizon
{
    class Listenner;
    class EventLoop;
    class EventFactory;
    class LoopThreadManager;
    class Log;
    class Server
    {
    public:
        Server(const std::string& servername,EventLoop* loop,
                const SockAddr& sockAddr,int connPoolSize,
                std::shared_ptr<EventFactory> factory);
		Server(const Server&)=delete;
		Server(const Server&&)=delete;
		Server& operator=(const Server&)=delete;
        void Start();
		~Server();
    private:
        const std::string _servername;
        std::shared_ptr<Log> _log;
        Socket _sock;
        EventLoop* _mainloop;
        LoopThreadManager* _connectPool;
        std::shared_ptr<Listenner> _listenner;
    };
}

#endif //HORIZON_SERVER_H
