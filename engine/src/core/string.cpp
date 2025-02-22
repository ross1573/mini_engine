#include "mini_engine.h"
#include "core/string.h"

namespace mini::memory
{

#ifdef MSVC

SizeT ConvertLength(char const* src, SizeT srcLen)
{
    return (SizeT)MultiByteToWideChar(CP_UTF8, 0, src, (int)srcLen, NULL, 0) + 1;
}

SizeT ConvertLength(wchar_t const* src, SizeT srcLen)
{
    return (SizeT)WideCharToMultiByte(CP_UTF8, 0, src, (int)srcLen, NULL, 0, NULL, NULL) + 1;
}

void Convert(char const* src, SizeT srcLen, wchar_t* dst, SizeT dstLen)
{
    MultiByteToWideChar(CP_UTF8, 0, src, (int)srcLen, dst, (int)dstLen);
    dst[dstLen - 1] = '\0';
}

void Convert(wchar_t const* src, SizeT srcLen, char* dst, SizeT dstLen)
{
    WideCharToMultiByte(CP_UTF8, 0, src, (int)srcLen, dst, (int)dstLen, NULL, NULL);
    dst[dstLen - 1] = '\0';
}

#endif // MSVC

SizeT ConcatStrings(char* dest, SizeT destLen, char const** src, SizeT srcCount)
{
    if (src == nullptr || srcCount == 0)
    {
        return 0;
    }

    char* begin = dest;
    for (SizeT i = 0; i < srcCount && destLen > 0; ++i)
    {
        if (src[i] == nullptr)
        {
            continue;
        }

        SizeT len = (SizeT)strnlen(src[i], destLen - 1);
        memcpy(dest, src[i], len);
        dest += len;
        destLen -= len;
    }

    *dest = '\0';
    return (SizeT)(dest - begin);
}

SizeT IntegerToASCII(char* dest, SizeT destLen, SizeT src)
{
    SizeT len = 0;
    char buf[10] = {0, };
    for (; src > 0; ++len)
    {
        buf[len] = '0' + src % 10;
        src /= 10;
    }

    SizeT min = len < destLen ? len : destLen;
    for (SizeT i = 0; i < min; ++i)
    {
        dest[i] = buf[(len - 1) - i];
    }

    return len;
}

SizeT SourceLocationToString(char* dest, SizeT destLen, std::source_location const& loc)
{
    char* begin = dest;
    char const* funcName = loc.function_name();
    SizeT funcLen = (SizeT)strnlen(funcName, destLen - 1);
    memcpy(dest, funcName, funcLen);
    dest += funcLen;
    destLen -= funcLen;

    char const* fileName = loc.file_name();
    SizeT fileLen = (SizeT)strnlen(fileName, destLen - 1);
    if (destLen > fileLen + 4)
    {
        dest[0] = ' ';
        dest[1] = '(';
        memcpy(dest + 2, fileName, fileLen);
        dest += fileLen + 2;
        destLen -= fileLen + 2;

        char lineBuf[10] = {0, };
        SizeT lineLen = IntegerToASCII(lineBuf, sizeof(lineBuf), loc.line());
        if (destLen > lineLen + 2)
        {
            dest[0] = ':';
            memcpy(dest + 1, lineBuf, lineLen);
            dest += lineLen + 1;
        }

        dest[0] = ')';
    }

    *dest = '\0';
    return (SizeT)(dest - begin);
}

} // namespace mini::memory
