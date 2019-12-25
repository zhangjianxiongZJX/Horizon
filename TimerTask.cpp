//
// Created by zhangjianxiong on 2019/12/14.
//

#include "TimerTask.h"
#include "EventLoop.h"
namespace Horizon
{
   TimerTask::TimerTask(TimeStamp time)
   :_timeout(time)
   {}

   void TimerTask::Set_timeout(const int& delay, int timeflag)
   {
      _timeout.Add_delay(delay,timeflag);
   }



}