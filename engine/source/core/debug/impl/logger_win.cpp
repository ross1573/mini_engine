module;

#include "win_include.h"

module mini.core;

import :type;
import :string_memory;
import :assert_format;
import :logger_platform;

namespace mini::debug {

void LogAssert(AssertFormatResult formatResult) noexcept
{
    HWND handle = GetActiveWindow();
    MessageBoxA(handle, formatResult.message, nullptr, MB_ICONERROR | MB_OK);
    OutputDebugStringA(formatResult.message);
}

void LogEnsure(AssertFormatResult formatResult) noexcept
{
    OutputDebugStringA(formatResult.message);
}

} // namespace mini::debug

namespace mini {

LoggerBase::LoggerBase(StringView category) noexcept
    : m_category(category)
{
}

void LoggerBase::PrintMessage(byte, StringView msg) noexcept
{
    char buffer[1024]{ 0 };
    char *ptr = buffer;

    ENSURE(msg.Size() + m_category.Size() + 4 < 1024) {
        return;
    }

    *(++ptr) = '[';
    memory::MemCopy(ptr, m_category.Data(), m_category.Size());
    *(++ptr) = ']';
    *(++ptr) = ' ';
    memory::MemCopy(ptr, msg.Data(), msg.Size());
    *(++ptr) = '\n';
    *(++ptr) = '\0';

    OutputDebugStringA(ptr);
}

} // namespace mini