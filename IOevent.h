//
// Created by zhangjianxiong on 2019/12/14.
//

#ifndef HORIZON_IOEVENT_H
#define HORIZON_IOEVENT_H

#include <atomic>
#include "Eventbase.h"
#include "EventTimeoutTask.h"
namespace Horizon
{
    enum EventState{kConnecting,kConnected,kDisconnecting,kDisconnected};
    class EventLoop;
    class Buffer;
    class IOevent:public Eventbase
    {
    public:
        IOevent(EventLoop*,const Socket&,int);

        void handleEvent()override ;
        void SendData(const char* data, size_t len);
        size_t ExtractData(char* data,size_t len);
        void Shutdown();
        void ForceClose();
		void EnableRead();
        void SetConnectState(const EventState& state){_state.store(state);}
        //keep-alive timeout
		void LinkTimer(std::shared_ptr<TimerTask> task);
        int timeout(){return  _timeout;}
    private:
        /*need override*/
        virtual void ConsumeData_inBuffer()=0;
		//if it's not keep-alive,override it,and set Connection state kDisconnecting
        virtual void Finish_KeepAlive()=0;

        void SendData_inloop(const char* data, size_t len);
        void Shutdown_inloop();
        void ForceClose_inloop();
		void EnableRead_inloop();
        void handleRead();
        void handleWrite();
        void handleClose();
        void handleError();

        std::atomic<EventState> _state;
     protected:
        std::shared_ptr<Buffer> _input;
        std::shared_ptr<Buffer> _output;

        //only for keep-alive
        int  _timeout;//millisecond
        std::weak_ptr<TimerTask> _timeoutTask;
    };
}

#endif //HORIZON_IOEVENT_H
