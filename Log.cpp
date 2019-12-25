//
// Created by zhangjianxiong on 2019/12/9.
//

#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <cstdio>
#include <cassert>
#include "Log.h"

using namespace Horizon;

Log* g_log;

Logstream::Logstream(const char* file, const int line, LogLevel level, const char* func)
{
    pid_t tid = static_cast<pid_t> (::syscall(SYS_gettid));
    char tidToString[32];
    int len = snprintf(tidToString, sizeof(tidToString), "%5d", tid);
    std::string str_tid(tidToString, len);

    std::string str;
    switch (level)
    {
        case DEBUG:
            str = "DEBUG";
            break;
        case WARN:
            str = "WARN";
            break;
        case ERR:
            str = "ERROR";
            break;
        default:
            str = "???";
            break;
    }

    _stream  << str_tid << file << ":" << line << " "<< str << " " << func << " ";
}
Logstream::~Logstream()
{
    if (g_log)
    {
        _stream << std::endl;
        g_log->append(_stream);
    }
}

	Log::Log(int dumpPeriod_sec)
	:_currentbuffer(new std::string),
 	 _nextbuffer(new std::string),
	 _buffervec(),
	 _log("Log.log",std::ios::app),
	 _waitTime(std::chrono::duration<int>(dumpPeriod_sec)),
	 _isrunning(true),
	 _logThread(nullptr),
	 _mutex(),
 	 _cond()
 	 {}
	Log::~Log()
	{
    	if (_isrunning)
    	{
       	 	_cond.notify_all();
    	}
    	_logThread->join();
    	_isrunning = false;
    	_log.close();
	}
	void Log::start()
	{
    	_logThread=new std::thread(std::bind(&Log::dump,this));
    	assert(_logThread!= nullptr);
	}
	void Log::set_gLog()
	{
    	g_log = this;
	}
	void Log::append(const std::stringstream &stream)
	{
    	std::lock_guard<std::mutex> lock(_mutex);
    	if (_currentbuffer->size() + stream.str().size() < Buffersize)
    	{
        	_currentbuffer->append(stream.str());
    	}
    	else
    	{
        	_buffervec.push_back(_currentbuffer);
        	if (_nextbuffer != nullptr)
        	{
            	_currentbuffer = std::move(_nextbuffer);
        	}
        	else
        	{
            	_currentbuffer.reset(new std::string);
        	}
        	_currentbuffer->append(stream.str());
    	}
    	_cond.notify_one();
	}

	void Log::dump()
	{
		BufferPtr tempBuffer1(new std::string);
    	BufferPtr tempBuffer2(new std::string);
    	BufferVec tempBufferVec;
    	while (_isrunning)
    	{
        	std::unique_lock<std::mutex> lock(_mutex);
        	_cond.wait_for(lock, _waitTime, [this] {return !this->_buffervec.empty(); });

        	_buffervec.push_back(_currentbuffer);
        	if (_buffervec.size() >= 25)
        	{
            	if (_log.is_open())
            	{
                	_log << "too much logs";
            	}
            	_buffervec.erase(_buffervec.begin() + 2, _buffervec.end());
        	}
        	_currentbuffer = std::move(tempBuffer1);
        	tempBufferVec.swap(_buffervec);
        	assert(!tempBufferVec.empty());

        	if (_nextbuffer == nullptr)
        	{
            	_nextbuffer = std::move(tempBuffer2);
        	}
        	if (_log.is_open())
        	{
            	for (const auto& str : tempBufferVec)
            	{
                	_log << *str;
            	}
        	}

        	if (tempBufferVec.size() >= 2)
        	{
            	if (tempBufferVec.size() > 2)
            	{
                	tempBufferVec.resize(2);
            	}
            	tempBuffer2 = tempBufferVec.back();
            	tempBuffer2->clear();
            	tempBufferVec.pop_back();
            	tempBuffer1 = tempBufferVec.back();
            	tempBuffer1->clear();
            	tempBufferVec.pop_back();
        	}
        	else
        	{
            	tempBuffer1 = tempBufferVec.back();
            	tempBufferVec.pop_back();
            	tempBuffer1->clear();
        	}

    	}

	}