module mini.core;

import :type;
import :cmemory;
import :string_memory;
import :assert_format;
import :source_location;

namespace mini::debug {

constexpr char const assertStrTitle[] = "\n\nAssertion failed!\n";
constexpr char const assertStrExpr[] = "- Expression: ";
constexpr char const assertStrFunc[] = "- Function: ";
constexpr char const assertStrMsg[] = "- Message: ";

constexpr char const assertSuffix[] = "\n\n" ENGINE_PROJECT_NAME ":" ENGINE_PROJECT_VERSION ":" ENGINE_BUILD_CONFIG
                                      " " ENGINE_BUILD_ARCH ":" ENGINE_BUILD_COMPILER "\n";

constexpr size_t assertSuffixSize = sizeof(assertSuffix);
constexpr size_t assertBufferSize = NOASSERT ? 1 : assertBufferRawSize - assertSuffixSize;
constexpr size_t assertRequiredSize = sizeof(assertStrTitle) + sizeof(assertStrExpr) + sizeof(assertStrFunc) +
                                      sizeof(assertStrMsg) + sizeof(assertSuffix);

static_assert(assertBufferRawSize > assertRequiredSize);

size_t FormatASCII(char* dest, size_t destLen, int src)
{
    size_t len = 0;
    char buf[10];
    for (; src > 0; ++len) {
        buf[len] = '0' + src % 10;
        src /= 10;
    }

    size_t min = len < destLen ? len : destLen;
    for (size_t i = 0; i < min; ++i) {
        dest[i] = buf[(len - 1) - i];
    }

    return len;
}

size_t FormatSourceLocation(char* dest, char const* destEnd, SourceLocation const& loc)
{
    char* begin = dest;
    size_t destLen = static_cast<size_t>(destEnd - dest);
    if (dest == destEnd) {
        return 0;
    }

    char const* funcName = loc.function_name();
    size_t funcLen = memory::StringLength(funcName);
    funcLen = destLen > funcLen ? funcLen : destLen;

    memory::MemCopy(dest, funcName, funcLen);
    dest += funcLen;
    destLen -= funcLen;

    char const* fileName = loc.file_name();
    size_t fileLen = memory::StringLength(fileName);
    if (destLen > fileLen + 4) {
        memory::MemCopy(dest, " (", 2);
        memory::MemCopy(dest + 2, fileName, fileLen);
        dest += fileLen + 2;
        destLen -= fileLen + 2;

        char lineBuf[16];
        size_t lineLen = FormatASCII(lineBuf, sizeof(lineBuf), (int)loc.line());
        if (destLen > lineLen + 2) {
            dest[0] = ':';
            memory::MemCopy(dest + 1, lineBuf, lineLen);
            dest += lineLen + 1;
        }

        dest[0] = ')';
        ++dest;
    }

    return static_cast<size_t>(dest - begin);
}

size_t FormatAssertLine(char* dest, char const* destEnd, char const** src, size_t srcCount)
{
    size_t destLen = static_cast<size_t>(destEnd - dest) - 1;
    if (dest == destEnd || src == nullptr || srcCount == 0) {
        return 0;
    }

    char* begin = dest;
    for (size_t i = 0; i < srcCount && destLen > 0; ++i) {
        if (src[i] == nullptr) {
            continue;
        }

        size_t len = memory::StringLength(src[i]);
        len = destLen > len ? len : destLen;

        memory::MemCopy(dest, src[i], len);
        dest += len;
        destLen -= len;
    }

    *(dest++) = '\n';
    return static_cast<size_t>(dest - begin);
}

size_t FormatAssertString(char* dest, char const* destEnd, char const* src)
{
    size_t destLen = static_cast<size_t>(destEnd - dest);
    if (dest == destEnd || src == nullptr) {
        return 0;
    }

    char* begin = dest;
    size_t len = memory::StringLength(src);
    len = destLen > len ? len : destLen;

    memory::MemCopy(dest, src, len);
    dest += len;
    destLen -= len;

    return static_cast<size_t>(dest - begin);
}

AssertFormatResult AssertFormat(char const* expr, char const* message, SourceLocation location)
{
    thread_local char assertBuffer[assertBufferRawSize] = { 0 };

    size_t endLineCount = message != nullptr ? 1 : 0;
    char* buffer = assertBuffer;
    char* bufferEnd = buffer + assertBufferSize - endLineCount;
    char const* srcBuffer[2];

    buffer += FormatAssertString(buffer, bufferEnd, assertStrTitle);
    if (buffer == bufferEnd) {
        goto format_result;
    }

    srcBuffer[0] = assertStrExpr;
    srcBuffer[1] = expr;
    buffer += FormatAssertLine(buffer, bufferEnd, srcBuffer, 2);
    if (buffer == bufferEnd) {
        goto format_result;
    }

    buffer += FormatAssertString(buffer, bufferEnd, assertStrFunc);
    if (buffer == bufferEnd) {
        goto format_result;
    }

    buffer += FormatSourceLocation(buffer, bufferEnd, location);
    if (buffer == bufferEnd) {
        goto format_result;
    }

    if (message != nullptr) {
        *(buffer++) = '\n';
        ++bufferEnd;
        buffer += FormatAssertString(buffer, bufferEnd, assertStrMsg);
        buffer += FormatAssertString(buffer, bufferEnd, message);
    }

format_result:
    memory::MemCopy(buffer, assertSuffix, assertSuffixSize);
    return AssertFormatResult{ .message = assertBuffer };
}

} // namespace mini::debug