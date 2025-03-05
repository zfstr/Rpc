#pragma once

#include "lockqueue.h"
#include <string>
enum LogLevel
{
    INFO, // 普通日志信息
    ERROR, // 错误日志信息
};

// mprpc 日志系统
class Logger
{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);  // 设置日志级别
    void Log(std::string msg);  // 写日志
    
private:
    int m_loglevel;   // 日志级别
    LockQueue<std::string> m_lockQueue;   // 日志缓冲队列
    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};

// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    } while (0);

#define LOG_ERROR(logmsgformat, ...) \
    do \
    { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    } while (0);
    
    
    