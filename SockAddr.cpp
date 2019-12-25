//
// Created by zhangjianxiong on 2019/12/13.
//
#include <cstring>
#include "SockAddr.h"

namespace Horizon
{
    SockAddr::SockAddr()
    {
        ::memset(&_addr,0, sizeof(_addr));
    }
    SockAddr::SockAddr(const uint16_t port,bool loopback)
    {
        ::memset(&_addr,0, sizeof(_addr));
        _addr.sin_family=AF_INET;
        _addr.sin_port=htons(port);
        in_addr_t ip=loopback?INADDR_LOOPBACK:INADDR_ANY;
        _addr.sin_addr.s_addr=htonl(ip);
    }

    SockAddr::SockAddr(struct sockaddr_in addr)
    {
        ::memset(&_addr,0, sizeof(_addr));
        _addr=addr;
    }
    SockAddr::SockAddr(const SockAddr& sockaddr)
    {
        ::memset(&_addr,0, sizeof(_addr));
        _addr=sockaddr._addr;
    }

    void SockAddr::SetAddr(const struct sockaddr_in& addr)
    {
        _addr=addr;
    }
}