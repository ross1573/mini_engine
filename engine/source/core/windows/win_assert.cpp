#include <d3dcommon.h>

#include "assert_util.h"

import mini.core;
import convert_utf;

constexpr int bufSize = DEBUG * 1023 + 1;

char assertMsg[bufSize];
char funcInfo[bufSize];
wchar_t wassertMsg[bufSize];
wchar_t wfuncInfo[bufSize];

namespace mini::detail {

wchar_t* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = { expr, msg == nullptr ? nullptr : "\nMessage: ", msg };

    int len = (int)ConcatStrings(assertMsg, bufSize, str, 3);
    int cvtLen = (int)ConvertLength(assertMsg, assertMsg + len, (wchar_t)0);
    int wlen = cvtLen < bufSize ? cvtLen : bufSize;

    Convert(assertMsg, assertMsg + len, wassertMsg, wassertMsg + wlen);
    wassertMsg[wlen - 1] = '\0';

    return wassertMsg;
}

wchar_t* AssertLoc(std::source_location const& loc)
{
    int len = (int)SourceLocationToString(funcInfo, bufSize, loc);
    int cvtLen = (int)ConvertLength(funcInfo, funcInfo + len, (wchar_t)0);
    int wlen = cvtLen < bufSize ? cvtLen : bufSize;

    Convert(funcInfo, funcInfo + len, wfuncInfo, wfuncInfo + wlen);
    wfuncInfo[wlen - 1] = '\0';
    return wfuncInfo;
}

void EnsureHelper(char const* expr, char const* msg, std::source_location const& loc)
{
    char locBuffer[512];
    int len = SourceLocationToString(locBuffer, sizeof(locBuffer), loc);
    locBuffer[len - 1] = '\0';

    log::Message(StringView{},
                 "\nEnsure failed!\n"
                 "  Expression: {0}\n"
                 "  Message: {1}\n"
                 "  Function: {2}\n\n",
                 expr, msg, locBuffer);
}

void EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc)
{
    EnsureHelper(expr, reinterpret_cast<char*>(error->GetBufferPointer()), loc);
}

} // namespace mini::detail
