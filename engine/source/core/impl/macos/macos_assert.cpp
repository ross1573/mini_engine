#include "impl/assert_util.h"

import mini.core;

constexpr int bufSize = DEBUG * 1023 + 1;

char assertMsg[bufSize] = { 0 };
char funcInfo[bufSize] = { 0 };

namespace mini::detail {

char* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = { expr, msg == nullptr ? nullptr : "', message: '", msg };
    ConcatStrings(assertMsg, sizeof(assertMsg), str, 3);
    return assertMsg;
}

char* AssertLoc(std::source_location const& loc)
{
    int32 len = SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memory::MemCopy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    static Logger assertLogger = Logger("Assert");

    char locBuffer[512];
    int32 len = SourceLocationToString(locBuffer, sizeof(locBuffer) - 2, loc);
    memory::MemCopy(locBuffer + len, "\n\n\0", 3);

    StringView exprView = expr;
    StringView msgView = msg;

    if (msgView.IsEmpty()) {
        assertLogger.Fatal("\n\nEnsure failed!\n"
                           "  Expression: {0}\n"
                           "  Function: {1}",
                           exprView, locBuffer);
    }
    else {
        assertLogger.Fatal("\n\nEnsure failed!\n"
                           "  Expression: {0}\n"
                           "  Message: {1}\n"
                           "  Function: {2}",
                           exprView, msgView, locBuffer);
    }
}

} // namespace mini::detail
