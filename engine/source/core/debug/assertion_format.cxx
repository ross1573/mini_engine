export module mini.core:assert_format;

import fmt;
import :type;
import :cmemory;
import :string_memory;
import :utility_operation;
import :source_location;

namespace mini::debug {

// note: only utf-8 string is allowed in assertion message
//       standard like strings aren't allowed
export template <typename T>
concept AssertFormatStringT = requires(T fmt) {
    { fmt.Data() } -> ConvertibleToT<char const*>;
    { fmt.Size() } -> ConvertibleToT<size_t>;
};

export struct ASSERT_API AssertFormatString {
public:
    char const* data;
    size_t size;
    static constexpr char const empty[1] = { '\0' };

public:
    constexpr AssertFormatString()
        : data(empty)
        , size(0)
    {
    }

    constexpr AssertFormatString(char const* str, size_t len)
        : data(str)
        , size(len)
    {
    }

    constexpr AssertFormatString(char const* str)
        : data(str)
        , size(memory::StringLength(str))
    {
    }
};

export struct ASSERT_API AssertFormatResult {
public:
    char const* message;
    char const* location;
};

export struct ASSERT_API AssertFormatContext {
public:
    AssertFormatString message;
    SourceLocation location;

public:
    AssertFormatContext(SourceLocation loc = SourceLocation::current())
        : message()
        , location(loc)
    {
    }

    AssertFormatContext(AssertFormatString msg, SourceLocation loc = SourceLocation::current())
        : message(msg)
        , location(loc)
    {
    }

    AssertFormatContext(char const* str, SourceLocation loc = SourceLocation::current())
        : message(str)
        , location(loc)
    {
    }

    template <AssertFormatStringT T>
    AssertFormatContext(T str, SourceLocation loc = SourceLocation::current())
        : message(static_cast<char const*>(str.Data()), static_cast<size_t>(str.Size()))
        , location(loc)
    {
    }
};

[[no_inline]] ASSERT_API constexpr size_t ConcatStrings(char* dest, size_t destLen, char const** src, size_t srcCount)
{
    if (src == nullptr || srcCount == 0) {
        return 0;
    }

    char* begin = dest;
    for (size_t i = 0; i < srcCount && destLen > 0; ++i) {
        if (src[i] == nullptr) {
            continue;
        }

        size_t len = memory::StringLength(src[i], destLen - 1);
        memory::MemCopy(dest, src[i], len);
        dest += len;
        destLen -= len;
    }

    *dest = '\0';
    return static_cast<size_t>(dest - begin);
}

ASSERT_API constexpr size_t IntegerToASCII(char* dest, size_t destLen, int src)
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

[[no_inline]] ASSERT_API constexpr size_t SourceLocationToString(char* dest, size_t destLen, SourceLocation const& loc)
{
    char* begin = dest;
    char const* funcName = loc.function_name();
    size_t funcLen = memory::StringLength(funcName, destLen - 1);

    funcLen = destLen > funcLen ? funcLen : destLen;
    memory::MemCopy(dest, funcName, funcLen);
    dest += funcLen;
    destLen -= funcLen;

    char const* fileName = loc.file_name();
    size_t fileLen = memory::StringLength(fileName, destLen - 1);
    if (destLen > fileLen + 4) {
        memory::MemCopy(dest, " (", 2);
        memory::MemCopy(dest + 2, fileName, fileLen);
        dest += fileLen + 2;
        destLen -= fileLen + 2;

        char lineBuf[16];
        size_t lineLen = IntegerToASCII(lineBuf, sizeof(lineBuf), (int)loc.line());
        if (destLen > lineLen + 2) {
            dest[0] = ':';
            memory::MemCopy(dest + 1, lineBuf, lineLen);
            dest += lineLen + 1;
        }

        dest[0] = ')';
        ++dest;
    }

    *dest = '\0';
    return static_cast<size_t>(dest - begin);
}

ASSERT_API constexpr char const assertMessageFormat[] = "\n- Message: ";
ASSERT_API constexpr char const assertMessageSuffix[] = "\n\n" ENGINE_PROJECT_NAME ":" ENGINE_PROJECT_VERSION
                                                        ":" ENGINE_BUILD_CONFIG " " ENGINE_BUILD_ARCH
                                                        ":" ENGINE_BUILD_COMPILER " ";
ASSERT_API constexpr char const assertFormat[] = "\n\nAssertion failed!\n"
                                                 "- Expression: {0}\n"
                                                 "- Function: {1}";

constexpr size_t assertSuffixSize = sizeof(assertMessageSuffix);
constexpr size_t assertBufferRawSize = 1024;
constexpr size_t assertBufferSize = NOASSERT ? 1 : assertBufferRawSize - assertSuffixSize;

[[no_inline]] ASSERT_API void LogAssert(AssertFormatResult formatResult);
[[no_inline]] ASSERT_API void LogEnsure(AssertFormatResult formatResult);

export template <typename... Args>
constexpr AssertFormatResult AssertFormat(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    thread_local char assertBuffer[assertBufferRawSize] = { 0 };
    thread_local char locationBuffer[assertBufferSize] = { 0 };
    char* buffer = assertBuffer;
    char const* bufferEnd = buffer + assertBufferSize;

    SourceLocationToString(locationBuffer, assertBufferSize, ctx.location);
    auto result = fmt::format_to_n(buffer, assertBufferSize, assertFormat, expr, locationBuffer);
    buffer += result.size;

    if (ctx.message.size == 0 || buffer >= bufferEnd) {
        memory::MemCopy(buffer, assertMessageSuffix, assertSuffixSize);
        return AssertFormatResult{ .message = assertBuffer, .location = locationBuffer };
    }

    memory::MemCopy(buffer, assertMessageFormat, sizeof(assertMessageFormat) - 1);
    buffer += sizeof(assertMessageFormat) - 1;

    auto fmtMessage = fmt::string_view(ctx.message.data, ctx.message.size);
    size_t remaining = static_cast<size_t>(bufferEnd - buffer);
    result = fmt::vformat_to_n(buffer, remaining, fmtMessage, fmt::make_format_args(args...));
    buffer += result.size;

    memory::MemCopy(buffer, assertMessageSuffix, assertSuffixSize);
    return AssertFormatResult{ .message = assertBuffer, .location = locationBuffer };
}

export template <typename... Args>
[[no_inline]] void LogAssert(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    LogAssert(AssertFormat(expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogAssert(char const* expr)
{
    LogAssert(AssertFormat(expr, AssertFormatContext()));
}

export template <typename... Args>
[[no_inline]] void LogEnsure(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    LogEnsure(AssertFormat(expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogEnsure(char const* expr)
{
    LogEnsure(AssertFormat(expr, AssertFormatContext()));
}

} // namespace mini::debug