#include "mini_engine.h"
#include "platform/windows/utility.h"

namespace mini::detail
{

void LogMessage(char const* msg, bool endLine)
{
    OutputDebugStringW(windows::StringConverter<char, wchar_t>(msg).Data());

    if (endLine)
    {
        OutputDebugStringW(L"\n");
    }
}

bool EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc)
{
    wchar_t* buffer = reinterpret_cast<wchar_t*>(error->GetBufferPointer());
    return EnsureHelper(expr, windows::StringConverter<wchar_t, char>(buffer).Data(), loc);
}

} // namespace mini::detail