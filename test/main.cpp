//
// Created by zhangjianxiong on 2019/12/18.
//
#include <memory>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "EventLoop.h"
#include "Server.h"
#include "IOevent.h"
#include "EventFactory.h"
using namespace Horizon;
class myEvent:public IOevent
{
public:
	myEvent(EventLoop* loop,const Socket& sock,int timeout)
	:IOevent(loop,sock,timeout)
	{}
    void ConsumeData_inBuffer()override
    {
		char req_data[4096];
		this->ExtractData(req_data,0);
		std::cout << req_data << std::endl;
		//char res[1024];
		std::string str = "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
		auto res = str.c_str();
		auto size = str.size();
		this->SendData(res, size);
    }
    void Finish_KeepAlive()override
    {
		//SetConnectState(kDisconnecting);
    }
private:
};
// keep-alive event factory
 class myFactory:public EventFactory
{
 public:
	myFactory(int timeout)
	:EventFactory()
	{
		_timeout=timeout;
	}
	std::shared_ptr<IOevent> CreateEvent(EventLoop* loop,const Socket& sock)override
	{
	  return std::make_shared<myEvent>(loop,sock,_timeout);
	}
  private:
	int _timeout;
};

 int main()
 {
   const std::string name = "myServer";
   std::thread::id _id = std::this_thread::get_id();
   SockAddr addr(8000, false);
   int timeout = 10000;
   std::shared_ptr<EventFactory> factory(new myFactory(timeout));
   EventLoop* mainloop = new EventLoop(_id);
   Server server(name,mainloop,addr,5,factory);
   server.Start();
   mainloop->loop();
 }


/*
#include <signal.h>
void handle_pipe(int sig)
{
//do nothing
}
int main()
{
  struct sigaction sa;
  sa.sa_handler = handle_pipe;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE,&sa,NULL);
//do something
}
*/