#include "mini_engine.h"
#include "platform/windows/utility.h"

namespace mini
{

void Print(StringView const msg)
{
    OutputDebugStringW(windows::StringConverter<char, wchar_t>(msg).Data());
}

void PrintLine(StringView const msg)
{
    if (msg.size() != 0)
    {
        OutputDebugStringW(windows::StringConverter<char, wchar_t>(msg).Data());
    }
    OutputDebugStringW(L"\n");
}

} // namespace mini