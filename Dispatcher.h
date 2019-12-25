//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_DISPATCHER_H
#define HORIZON_DISPATCHER_H

#include <set>
#include <map>
#include <vector>
namespace Horizon
{
    class EventLoop;
    class Eventbase;
    class Dispatcher
    {
    public:
        explicit Dispatcher(EventLoop* loop);
        Dispatcher(const Dispatcher&)= delete;
        Dispatcher(const Dispatcher&&)= delete;
        Dispatcher& operator=(const Dispatcher&)= delete;
        ~Dispatcher();
        void update(std::shared_ptr<Eventbase> pEvent);
        void remove(std::shared_ptr<Eventbase> pEvent);

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

#endif //HORIZON_DISPATCHER_H
