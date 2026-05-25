module;

#include <os/log.h>

module mini.core;

import :type;
import :string;
import :logger_platform;

namespace mini {

LoggerBase::LoggerBase(StringView category)
    : m_category(category)
    , m_logger(nullptr)
{
}

LoggerBase::~LoggerBase()
{
    os_release(m_logger);
}

void LoggerBase::PrintMessage(byte level, StringView msg)
{
    // delay initialization of the logger until first message has been fetched
    if (m_logger == nullptr) {
        StringView identifier = "com." ENGINE_PROJECT_AUTHOR "." ENGINE_PROJECT_NAME;
        m_logger = os_log_create(identifier.Data(), m_category.Data());
    }

    LogLevel type = GetLogType(level);
    os_log_with_type(m_logger, type, "%s", msg.Data());
}

LoggerBase::LogLevel LoggerBase::GetLogType(byte level)
{
    LogLevel type = OS_LOG_TYPE_DEFAULT;
    switch (level) {
        case 0: type = OS_LOG_TYPE_DEBUG; break;
        case 1: type = OS_LOG_TYPE_DEFAULT; break;
        case 2: type = OS_LOG_TYPE_ERROR; break;
        case 3:
        case 4: type = OS_LOG_TYPE_FAULT; break;
    }

    return type;
}

} // namespace mini