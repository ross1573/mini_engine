module;

#include "core/assert.h"
#include "windows/assert.h"

module mini.core:log;

import mini.windows;

void LogMessage(char const* msg, bool endLine)
{
    OutputDebugStringW(mini::windows::StringConverter<char, wchar_t>(msg).Data());

    if (endLine)
    {
        OutputDebugStringW(L"\n");
    }
}

namespace mini::detail
{

void EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc)
{
    wchar_t* buffer = reinterpret_cast<wchar_t*>(error->GetBufferPointer());
    EnsureHelper(expr, windows::StringConverter<wchar_t, char>(buffer).Data(), loc);
}

}