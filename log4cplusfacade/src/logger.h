/***********************************************************************************
 * 文 件 名   : logger.h
 * 负 责 人   : zhoulee
 * 创建日期   : 2017年1月12日
 * 文件描述   : log4cplus接口封装
 * 版权说明   : Copyright (c) 2008-2017
 * 其    他   :
 * 修改日志   :
***********************************************************************************/

#ifndef _LOGGER_H
#define _LOGGER_H

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

enum LoggerLevelSize
{
    FATAL_SIZE = 2048,
    ERROR_SIZE = 2048,
    WARN_SIZE  = 2048,
    INFO_SIZE  = 2048,
    DEBUG_SIZE = 4096,
    TRACE_SIZE = 4096,
};

class LogManager
{
public:
    explicit LogManager(const std::string& configPath);

    ~LogManager();
};

#define IMPL_LOGGER_CLASS_TYPE_ log4cplus::Logger

#define IMPLEMENT_STATIC_LOGGER(logger_name) \
    static IMPL_LOGGER_CLASS_TYPE_ logger =                       \
        log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(logger_name))

#define DECLARE_GET_LOGGER(logger_name)         \
    static IMPL_LOGGER_CLASS_TYPE_& GetLogger() { \
        IMPLEMENT_STATIC_LOGGER(logger_name);       \
        return logger;                              \
    }

#define PRINT_LOG_C_FROMAT(level, size, fmt, args...)\
do {\
    \
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();\
    if(_logger.isEnabledFor(level))\
    {\
        char buf[size] = {0};\
        snprintf(buf, sizeof(buf), fmt, ##args);\
        _logger.forcedLog(level, LOG4CPLUS_STRING_TO_TSTRING(buf));\
    }\
}while(0)


#define LOG_TRACEL(logger, message) \
    LOG4CPLUS_TRACE(logger, LOG4CPLUS_TEXT(message))

#define LOG_DEBUGL(logger, message) \
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(message))

#define LOG_INFOL(logger, message) \
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT(message))

#define LOG_WARNL(logger, message) \
    LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(message))

#define LOG_ERRORL(logger, message) \
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(message))

#define LOG_FATALL(logger, message) \
    LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT(message))

#define INIT_LOGGER(log_config) LogManager log_manager__(log_config)

//C++ format logging
#define LOG_TRACE(message) LOG_TRACEL(GetLogger(), message)

#define LOG_DEBUG(message) LOG_DEBUGL(GetLogger(), message)

#define LOG_INFO(message) LOG_INFOL(GetLogger(), message)

#define LOG_WARN(message) LOG_WARNL(GetLogger(), message)

#define LOG_ERROR(message) LOG_ERRORL(GetLogger(), message)

#define LOG_FATAL(message) LOG_FATALL(GetLogger(), message)


//C format logging
#define CLOG_TRACE(...) \
    PRINT_LOG_C_FROMAT(log4cplus::TRACE_LOG_LEVEL, TRACE_SIZE, __VA_ARGS__)

#define CLOG_DEBUG(...) \
    PRINT_LOG_C_FROMAT(log4cplus::DEBUG_LOG_LEVEL, DEBUG_SIZE, __VA_ARGS__)

#define CLOG_INFO(...) \
    PRINT_LOG_C_FROMAT(log4cplus::INFO_LOG_LEVEL, INFO_SIZE, __VA_ARGS__)

#define CLOG_WARN(...) \
    PRINT_LOG_C_FROMAT(log4cplus::WARN_LOG_LEVEL, WARN_SIZE, __VA_ARGS__)

#define CLOG_ERROR(...) \
    PRINT_LOG_C_FROMAT(log4cplus::ERROR_LOG_LEVEL, ERROR_SIZE, __VA_ARGS__)

#define CLOG_FATAL(...) \
    PRINT_LOG_C_FROMAT(log4cplus::FATAL_LOG_LEVEL, FATAL_SIZE, __VA_ARGS__)

#endif

