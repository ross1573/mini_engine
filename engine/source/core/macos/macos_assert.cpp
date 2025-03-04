#include "assertion.h"

import mini.core;
import mini.log;

namespace mini::detail
{

#if DEBUG_ASSERT

constexpr int bufSize = 256;
char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };

char* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "', message: '", msg};
    memory::ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

char* AssertLoc(std::source_location const& loc)
{
    SizeT len = memory::SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memcpy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

#endif // DEBUG_ASSERT

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[256] = {0, };
    memory::SourceLocationToString(locBuffer, sizeof(locBuffer), loc);

    LogMessage(Format("\nEnsure failed!\n"
                      "  Expression: {0}\n"
                      "  Message: {1}\n"
                      "  Function: {2}\n\n",
                      expr, (msg == nullptr ? "" : msg), locBuffer));
}

} // namespace mini::detail
