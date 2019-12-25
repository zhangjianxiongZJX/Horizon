//
// Created by zhangjianxiong on 2019/12/12.
//
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

#include "Socket.h"
#include "SockAddr.h"
#include "Log.h"

namespace Horizon
{
    Socket::Socket()
    {
        _fd=::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
        if(_fd<0)
		{
			LOG_ERR << "create socket failed";
			throw (std::logic_error("create socket failed"));
		}
        SetReuseAddr();
        SetReusePort();
    }
    Socket::Socket(const int& fd,const int& flag):_fd(fd)
    {
      	::fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|SOCK_NONBLOCK|SOCK_CLOEXEC);
        if(flag==1)
        {
            SetReuseAddr();
            SetReusePort();
        }
    }
    int Socket::Bind(const SockAddr & server_addr)const
    {
        struct sockaddr_in sock=(server_addr.GetAddr());
        int ret=::bind(_fd, (struct sockaddr*)&sock, sizeof(sock));
        if(ret <0)
        {
            LOG_ERR << "bind socket failed";
        }
        return ret;
    }
    int Socket::Listen(int size)const
    {
        int ret=::listen(_fd,size);
        if(ret <0)
        {
            LOG_ERR << "listen socket error";
        }
        return ret;
    }
    int Socket::Connect(const SockAddr& server_addr)const
    {
        struct sockaddr_in sock=(server_addr.GetAddr());
        int ret = ::connect(_fd,(struct sockaddr*)&sock, sizeof(sock));
        if(ret<0)
        {
            LOG_ERR << "socket connect error";
        }
        return ret;
    }
    int Socket::Accept(struct sockaddr_in& sock)const
    {
        socklen_t len = 0;
        int connectfd = ::accept4(_fd,(struct sockaddr*)&sock, &len,SOCK_NONBLOCK|SOCK_CLOEXEC);
        if(connectfd<0)
        {
            int err = errno;
            LOG_ERR << "accept error" << err;
        }
#ifdef DEBUG
        LOG_DEBUG << "accept successed!" << "client connection from:"
        	<< inet_ntoa(sock.sin_addr) << ":" << ntohs(sock.sin_port)
        	<< ",clientfd= " << connectfd;
#endif
        return connectfd;

    }
	int Socket::Write(const void* buf,size_t len)
	{
      int n = ::write(_fd,buf,len);
      if(n < 0)
	  {
        n=0;
        if(errno != EWOULDBLOCK)
		{
          LOG_ERR << errno;
		  if(errno == EPIPE || errno == ECONNRESET)
		  {
			return -1;
		  }
		  return n;
		}
	  }
      return n;
	}
    int Socket::Close()const
    {
        int ret = ::close(_fd);
        if(ret<-1)
        {
            LOG_ERR << "closed fd errror";
        }
        return ret;
    }
    int Socket::Shutdownwrite()const
    {
        if(::shutdown(_fd,SHUT_WR)<0)
        {
            LOG_ERR << "shutdown write failed";
            return -1;
        }
        return 0;
    }
    int Socket::SetReuseAddr()const
    {
        unsigned value=1;
        int ret = ::setsockopt(_fd,SOL_SOCKET,SO_REUSEADDR,&value, sizeof(value));
        if(ret<0)
        {
            LOG_ERR << "setReuseAddr failed";
        }
        return ret;
    }
    int Socket::SetReusePort()const
    {
        unsigned value=1;
        int ret = ::setsockopt(_fd,SOL_SOCKET,SO_REUSEPORT,&value, sizeof(value));
        if(ret<0)
        {
            LOG_ERR << "setReusePort failed";
        }
        return ret;
    }
    int Socket::getSocketError()
    {
        int optval;
        socklen_t optlen = static_cast<socklen_t>(sizeof optval);

        if (::getsockopt(_fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        {
            return errno;
        }
        else
        {
            return optval;
        }
    }
}