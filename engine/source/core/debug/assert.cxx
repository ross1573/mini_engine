export module mini.core:assert;

export import :assert_expr;
export import :assert_format;

import :type;
import :utility_operation;
import :source_location;

namespace mini::debug {

[[no_inline]] ASSERT_API void LogAssert(AssertFormatResult formatResult) noexcept;
[[no_inline]] ASSERT_API void LogEnsure(AssertFormatResult formatResult) noexcept;

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

export template <typename T, typename... Args>
[[no_inline]] constexpr bool LogEvaluate(T const& arg,
                                         char const* expr,
                                         AssertFormatContext ctx,
                                         Args&&... args) noexcept
{
    if (Evaluate(arg)) [[likely]] {
        return true;
    }

    LogEnsure(FormatAssert(assertExprCategory, expr, ctx, ForwardArg<Args>(args)...));
    return false;
}

export template <typename T>
[[no_inline]] constexpr bool LogEvaluate(T const& arg,
                                         char const* expr,
                                         SourceLocation loc = SourceLocation::current()) noexcept
{
    if (Evaluate(arg)) [[likely]] {
        return true;
    }

    LogEnsure(FormatAssert(assertExprCategory, expr, nullptr, loc));
    return false;
}

export template <typename T, typename ErrorT, typename... Args>
[[no_inline]] constexpr bool LogEvaluate(T const& arg,
                                         char const* expr,
                                         ErrorT error,
                                         AssertFormatContext ctx,
                                         Args&&... args) noexcept
    requires(!ImplicitlyConvertibleToT<ErrorT, AssertFormatContext>)
{
    if (Evaluate(arg)) [[likely]] {
        return true;
    }

    LogEnsure(FormatAssert(expr, error, ctx, ForwardArg<Args>(args)...));
    return false;
}

export template <typename T, typename ErrorT, typename... Args>
[[no_inline]] constexpr bool LogEvaluate(T const& arg,
                                         char const* expr,
                                         ErrorT error,
                                         SourceLocation loc = SourceLocation::current()) noexcept
    requires(!ImplicitlyConvertibleToT<ErrorT, AssertFormatContext>)
{
    if (Evaluate(arg)) [[likely]] {
        return true;
    }

    LogEnsure(FormatAssert(expr, error, AssertFormatContext(loc)));
    return false;
}

} // namespace mini::debug