module;

#include "win_include.h"

module mini.core;

import :type;
import :string_memory;
import :assert_format;
import :logger_platform;

namespace mini::debug {

void logAssert(AssertFormatResult formatResult) noexcept
{
    HWND handle = GetActiveWindow();
    MessageBoxA(handle, formatResult.message, nullptr, MB_ICONERROR | MB_OK);
    OutputDebugStringA(formatResult.message);
}

void logEnsure(AssertFormatResult formatResult) noexcept
{
    OutputDebugStringA(formatResult.message);
}

} // namespace mini::debug

namespace mini {

LoggerBase::LoggerBase(StringView category) noexcept
    : m_category(category)
{
}

void LoggerBase::printMessage(byte, StringView msg) noexcept
{
    char buffer[1024]{0};
    char *ptr = buffer;

    ENSURE(msg.size() + m_category.size() + 4 < 1024) {
        return;
    }

    *(ptr++) = '[';
    memory::memcopy(ptr, m_category.data(), m_category.size());
    ptr += m_category.size();
    *(ptr++) = ']';
    *(ptr++) = ' ';
    memory::memcopy(ptr, msg.data(), msg.size());
    ptr += msg.size();
    *(ptr++) = '\n';
    *(ptr++) = '\0';

    OutputDebugStringA(buffer);
}

} // namespace mini