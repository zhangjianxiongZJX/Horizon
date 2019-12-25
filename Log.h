//
// Created by zhangjianxiong on 2019/12/9.
//

#ifndef HORIZON_LOG_H
#define HORIZON_LOG_H

#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>
namespace Horizon {
    class Logstream
    {
    public:
        enum LogLevel
        {
            DEBUG,
            WARN,
            ERR,
        };

        Logstream(const char* file, const int line, LogLevel level, const char* func);
        ~Logstream();
        Logstream& operator<<(bool v) { _stream << v; return *this; }
        Logstream& operator<<(short v) { _stream << v; return *this;}
        Logstream& operator<<(unsigned short v) {_stream << v; return *this;}
        Logstream& operator<<(int v) {_stream << v; return *this;}
        Logstream& operator<<(unsigned int v) {_stream << v; return *this;}
        Logstream& operator<<(long v) {_stream << v; return *this;}
        Logstream& operator<<(unsigned long v) {_stream << v; return *this;}
        Logstream& operator<<(long long v) {_stream << v; return *this;}
        Logstream& operator<<(unsigned long long v) {_stream << v; return *this;}
        Logstream& operator<<(float v) {_stream << v; return *this;}
        Logstream& operator<<(double v) {_stream << v; return *this;}
        Logstream& operator<<(char v) {_stream << v; return *this;}
        Logstream& operator<<(const char* str) {_stream << str; return *this;}
        Logstream& operator<<(const unsigned char* str) {_stream << str; return *this;}
        Logstream& operator<<(const std::string& v) {_stream << v; return *this;}
    private:
        std::stringstream _stream;
        std::string _str;
    };

#define LOG_DEBUG Horizon::Logstream(__FILE__,__LINE__,Horizon::Logstream::DEBUG,__func__)
#define LOG_WARN Horizon::Logstream(__FILE__,__LINE__,Horizon::Logstream::WARN,__func__)
#define LOG_ERR  Horizon::Logstream(__FILE__,__LINE__,Horizon::Logstream::ERR,__func__)

#define Buffersize 4*1024*1024

    class Log
    {
    public:
        explicit Log(int dumpPeriod_sec=3);
        ~Log();

        void start();
        void set_gLog();
        void append(const std::stringstream& stream);

    private:
        void dump();

        typedef std::shared_ptr<std::string> BufferPtr;
        typedef std::vector<BufferPtr> BufferVec;

        BufferPtr _currentbuffer;// using buffer
        BufferPtr _nextbuffer; // ready buffer
        BufferVec _buffervec;  // full buffers

        std::ofstream _log;  //write log file
		std::chrono::duration<int> _waitTime;
        bool _isrunning;
        std::thread* _logThread;
        std::mutex _mutex;
        std::condition_variable _cond;

    };
}

#endif //HORIZON_LOG_H
