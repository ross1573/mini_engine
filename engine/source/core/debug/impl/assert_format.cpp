module mini.core;

import :type;
import :cmemory;
import :string_memory;
import :assert_format;
import :source_location;

namespace mini::debug {

constexpr char const assertStrTitle[] = "\n\nAssertion failed!\n";
constexpr char const assertStrFunc[] = "- function: ";
constexpr char const assertStrMsg[] = "- message: ";
constexpr char const assertSuffix[] = "\n\n" ENGINE_PROJECT_NAME ":" ENGINE_PROJECT_VERSION ":" ENGINE_BUILD_CONFIG
                                      " " ENGINE_BUILD_ARCH ":" ENGINE_BUILD_COMPILER "\n";

constexpr size_t assertBufferRawSize = 2048;
constexpr size_t assertSuffixSize = sizeof(assertSuffix);
constexpr size_t assertBufferSize = assertBufferRawSize - assertSuffixSize;

bool CopyString(char*& dest, size_t& destLen, char const* src, size_t srcLen) noexcept
{
    size_t len = destLen > srcLen ? srcLen : destLen;
    memory::MemCopy(dest, src, len);
    dest += len;
    destLen -= len;
    return destLen != 0;
}

bool Copy2Characters(char*& dest, size_t& destLen, char const src[2], char const alter[2]) noexcept
{
    switch (destLen) {
        [[unlikely]] case 2:
            *(dest++) = alter[0];
            --destLen;
            [[fallthrough]];
        [[unlikely]] case 1:
            *(dest++) = alter[1];
            --destLen;
            [[fallthrough]];
        case 0: return false;
        default:
            *(dest++) = src[0];
            *(dest++) = src[1];
            destLen -= 2;
            return true;
    }
}

extern "C++" size_t FormatHex(char* dest, char const* destEnd, uint64 src) noexcept
{
    offset_t offsetLen = destEnd - dest;
    if (offsetLen <= 0) [[unlikely]] {
        return 0;
    }

    constexpr size_t bufferLen = (sizeof(int64) << 1);
    size_t destLen = static_cast<size_t>(offsetLen);
    char buf[bufferLen] = { 0 };
    char const* begin = dest;

    size_t len = 0;
    uint64 mask = 0xF;
    for (; len < bufferLen; ++len, mask <<= 4) {
        byte n = static_cast<byte>(src & mask);
        if (n > 9) {
            buf[len] = 'A' + (n - 10);
        } else {
            buf[len] = '0' + n;
        }
    }

    char const* bufPos = buf + len;
    char const* bufEnd = buf;

    if (!Copy2Characters(dest, destLen, "0x", "  ")) {
        goto format_hex_end;
    }

    for (; bufPos > bufEnd && dest < destEnd;) {
        *(dest++) = *(--bufPos);
    }

format_hex_end:
    return static_cast<size_t>(dest - begin);
}

extern "C++" size_t FormatInt(char* dest, char const* destEnd, int32 src) noexcept
{
    size_t len = 0;
    char* begin = dest;
    char buf[10];
    for (; src > 0; ++len) {
        buf[len] = '0' + src % 10;
        src /= 10;
    }

    char* bufPos = buf + len;
    char* bufEnd = buf;
    for (; bufPos > bufEnd && dest < destEnd;) {
        *(dest++) = *(--bufPos);
    }

    return static_cast<size_t>(dest - begin);
}

extern "C++" size_t FormatSourceLocation(char* dest, char const* destEnd, SourceLocation const& loc) noexcept
{
    offset_t offsetLen = destEnd - dest;
    if (offsetLen <= 0) [[unlikely]] {
        return 0;
    }

    char const* begin = dest;
    char const* funcName;
    char const* fileName;
    size_t destLen = static_cast<size_t>(offsetLen);
    size_t funcLen;
    size_t fileLen;

    funcName = loc.function_name();
    funcLen = memory::StringLength(funcName);
    if (!CopyString(dest, destLen, funcName, funcLen)) goto format_location_end;
    if (!Copy2Characters(dest, destLen, " (", "  ")) goto format_location_end;

    fileName = loc.file_name();
    fileLen = memory::StringLength(fileName);
    if (!CopyString(dest, destLen, fileName, fileLen)) goto format_location_end;
    switch (destLen) {
        case 2: *(dest++) = ' '; [[unlikely]] [[fallthrough]];
        case 1: *(dest++) = ' '; [[unlikely]] [[fallthrough]];
        case 0: [[unlikely]] goto format_location_end;
        default:
            *(dest++) = ':';
            destLen -= 2;
            break;
    }

    dest += FormatInt(dest, destEnd, (int)loc.line());
    if (dest != destEnd) [[unlikely]] {
        *(dest++) = ')';
    }

format_location_end:
    return static_cast<size_t>(dest - begin);
}

extern "C++" size_t FormatAssertLine(char* dest, char const* destEnd, char const** src, size_t srcCount) noexcept
{
    offset_t offsetLen = destEnd - dest - 1;
    if (offsetLen <= 0 || src == nullptr || srcCount == 0) [[unlikely]] {
        return 0;
    }

    char* begin = dest;
    size_t destLen = static_cast<size_t>(offsetLen);

    for (size_t i = 0; i < srcCount && destLen > 0; ++i) {
        if (src[i] == nullptr) [[unlikely]] {
            continue;
        }

        size_t len = memory::StringLength(src[i]);
        if (!CopyString(dest, destLen, src[i], len)) [[unlikely]] {
            break;
        }
    }

    *(dest++) = '\n';
    return static_cast<size_t>(dest - begin);
}

extern "C++" size_t FormatAssertString(char* dest, char const* destEnd, char const* src) noexcept
{
    offset_t offsetLen = destEnd - dest;
    if (offsetLen <= 0 || src == nullptr) [[unlikely]] {
        return 0;
    }

    char* begin = dest;
    size_t destLen = static_cast<size_t>(offsetLen);
    size_t len = memory::StringLength(src);
    CopyString(dest, destLen, src, len);

    return static_cast<size_t>(dest - begin);
}

AssertFormatResult FormatAssert(char const* category,
                                char const* content,
                                char const* message,
                                SourceLocation location) noexcept
{
    thread_local char assertBuffer[assertBufferRawSize] = { 0 };

    size_t endLineCount = message != nullptr ? 1 : 0;
    size_t bufferLen = assertBufferSize - endLineCount;
    char* buffer = assertBuffer;
    char* bufferEnd = buffer + bufferLen;
    char const* srcBuffer[4] = { "- ", category, ": ", content };

    buffer += FormatAssertString(buffer, bufferEnd, assertStrTitle);
    if (buffer == bufferEnd) [[unlikely]] {
        goto format_result;
    }

    buffer += FormatAssertLine(buffer, bufferEnd, srcBuffer, 4);
    if (buffer == bufferEnd) [[unlikely]] {
        goto format_result;
    }

    buffer += FormatAssertString(buffer, bufferEnd, assertStrFunc);
    if (buffer == bufferEnd) [[unlikely]] {
        goto format_result;
    }

    buffer += FormatSourceLocation(buffer, bufferEnd, location);
    if (buffer == bufferEnd) [[unlikely]] {
        goto format_result;
    }

    if (message != nullptr) [[unlikely]] {
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