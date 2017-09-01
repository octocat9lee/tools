#include "logger.h"
#include "log4cplus/configurator.h"

LogManager::LogManager(const std::string& configPath)
{
    log4cplus::initialize();
    log4cplus::PropertyConfigurator::doConfigure(
        LOG4CPLUS_STRING_TO_TSTRING(configPath));
}

LogManager::~LogManager()
{
    log4cplus::Logger::shutdown();
}

