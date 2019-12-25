//
// Created by zhangjianxiong on 2019/12/14.
//
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>
#include <memory>

#include "Buffer.h"
#include "IOevent.h"
#include "EventLoop.h"
#include "Log.h"

namespace Horizon
{
    IOevent::IOevent(EventLoop* loop,const Socket& sock,int timeout)
    :Eventbase(loop,sock),
    _state(kConnecting),
    _input(new Buffer),
    _output(new Buffer),
    _timeout(timeout),
    _timeoutTask()
    {}
	void IOevent::LinkTimer(std::shared_ptr<TimerTask> task)
	{
    	_timeoutTask=task;
	}
    void IOevent::SendData(const char* data, size_t len)
    {
        if(_state.load() == kConnected)
		{
            if(_loop->isThisThread())
            {
			  SendData_inloop(data, len);
            }
            else
            {
                _loop->RuninLoop(std::bind(&IOevent::SendData_inloop, this, data, len));
            }
        }
    }
    void IOevent::SendData_inloop(const char* data, size_t len)
    {
      	size_t remain=0;
        assert(_loop->isThisThread());
        if(!(this->isWriteEvent()) && _output->empty())
        {
          	int n = _sock.Write(data,len);
          	if(n>=0 && (size_t)n < len)
			{
          	  remain = len-n;
          	  data+=n;
          	  try
          	  {
          	    _output->Add_data_to_buffer(data,remain);
          	  }
          	  catch (...)
          	  {
          	    LOG_ERR << errno;
          	    return;
          	  }
			}
			if(!(this->isWriteEvent()))
			{
				this->enableWrite();
			}
        }
    }
	size_t IOevent::ExtractData(char* data,size_t len)
	{
       size_t n = _input->Extract_data_from_buffer(data,len);
       return n;
	}
    void IOevent::Shutdown()
    {
        if(_state.load() == kConnected)
        {
            this->SetConnectState(kDisconnecting);
            _loop->RuninLoop(std::bind(&IOevent::Shutdown_inloop,this));
        }
    }
    void IOevent::Shutdown_inloop()
    {
        if(!(this->isWriteEvent()))
		{
            _sock.Shutdownwrite();
        }
    }
    void IOevent::EnableRead()
    {
    	assert(_loop->isThisThread());
    	assert(_state.load()==kConnecting);
    	_loop->RuninLoop(std::bind(&IOevent::EnableRead_inloop,this));
    }
    void IOevent::EnableRead_inloop()
	{
    	assert(_loop->isThisThread());
    	this->enableRead();
    	this->SetConnectState(kConnected);

	}
    void IOevent::ForceClose()
    {
        if(_state.load() == kConnected || _state.load() == kConnecting)
        {
            this->SetConnectState(kDisconnecting);
            _loop->RuninLoop(std::bind(&IOevent::ForceClose_inloop,this));
        }
    }
    void IOevent::ForceClose_inloop()
    {
        assert(_loop->isThisThread());
        if(_state.load() == kDisconnecting || _state.load() == kConnected)
        {
        	this->disableAll();
            this->remove();
        }
    }

    void IOevent::handleRead()
    {
        size_t n = _input->Read_data_from_fd(_sock);
        if(n < 0)
        {
            int err = errno;
            LOG_ERR << err;
            handleError();
        }
        else if(n==0)
        {
            handleClose();
        }
        else
        {
        	std::shared_ptr<TimerTask> task = _timeoutTask.lock();
        	if(task != nullptr)
			{
        		_loop->RemoveTask(task);
        		_loop->RunAfter(_timeout,kMillisecondflag,task);
			}
            ConsumeData_inBuffer();
        }
    }
    void IOevent::handleWrite()
    {
        if(this->isWriteEvent())
        {
            int n = _output->Write_data_to_fd(_sock);
            if(n < 0)
            {
                LOG_ERR << "write error," << "error number is" << errno;
                return;
            }
            if(_output->empty())
            {
                this->disableWrite();
                Finish_KeepAlive();
                if(_state == kDisconnecting)
				{
                	Shutdown_inloop();
				}
            }

        }
    }
    void IOevent::handleClose()
    {
    	assert(_loop->isThisThread());
        if(_state != kConnected && _state != kDisconnecting)
		{
        	LOG_ERR << "connection is dead";
        	abort();
		}
		else
		{
			this->SetConnectState(kDisconnected);
		}
        this->disableAll();
        this->remove();
    }
    void IOevent::handleError()
    {
        int err = _sock.getSocketError();
        LOG_ERR << "socket error is" << err;
    }
    void IOevent::handleEvent()
    {
        if((_readyEvent & EPOLLHUP)&& !(_readyEvent & EPOLLIN))
        {
            handleClose();
        }
        else if (_readyEvent & EPOLLIN)
        {
            handleRead();
        }
        else if (_readyEvent & EPOLLOUT)
        {
            handleWrite();
        }
        else if (_readyEvent & EPOLLERR)
        {
            handleError();
        }
    }
}