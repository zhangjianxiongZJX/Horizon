//
// Created by zhangjianxiong on 2019/12/12.
//

#ifndef HORIZON_SOCKET_H
#define HORIZON_SOCKET_H

#include <memory>
#include "SockAddr.h"

namespace Horizon
{
    class SockAddr;
    class Socket
    {
    public:
        Socket();
        Socket(const int& fd,const int& flag);
        ~Socket()= default;


        int Bind(const SockAddr& server_addr)const;
        int Listen(int size)const;
        int Connect(const SockAddr& server_addr)const;
        int Accept(struct sockaddr_in& clienAddr)const;
        int Close()const;
        int Write(const void* buf,size_t len);
        int getSocketError();

        int Shutdownwrite()const;
        int SetReuseAddr()const;
        int SetReusePort()const;
        const int& fd()const { return _fd;}
    private:
        int _fd;
        SockAddr _addr;
    };
}

#endif //HORIZON_SOCKET_H
