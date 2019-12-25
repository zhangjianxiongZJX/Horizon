//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_EPOLL_H
#define HORIZON_EPOLL_H

#include <set>
#include <map>
#include <vector>
namespace Horizon
{
    class EventLoop;
    class Eventbase;
    class Epoll
    {
    public:
        explicit Epoll(EventLoop* loop);
        Epoll(const Epoll&)= delete;
        Epoll(const Epoll&&)= delete;
        Epoll& operator=(const Epoll&)= delete;
        ~Epoll();
        void update(std::shared_ptr<Eventbase> pEvent);
        void remove(std::shared_ptr<Eventbase> pEvent);

        //time out period (ms)
        int dispatch(int,std::vector<std::shared_ptr<Eventbase>>&);
    private:
        void apply_change(std::shared_ptr<Eventbase>,int);

        int _epfd;
        EventLoop* _loop;
        //std::set<std::shared_ptr<Eventbase>>  _allevent;
        std::map<int,std::shared_ptr<Eventbase>> _registEvent;
        std::vector<struct epoll_event> _readyEvent;
    };

}

#endif //HORIZON_EPOLL_H
