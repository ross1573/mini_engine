module;

#include <windows.h>

module mini.core;

import :type;
import :string;
import :string_view;
import :logger_base;

namespace mini {

LoggerBase::LoggerBase(StringView category)
    : m_Category(category)
{
}

void LoggerBase::PrintMessage(byte, StringView msg)
{
    String log(3 + m_Category.Size() + msg.Size());
    log.Push('[');
    log.Append(m_Category);
    log.Append("] ", 2);
    log.Append(msg);

    OutputDebugStringA(log.Data());
}

} // namespace mini