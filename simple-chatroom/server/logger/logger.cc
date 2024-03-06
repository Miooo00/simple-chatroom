#include "logger.h"
#include <time.h>
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <stdarg.h>

using namespace uname::utility;

const char* Logger::s_level[LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger* Logger::m_instance = NULL;

Logger::Logger():m_level(DEBUG),m_max(0),m_len(0)
{
}

Logger::~Logger()
{
    close();
}

Logger* Logger::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::open(const string& filename)
{
    m_filename = filename;
    m_fout.open(filename,ios::app);
    if(m_fout.fail())
    {
        throw std::logic_error("open file failed " + filename);
    }
    m_fout.seekp(0,ios::end);   //将文件指针指向文件末尾 偏移量为0
    m_len = m_fout.tellp();     //获得文件指针位置 文件末即为文件字节数
}

void Logger::close()
{
    m_fout.close();
}

void Logger::log(Level level,const char* file,int line,const char* format,...)
{
    if(m_level > level)     //如果设置的日志等级大于当前等级 则不记录日志
    {
        return;
    }
    if(m_fout.fail())       //文件打开失败
    {
        throw std::logic_error("open file failed" + m_filename);
    }

    time_t ticks = time(NULL);
    struct tm* ptn = localtime(&ticks);
    char timestamp[32];
    memset(timestamp,0,sizeof(timestamp));
    strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptn);  //获得格式化的时间
    const char* fmt = "%s %s %s:%d ";   //格式
    int size = snprintf(NULL, 0, fmt, timestamp,s_level[level],file,line);  //不输出字符串到指定缓存中 只获取字符串的大小
    if(size > 0)
    {
        char* buffer = new char[size+1];
        snprintf(buffer,size+1,fmt,timestamp,s_level[level],file,line);     //写入到指定变量缓存中
        buffer[size] = '\0';    //结束标识
        // std::cout << buffer << std::endl;
        m_fout << buffer;       //写入文件
        m_len += size;          //总字节数计算
        delete buffer;
    }
    //处理传入的format部分
    va_list arg_ptr;
    va_start(arg_ptr,format);   //可变参数部分
    size = vsnprintf(NULL,0,format,arg_ptr);
    va_end(arg_ptr);
    if(size > 0)
    {
        char* content = new char[size + 1];
        va_start(arg_ptr,format);
        size = vsnprintf(content,size+1,format,arg_ptr);
        va_end(arg_ptr);
        // std::cout << content << std::endl;
        m_fout << content;
        m_len += size;
        delete(content);
    }
    m_fout << "\n";
    m_fout.flush();

    if(m_len >= m_max && m_max > 0)
    {
        rotate();
    }
}

void Logger::rotate()
{
    close();
    time_t ticks = time(NULL);
    struct tm* ptn = localtime(&ticks);
    char timestamp[32];
    memset(timestamp,0,sizeof(timestamp));
    strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",ptn);
    string filename = m_filename + timestamp;
    if(rename(m_filename.c_str(),filename.c_str()) != 0)
    {
        throw std::logic_error("rename log file failed: "+string(strerror(errno)));
    }
    open(m_filename);
}