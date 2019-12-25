//
// Created by zhangjianxiong on 2019/12/14.
//
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "EventLoop.h"
#include "Eventbase.h"
#include "Log.h"
#include "Dispatcher.h"
namespace Horizon
{
    constexpr size_t InitMaxSize=1024;
    constexpr size_t MAXNEVENT = 4096;


    Dispatcher::Dispatcher(EventLoop* loop)
    : _loop(loop),
    //_allevent(),
    _registEvent(),
	  _readyEvent(InitMaxSize)
    {
        _epfd=::epoll_create1(EPOLL_CLOEXEC);
        if(_epfd==-1)
        {
            LOG_WARN << "epoll create failed";
        }
    }
    Dispatcher::~Dispatcher()
    {
        ::close(_epfd);
    }

    void Dispatcher::update(std::shared_ptr<Eventbase> pEvent)
    {
        int flag=pEvent->GetOpcodeFlag();
        unsigned int ev = pEvent->event();
        if(flag==kAdded)
        {
        	if(ev == 0)
			{
				assert(_registEvent.find(pEvent->fd())!=_registEvent.end());
				apply_change(pEvent,EPOLL_CTL_DEL);
				pEvent->SetOpcodeFlag(kDeleted);
			}
			else
			{
				assert(_registEvent.find(pEvent->fd()) != _registEvent.end());
				_registEvent[pEvent->fd()]=pEvent;
				apply_change(pEvent,EPOLL_CTL_MOD);
			}

        }
        else
        {
            assert(_registEvent.find(pEvent->fd())==_registEvent.end());
            _registEvent.insert(std::pair<int, std::shared_ptr<Eventbase>>(pEvent->fd(),pEvent));
			apply_change(pEvent,EPOLL_CTL_ADD);
            pEvent->SetOpcodeFlag(kAdded);
        }


    }
    void Dispatcher::remove(std::shared_ptr<Eventbase> pEvent)
    {
        assert(_registEvent.find(pEvent->fd()) != _registEvent.end());
        assert(pEvent->event()==0);
        if(pEvent->GetOpcodeFlag()==kAdded)
		{
			apply_change(pEvent,EPOLL_CTL_DEL);
		}
		_registEvent.erase(pEvent->fd());
    }
    int Dispatcher::dispatch(int timeout,std::vector<std::shared_ptr<Eventbase>>& activelist)
    {
        int res= ::epoll_wait(_epfd,&(*_readyEvent.begin()),_readyEvent.size(),timeout);
        if(res == -1)
		{
        	if(errno != EINTR)
			{
        		LOG_WARN << "epoll wait error" << errno;
				return -1;
			}
			return 0;
		}
        assert((size_t)res <= _readyEvent.size());
		for(int i=0;i<res;i++)
		{
			auto fd = _readyEvent[i].data.fd;
			auto event = _registEvent[fd];
			event->set_readyEvent(_readyEvent[i].events);
			activelist.push_back(event);
		}

        if((unsigned int)res == _readyEvent.size() && _readyEvent.size()<MAXNEVENT)
        {
            size_t oldsize=_readyEvent.size();
            _readyEvent.resize(oldsize*2);
        }
		return 0;
    }

    void Dispatcher::apply_change(std::shared_ptr<Eventbase> pEvent, int op)
    {
        struct epoll_event epev;
        ::memset(&epev,0, sizeof(epev));
        unsigned int event = pEvent->event();
        epev.data.fd=pEvent->fd();
        epev.events=event;
        epev.events|=EPOLLET;
        if(::epoll_ctl(_epfd,op,pEvent->fd(),&epev)==0)
        {
            return;
        }
        switch(op)
        {
            case EPOLL_CTL_MOD:
                if(errno == ENOENT)//fd closed and reopened
                {
                    if(::epoll_ctl(_epfd,EPOLL_CTL_ADD,pEvent->fd(),&epev)==-1)
                    {
                        LOG_WARN << "EPOLL ADD AGAIN FAILED";
                        return;
                    }
                    return;
                }
                break;
            case EPOLL_CTL_ADD:
                if(errno == EEXIST)//already exist
                {
                    if(::epoll_ctl(_epfd,EPOLL_CTL_MOD,pEvent->fd(),&epev)==-1)
                    {
                        LOG_WARN << "EPOLL MOD AGAIN FAILED";
                        return;
                    }
                    return;
                }
                break;
            case EPOLL_CTL_DEL:
                if(errno == ENOENT || errno == EBADF || errno == EPERM)
                {
                    LOG_ERR << "EPOLL DEL FAILED";
                    return;
                }
                break;
            default:
                break;
        }
        LOG_WARN << "epoll_ctl failed";
    }
}