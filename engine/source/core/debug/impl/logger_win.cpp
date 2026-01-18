module;

#include "internal/min_windows.h"

module mini.core;

import :type;
import :string;
import :logger_platform;

namespace mini {

LoggerBase::LoggerBase(StringView category)
    : m_category(category)
{
}

void LoggerBase::PrintMessage(byte, StringView msg)
{
    String log(4 + m_category.Size() + msg.Size());
    log.Push('[');
    log.Append(m_category);
    log.Append("] ", 2);
    log.Append(msg);
    log.Push('\n');

    OutputDebugStringA(log.Data());
}

} // namespace mini