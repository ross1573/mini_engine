#include <string.h>
#include <source_location>

namespace mini::detail
{

int ConcatStrings(char* dest, int destLen, char const** src, int srcCount)
{
    if (src == nullptr || srcCount == 0)
    {
        return 0;
    }

    char* begin = dest;
    for (int i = 0; i < srcCount && destLen > 0; ++i)
    {
        if (src[i] == nullptr)
        {
            continue;
        }

        int len = (int)strnlen(src[i], (size_t)destLen - 1);
        memcpy(dest, src[i], (size_t)len);
        dest += len;
        destLen -= len;
    }

    *dest = '\0';
    return (int)(dest - begin);
}

int IntegerToASCII(char* dest, int destLen, int src)
{
    int len = 0;
    char buf[10] = {0, };
    for (; src > 0; ++len)
    {
        buf[len] = '0' + src % 10;
        src /= 10;
    }

    int min = len < destLen ? len : destLen;
    for (int i = 0; i < min; ++i)
    {
        dest[i] = buf[(len - 1) - i];
    }

    return len;
}

int SourceLocationToString(char* dest, int destLen, std::source_location const& loc)
{
    char* begin = dest;
    char const* funcName = loc.function_name();
    int funcLen = (int)strnlen(funcName, (size_t)destLen - 1);
    memcpy(dest, funcName, (size_t)funcLen);
    dest += funcLen;
    destLen -= funcLen;

    char const* fileName = loc.file_name();
    int fileLen = (int)strnlen(fileName, (size_t)destLen - 1);
    if (destLen > fileLen + 4)
    {
        dest[0] = ' ';
        dest[1] = '(';
        memcpy(dest + 2, fileName, (size_t)fileLen);
        dest += fileLen + 2;
        destLen -= fileLen + 2;

        char lineBuf[10] = {0, };
        int lineLen = IntegerToASCII(lineBuf, sizeof(lineBuf), (int)loc.line());
        if (destLen > lineLen + 2)
        {
            dest[0] = ':';
            memcpy(dest + 1, lineBuf, (size_t)lineLen);
            dest += lineLen + 1;
        }

        dest[0] = ')';
        ++dest;
    }

    *dest = '\0';
    return (int)(dest - begin);
}

} // namespace mini::detail
