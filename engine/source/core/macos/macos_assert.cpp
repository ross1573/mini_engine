#include <cstring>

#include "assertion.h"

import mini.core;

constexpr int bufSize = DEBUG * 1023 + 1;

char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };

namespace mini::detail
{

char* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "', message: '", msg};
    ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

char* AssertLoc(std::source_location const& loc)
{
    int len = SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memcpy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[512] = {0, };
    SourceLocationToString(locBuffer, sizeof(locBuffer), loc);
    LogMessage(Format("\nEnsure failed!\n"
                      "  Expression: {0}\n"
                      "  Message: {1}\n"
                      "  Function: {2}\n\n",
                      expr, (msg == nullptr ? "" : msg), locBuffer).c_str());
}

} // namespace mini::detail
