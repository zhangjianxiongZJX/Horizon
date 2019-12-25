//
// Created by zhangjianxiong on 2019/12/13.
//
#include <memory>
#include <sys/epoll.h>
#include "Eventbase.h"
#include "EventLoop.h"
#include "Log.h"
namespace Horizon
{
    const unsigned int Eventbase::kReadEvent = EPOLLIN | EPOLLPRI;
    const unsigned int Eventbase::kWriteEvent = EPOLLOUT;

    Eventbase::Eventbase(EventLoop* loop,const Socket& sock)
    :_loop(loop),
     _sock(sock),
     _event(0),
     _readyEvent(0),
     _isInLoop(false),
     _opflag(kNew)
    {}
    Eventbase::~Eventbase()
    {
        int ret = _sock.Close();
        if(ret < 0)
        {
            LOG_ERR << "close fd error";
        }
    }

    void Eventbase::remove()
    {
        _isInLoop= false;
        _loop->remove(shared_from_this());
    }
    void Eventbase::update()
    {
        //std::shared_ptr<Eventbase> aaa  = shared_from_this();
        _loop->update(shared_from_this());
        _isInLoop=true;
    }


}