#include "logger.h"
#include <time.h>
#include <iostream>
Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for(;;)
        {
            // 获取当前的日期，然后取日志信息，写入响应的日志文件中
            time_t now = time(nullptr);
            tm * nowtm = localtime(&now);

            char filename[128] = {0};
            sprintf(filename,"%d-%d-%d-log.txt",nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE *pf = fopen(filename,"a+");
            if (pf == nullptr)
            {
                std::cout << "logger file: " << filename << " open error" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = m_lockQueue.Pop();
            char time_buf[128] = {0};
            sprintf(time_buf,"%d:%d:%d ==> [%s]",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec,(m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg += "\n";
            fputs(msg.c_str(),pf);

            fclose(pf);
        }
    });
    // 设置分离线程
    writeLogTask.detach();
}

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}
void Logger::Log(std::string msg)
{
    m_lockQueue.Push(msg);
}