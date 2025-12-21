module;

#include <windows.h>

module mini.debug;

import mini.type;
import mini.string;
import :base;

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