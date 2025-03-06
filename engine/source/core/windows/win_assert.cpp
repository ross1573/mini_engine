#include <format>
#include <d3dcommon.h>

import mini.core;
import convert_utf;

constexpr int bufSize = DEBUG * 1023 + 1;

char assertMsg[bufSize] = {0, };
char funcInfo[bufSize] = {0, };

wchar_t wassertMsg[bufSize] = {0, };
wchar_t wfuncInfo[bufSize] = {0, };

namespace mini::detail
{

wchar_t* AssertMsg(char const* expr, char const* msg)
{
    char const* str[3] = {expr, msg == nullptr ? nullptr : "\nMessage: ", msg};

    SizeT len = ConcatStrings(assertMsg, bufSize, str, 3);
    SizeT cvtLen = (SizeT)ConvertLength(assertMsg, assertMsg + len, (wchar_t)0);
    SizeT wlen = cvtLen < bufSize ? cvtLen : bufSize;

    Convert(assertMsg, assertMsg + len, wassertMsg, wassertMsg + wlen);
    wassertMsg[wlen - 1] = '\0';

    return wassertMsg;
}

wchar_t* AssertLoc(std::source_location const& loc)
{
    SizeT len = SourceLocationToString(funcInfo, bufSize, loc);
    SizeT cvtLen = (SizeT)ConvertLength(funcInfo, funcInfo + len, (wchar_t)0);
    SizeT wlen = cvtLen < bufSize ? cvtLen : bufSize;

    Convert(funcInfo, funcInfo + len, wfuncInfo, wfuncInfo + wlen);
    wfuncInfo[wlen - 1] = '\0';
    return wfuncInfo;
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

void EnsureHelper(char const* expr, ID3DBlob* error, std::source_location const& loc)
{
    EnsureHelper(expr, reinterpret_cast<char*>(error->GetBufferPointer()), loc);
}

} // namespace mini::detail
