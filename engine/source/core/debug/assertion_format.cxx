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

public:
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
};

export struct ASSERT_API AssertFormatContext {
public:
    AssertFormatString message;
    SourceLocation location;

public:
    AssertFormatContext(SourceLocation loc = SourceLocation::current())
        : message(nullptr, 0)
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

constexpr size_t assertBufferRawSize = 1024;

[[no_inline]] ASSERT_API AssertFormatResult AssertFormat(char const* expr, char const* msg, SourceLocation loc);
[[no_inline]] ASSERT_API void LogAssert(AssertFormatResult formatResult);
[[no_inline]] ASSERT_API void LogEnsure(AssertFormatResult formatResult);

export template <typename... Args>
constexpr AssertFormatResult AssertFormat(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    if constexpr (sizeof...(Args) == 0) {
        return AssertFormat(expr, ctx.message.data, ctx.location);
    }

    char fmtBuffer[assertBufferRawSize] = { 0 };
    auto fmtMessage = fmt::string_view(ctx.message.data, ctx.message.size);
    fmt::vformat_to_n(fmtBuffer, assertBufferRawSize, fmtMessage, fmt::make_format_args(args...));
    return AssertFormat(expr, fmtBuffer, ctx.location);
}

export template <typename... Args>
[[no_inline]] void LogAssert(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    LogAssert(AssertFormat(expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogAssert(char const* expr, SourceLocation loc = SourceLocation::current())
{
    LogAssert(AssertFormat(expr, nullptr, loc));
}

export template <typename... Args>
[[no_inline]] void LogEnsure(char const* expr, AssertFormatContext ctx, Args&&... args)
{
    LogEnsure(AssertFormat(expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogEnsure(char const* expr, SourceLocation loc = SourceLocation::current())
{
    LogEnsure(AssertFormat(expr, nullptr, loc));
}

} // namespace mini::debug