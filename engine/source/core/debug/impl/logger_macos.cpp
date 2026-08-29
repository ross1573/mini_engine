module;

#include <os/log.h>

module mini.core;

import :type;
import :string;
import :assert_format;
import :logger_platform;

namespace mini::debug {

Logger assertLogger = Logger("Assert");

void LogAssert(AssertFormatResult formatResult) noexcept
{
    assertLogger.Fatal(formatResult.message);
}

void LogEnsure(AssertFormatResult formatResult) noexcept
{
    assertLogger.Fatal(formatResult.message);
}

} // namespace mini::debug

namespace mini {

LoggerBase::LoggerBase(StringView category) noexcept
    : m_category(category)
    , m_logger(nullptr)
{
}

LoggerBase::~LoggerBase() noexcept
{
    os_release(m_logger);
}

void LoggerBase::PrintMessage(byte level, StringView msg) noexcept
{
    // delay initialization of the logger until first message has been fetched
    if (m_logger == nullptr) {
        StringView identifier = "com." ENGINE_PROJECT_AUTHOR "." ENGINE_PROJECT_NAME;
        m_logger = os_log_create(identifier.Data(), m_category.Data());
    }

    LogLevel type = GetLogType(level);
    os_log_with_type(m_logger, type, "%s", msg.Data());
}

LoggerBase::LogLevel LoggerBase::GetLogType(byte level) noexcept
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