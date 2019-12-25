//
// Created by zhangjianxiong on 2019/12/13.
//

#ifndef HORIZON_EVENTBASE_H
#define HORIZON_EVENTBASE_H
#include <memory>
#include "TimeStamp.h"
#include "Socket.h"
namespace Horizon
{
    constexpr int kNew=-1;
    constexpr int kAdded=1;
    constexpr int kDeleted = 2;
    class EventLoop;
 class Eventbase:public std::enable_shared_from_this<Eventbase>
    {
    public:
        Eventbase(EventLoop*,const Socket&);
		Eventbase(const Eventbase&)=delete;
		Eventbase(const Eventbase&&)=delete;
		Eventbase& operator=(const Eventbase&)=delete;
        virtual ~Eventbase();

        /*need override*/
        virtual void handleEvent()=0;
        const int& fd()const{return _sock.fd();}
        const Socket& socket()const{ return _sock;}
        unsigned int event(){ return _event;}
        EventLoop* loop(){ return _loop;}

        unsigned int get_readyEvent(){ return _readyEvent;}
        //get state in epoll
        const int& GetOpcodeFlag(){ return _opflag;}

        //set triggled event
        void set_readyEvent(int rEvent){_readyEvent=rEvent;}
        //set state in epoll
        void SetOpcodeFlag(const int& flag){_opflag=flag;}

        void enableRead(){_event|=kReadEvent;update();}
        void enableWrite(){_event|=kWriteEvent;update();}
        void disableWrite(){_event &= ~kWriteEvent;update();}
        void disableRead(){_event &= ~kReadEvent;update();}
        void disableAll(){_event=0;update();}
        bool isWriteEvent()const{ return  _event & kWriteEvent;}
        bool isReadEvent()const{ return  _event & kReadEvent;}

        void remove();

  	protected:
	 	EventLoop* _loop;
	 	Socket _sock;
	 	unsigned int _event;
	 	unsigned int _readyEvent;

    private:
        void update();

        bool _isInLoop;
        int _opflag;
        static const unsigned int kReadEvent;
        static const unsigned int kWriteEvent;

    };
}



#endif //HORIZON_EVENTBASE_H
