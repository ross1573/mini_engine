#include <format>
#include <d3dcommon.h>
#include "assertion.h"

import mini.core;

namespace mini::detail
{

CORE_API void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[256] = {0, };
    memory::SourceLocationToString(locBuffer, sizeof(locBuffer), loc);

    LogMessage(std::format("\nEnsure failed!\n"
                           "  Expression: {0}\n"
                           "  Message: {1}\n"
                           "  Function: {2}\n\n",
                           expr, (msg == nullptr ? "" : msg), locBuffer).c_str());
}

#if DEBUG_ASSERT

constexpr int bufSize = 256;
char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };
wchar_t wassertMsg[bufSize] = {0, };
wchar_t wfuncInfo[bufSize] = {0, };

wchar_t* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "\nMessage: ", msg};
    SizeT len = memory::ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    memory::Convert(assertMsg, len, wassertMsg, memory::ConvertLength(assertMsg, len));
    return wassertMsg;
}

wchar_t* AssertLoc(std::source_location const& loc)
{
    SizeT len = memory::SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memory::Convert(funcInfo, len, wfuncInfo, memory::ConvertLength(funcInfo, len));
    return wfuncInfo;
}

void EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc)
{
    EnsureHelper(expr, reinterpret_cast<char*>(error->GetBufferPointer()), loc);
}

#endif // DEBUG_ASSERT

} // namespace mini::detail
