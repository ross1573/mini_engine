#include "mini_engine.h"
#include "core/assert.h"
#include "core/string.h"

namespace mini::detail
{

#ifdef DEBUG_ASSERT

constexpr int bufSize = 256;

char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };

#ifdef MSVC

wchar_t wassertMsg[bufSize] = {0, };
wchar_t wfuncInfo[bufSize] = {0, };

wchar_t* ConvertAssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = { expr,  msg == nullptr ? nullptr : "\nMessage: ", msg };
    SizeT len = memory::ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    memory::Convert(assertMsg, len, wassertMsg, memory::ConvertLength(assertMsg, len));
    return wassertMsg;
}

wchar_t* ConvertAssertLoc(std::source_location const& loc)
{
    SizeT len = memory::SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memory::Convert(funcInfo, len, wfuncInfo, memory::ConvertLength(funcInfo, len));
    return wfuncInfo;
}

#else // MSVC

char* ConvertAssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "\nMessage: ", msg};
    memory::ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

char* ConvertAssertLoc(std::source_location const& loc)
{
    memory::SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    return funcInfo;
}

#endif // MSVC

#endif // DEBUG_ASSERT

} // namespace mini::detail