#include "mini_engine.h"
#include "platform/windows/string_convert.h"

namespace mini::detail
{

SizeT ConvertLength(char const* src, int srcLen)
{
    return (SizeT)MultiByteToWideChar(CP_UTF8, 0, src, srcLen, NULL, 0) + 1;
}

SizeT ConvertLength(wchar_t const* src, int srcLen)
{
    return (SizeT)WideCharToMultiByte(CP_UTF8, 0, src, srcLen, NULL, 0, NULL, NULL) + 1;
}

void Convert(char const* src, int srcLen, wchar_t* dst, SizeT dstLen)
{
    MultiByteToWideChar(CP_UTF8, 0, src, srcLen, dst, dstLen);
    dst[dstLen - 1] = '\0';
}

void Convert(wchar_t const* src, int srcLen, char* dst, SizeT dstLen)
{
    WideCharToMultiByte(CP_UTF8, 0, src, srcLen, dst, dstLen, NULL, NULL);
    dst[dstLen - 1] = '\0';
}

} // namespace mini::detail