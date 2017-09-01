#include "logger.h"
#include "timestamp.h"

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

using namespace appserver::base;

DECLARE_GET_LOGGER("Logger.Global")

void FunctionTest()
{
    for(int i = 0; i < 1000; ++i)
    {
        LOG_TRACE("i " << i << " threadid " << pthread_self());
    }

    for(int i = 0; i < 1000; ++i)
    {
        LOG_DEBUG("debug " << i);
    }

    for(int i = 0; i < 1000; ++i)
    {
        LOG_INFO("info " << i);
    }

    for(int i = 0; i < 1000; ++i)
    {
        LOG_WARN("warn " << i);
    }

    for(int i = 0; i < 1000; ++i)
    {
        LOG_ERROR("error " << i);
    }

    for(int i = 0; i < 1000; ++i)
    {
        LOG_FATAL("fatal " << i);
    }
}

void CFormatTest()
{
    for(int i = 0; i < 5; ++i)
    {
        CLOG_TRACE(" i [%d]", i);
        CLOG_TRACE(" null");
        CLOG_TRACE(" p [%p]", &i);
        CLOG_TRACE(" u [%u]", static_cast<unsigned int>(i));
    }

    for(int i = 0; i < 5; ++i)
    {
        CLOG_DEBUG(" i [%d]", i);
        CLOG_DEBUG(" null");
        CLOG_DEBUG(" p [%p]", &i);
        CLOG_DEBUG(" u [%u]", static_cast<unsigned int>(i));
    }

    for(int i = 0; i < 5; ++i)
    {
        CLOG_INFO(" i [%d]", i);
        CLOG_INFO(" null");
        CLOG_INFO(" p [%p]", &i);
        CLOG_INFO(" u [%u]", static_cast<unsigned int>(i));
    }

    for(int i = 0; i < 5; ++i)
    {
        CLOG_WARN(" i [%d]", i);
        CLOG_WARN(" null");
        CLOG_WARN(" p [%p]", &i);
        CLOG_WARN(" u [%u]", static_cast<unsigned int>(i));
    }

    for(int i = 0; i < 5; ++i)
    {
        CLOG_ERROR(" i [%d]", i);
        CLOG_ERROR(" null");
        CLOG_ERROR(" p [%p]", &i);
        CLOG_ERROR(" u [%u]", static_cast<unsigned int>(i));
    }

    for(int i = 0; i < 5; ++i)
    {
        CLOG_FATAL(" i [%d]", i);
        CLOG_FATAL(" null");
        CLOG_FATAL(" p [%p]", &i);
        CLOG_FATAL(" u [%u]", static_cast<unsigned int>(i));
    }
}

void PerformanceTest()
{
    int COUNTS_PER_LOOP = 10000;

    Timestamp t1 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_TRACE("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t2 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_DEBUG("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t3 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_INFO("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t4 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_WARN("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t5 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_ERROR("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t6 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        LOG_FATAL("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH " << i);
    }

    Timestamp t7 = Timestamp::now();
    for(int i = 0; i < COUNTS_PER_LOOP; ++i)
    {
        printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH[%d]\n", i);
    }
    Timestamp t8 = Timestamp::now();

    printf("===============logging result [%d] counts ===========\n", COUNTS_PER_LOOP);
    printf("trace [%10lld] us\n", timeDiffInMicroSec(t2, t1));
    printf("debug [%10lld] us\n", timeDiffInMicroSec(t3, t2));
    printf("info  [%10lld] us\n", timeDiffInMicroSec(t4, t3));
    printf("warn  [%10lld] us\n", timeDiffInMicroSec(t5, t4));
    printf("error [%10lld] us\n", timeDiffInMicroSec(t6, t5));
    printf("fatal [%10lld] us\n", timeDiffInMicroSec(t7, t6));
    printf("print [%10lld] us\n", timeDiffInMicroSec(t8, t7));
}

typedef void(*PRINT_LOG)(const char*, ...);

void TracePrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::TRACE_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[TRACE_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::TRACE_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

void DebugPrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[DEBUG_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::DEBUG_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

void InfoPrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::INFO_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[INFO_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::INFO_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

void WarnPrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::WARN_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[WARN_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::WARN_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

void ErrorPrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::ERROR_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[ERROR_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::ERROR_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

void FatalPrint(const char* format, ...)
{
    IMPL_LOGGER_CLASS_TYPE_ _logger = GetLogger();
    if(_logger.isEnabledFor(log4cplus::FATAL_LOG_LEVEL))
    {
        va_list args;
        va_start(args, format);
        char buf[FATAL_SIZE] = {0};
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _logger.forcedLog(log4cplus::FATAL_LOG_LEVEL,
            LOG4CPLUS_STRING_TO_TSTRING(buf));
    }
}

int main()
{
    INIT_LOGGER("../config/log.conf");

    //CFormatTest();

    //FunctionTest();

    //PerformanceTest();

    for(int i = 0; i < 500; ++i)
    {
        TracePrint("trace %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        DebugPrint("debug %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        InfoPrint("info %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        WarnPrint("warn %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        ErrorPrint("error %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        FatalPrint("fatal %s %d %u %f", "hello", -1234 + i, 8899 + i, 23.345 + i);
        sleep(1);
    }
    return 0;
}

