#include <d3dcommon.h>
#include "assertion.h"

import mini.core;

#if DEBUG_ASSERT
constexpr int bufSize = 256;
char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };
#if PLATFORM_WINDOWS
wchar_t wassertMsg[bufSize] = {0, };
wchar_t wfuncInfo[bufSize] = {0, };
#endif // PLATFORM_WINDOWS
#endif // DEBUG_ASSERT

namespace mini::detail
{

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[256] = {0, };
    memory::SourceLocationToString(locBuffer, sizeof(locBuffer), loc);

    Print(Format("\nEnsure failed!\n"
                 "  Expression: {0}\n"
                 "  Message: {1}\n"
                 "  Function: {2}\n\n",
                 expr, (msg == nullptr ? "" : msg), locBuffer));
}

#if DEBUG_ASSERT
#if PLATFORM_WINDOWS

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

#else

CORE_API char* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "', message: '", msg};
    memory::ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

CORE_API char* AssertLoc(std::source_location const& loc)
{
    SizeT len = memory::SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memcpy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

#endif // PLATFORM_WINDOWS
#endif // DEBUG_ASSERT

} // namespace mini::detail
