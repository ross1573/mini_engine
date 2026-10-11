export module mini.core:assert;

export import :assert_expr;
export import :assert_format;

import :type;
import :utility_operation;
import :source_location;

namespace mini::debug {

[[no_inline]] ASSERT_API void logAssert(AssertFormatResult formatResult) noexcept;
[[no_inline]] ASSERT_API void logEnsure(AssertFormatResult formatResult) noexcept;

export template <typename... Args>
[[no_inline]] void logAssert(char const* expr, AssertFormatContext ctx, Args&&... args) noexcept
{
    logAssert(formatAssert(assertExprCategory, expr, ctx, forwardCast<Args>(args)...));
}

export [[no_inline]] ASSERT_API void logAssert(char const* expr,
                                               SourceLocation loc = SourceLocation::current()) noexcept
{
    logAssert(formatAssert(assertExprCategory, expr, nullptr, loc));
}

export template <typename T, typename... Args>
[[no_inline]] void logAssert(char const* expr, T error, AssertFormatContext ctx, Args&&... args) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    logAssert(formatAssert(expr, error, ctx, forwardCast<Args>(args)...));
}

export template <typename T>
[[no_inline]] void logAssert(char const* expr, T error, SourceLocation loc = SourceLocation::current()) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    logAssert(formatAssert(expr, error, AssertFormatContext(loc)));
}

export template <typename... Args>
[[no_inline]] void logEnsure(char const* expr, AssertFormatContext ctx, Args&&... args) noexcept
{
    logEnsure(formatAssert(assertExprCategory, expr, ctx, forwardCast<Args>(args)...));
}

export [[no_inline]] ASSERT_API void logEnsure(char const* expr,
                                               SourceLocation loc = SourceLocation::current()) noexcept
{
    logEnsure(formatAssert(assertExprCategory, expr, nullptr, loc));
}

export template <typename T, typename... Args>
[[no_inline]] void logEnsure(char const* expr, T error, AssertFormatContext ctx, Args&&... args) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    logEnsure(formatAssert(expr, error, ctx, forwardCast<Args>(args)...));
}

export template <typename T>
[[no_inline]] void logEnsure(char const* expr, T error, SourceLocation loc = SourceLocation::current()) noexcept
    requires(!ImplicitlyConvertibleToT<T, AssertFormatContext>)
{
    logEnsure(formatAssert(expr, error, AssertFormatContext(loc)));
}

} // namespace mini::debug