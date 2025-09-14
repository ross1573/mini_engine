#include "assert_util.h"

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
    int len = SourceLocationToString(funcInfo, sizeof(funcInfo), loc);
    memory::MemCopy(funcInfo + len, "\n\0", 3);
    return funcInfo;
}

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[512];
    int len = SourceLocationToString(locBuffer, sizeof(locBuffer) - 2, loc);
    memory::MemCopy(locBuffer + len, "\n\n\0", 3);

    StringView exprView = expr;
    StringView msgView = msg;

    if (msgView.IsEmpty()) {
        log::Message(StringView{},
                     "\nEnsure failed!\n"
                     "  Expression: {0}\n"
                     "  Function: {2}",
                     exprView, locBuffer);
    }
    else {
        log::Message(StringView{},
                     "\nEnsure failed!\n"
                     "  Expression: {0}\n"
                     "  Message: {1}\n"
                     "  Function: {2}",
                     exprView, msgView, locBuffer);
    }
}

} // namespace mini::detail
