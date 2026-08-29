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
    constexpr AssertFormatString(char const* str, size_t len) noexcept
        : data(str)
        , size(len)
    {
    }

    constexpr AssertFormatString(char const* str) noexcept
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
    explicit constexpr AssertFormatContext(SourceLocation loc = SourceLocation::current()) noexcept
        : message(nullptr, 0)
        , location(loc)
    {
    }

    constexpr AssertFormatContext(AssertFormatString msg, SourceLocation loc = SourceLocation::current()) noexcept
        : message(msg)
        , location(loc)
    {
    }

    constexpr AssertFormatContext(char const* str, SourceLocation loc = SourceLocation::current()) noexcept
        : message(str)
        , location(loc)
    {
    }

    template <AssertFormatStringT T>
    constexpr AssertFormatContext(T str, SourceLocation loc = SourceLocation::current()) noexcept
        : message(static_cast<char const*>(str.Data()), static_cast<size_t>(str.Size()))
        , location(loc)
    {
    }
};

constexpr size_t assertFormatMaxLength = 1024;
ASSERT_API char const* assertExprCategory = "expression";

extern "C++" { // format function which can be used on other modules
ASSERT_API size_t FormatHex(char* dest, char const* destEnd, uint64 src) noexcept;
ASSERT_API size_t FormatInt(char* dest, char const* destEnd, int32 src) noexcept;
ASSERT_API size_t FormatSourceLocation(char* dest, char const* destEnd, SourceLocation const& loc) noexcept;
ASSERT_API size_t FormatAssertLine(char* dest, char const* destEnd, char const** src, size_t count) noexcept;
ASSERT_API size_t FormatAssertString(char* dest, char const* destEnd, char const* src) noexcept;
}

[[no_inline]] ASSERT_API void LogAssert(AssertFormatResult formatResult) noexcept;
[[no_inline]] ASSERT_API void LogEnsure(AssertFormatResult formatResult) noexcept;
[[no_inline]] ASSERT_API AssertFormatResult FormatAssert(char const* category,
                                                         char const* content,
                                                         char const* message,
                                                         SourceLocation location) noexcept;

export template <typename... Args>
AssertFormatResult FormatAssert(char const* category,
                                char const* content,
                                AssertFormatContext const& ctx,
                                Args&&... args) noexcept
{
    if constexpr (sizeof...(Args) == 0) {
        return FormatAssert(category, content, ctx.message.data, ctx.location);
    }

    thread_local char fmtBuffer[assertFormatMaxLength] = { 0 };
    auto fmtMessage = fmt::string_view(ctx.message.data, ctx.message.size);
    try {
        fmt::vformat_to_n(fmtBuffer, assertFormatMaxLength, fmtMessage, fmt::make_format_args(args...));
    } catch (fmt::format_error const& error) {
        char const* fmtError[] = { "format failed with error: ", error.what() };
        FormatAssertLine(fmtBuffer, fmtBuffer + assertFormatMaxLength, fmtError, sizeof(fmtError));
    }

    return FormatAssert(category, content, fmtBuffer, ctx.location);
}

export template <typename... Args>
[[no_inline]] void LogAssert(char const* expr, AssertFormatContext ctx, Args&&... args) noexcept
{
    LogAssert(FormatAssert(assertExprCategory, expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogAssert(char const* expr,
                                               SourceLocation loc = SourceLocation::current()) noexcept
{
    LogAssert(FormatAssert(assertExprCategory, expr, nullptr, loc));
}

export template <typename T, typename... Args>
[[no_inline]] void LogAssert(char const* expr, T error, AssertFormatContext ctx, Args&&... args) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    LogAssert(FormatAssert(expr, error, ctx, ForwardArg<Args>(args)...));
}

export template <typename T>
[[no_inline]] void LogAssert(char const* expr, T error, SourceLocation loc = SourceLocation::current()) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    LogAssert(FormatAssert(expr, error, AssertFormatContext(loc)));
}

export template <typename... Args>
[[no_inline]] void LogEnsure(char const* expr, AssertFormatContext ctx, Args&&... args) noexcept
{
    LogEnsure(FormatAssert(assertExprCategory, expr, ctx, ForwardArg<Args>(args)...));
}

export [[no_inline]] ASSERT_API void LogEnsure(char const* expr,
                                               SourceLocation loc = SourceLocation::current()) noexcept
{
    LogEnsure(FormatAssert(assertExprCategory, expr, nullptr, loc));
}

export template <typename T, typename... Args>
[[no_inline]] void LogEnsure(char const* expr, T error, AssertFormatContext ctx, Args&&... args) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    LogEnsure(FormatAssert(expr, error, ctx, ForwardArg<Args>(args)...));
}

export template <typename T>
[[no_inline]] void LogEnsure(char const* expr, T error, SourceLocation loc = SourceLocation::current()) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    LogEnsure(FormatAssert(expr, error, AssertFormatContext(loc)));
}

} // namespace mini::debug