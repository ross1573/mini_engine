module;

#include <windows.h>

module mini.core;

import :type;
import :string;
import :logger_platform;

namespace mini {

LoggerBase::LoggerBase(StringView category)
    : m_Category(category)
{
}

void LoggerBase::PrintMessage(byte, StringView msg)
{
    String log(4 + m_Category.Size() + msg.Size());
    log.Push('[');
    log.Append(m_Category);
    log.Append("] ", 2);
    log.Append(msg);
    log.Push('\n');

    OutputDebugStringA(log.Data());
}

} // namespace mini