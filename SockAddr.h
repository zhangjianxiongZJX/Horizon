//
// Created by zhangjianxiong on 2019/12/13.
//

#ifndef HORIZON_SOCKADDR_H
#define HORIZON_SOCKADDR_H

#include <arpa/inet.h>

namespace Horizon
{
    class SockAddr
    {
    public:
        SockAddr();
        SockAddr(uint16_t port,bool loopback);
        explicit SockAddr(struct sockaddr_in addr);
        SockAddr(const SockAddr& sockaddr);
        SockAddr& operator=(const SockAddr& sockaddr)= default;

        void SetAddr(const struct sockaddr_in& addr);
        struct sockaddr_in GetAddr()const{ return _addr;}
    private:
        struct sockaddr_in _addr;
    };
}


#endif //HORIZON_SOCKADDR_H
