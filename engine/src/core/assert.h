#pragma once

#include "core/type.h"
#include "core/macro.h"

#if defined(DEBUG) && !defined(NOASSERT)
#   ifndef DEBUG_ASSERT
#   define DEBUG_ASSERT
#   endif
#endif // DEBUG && !NOASSERT

#ifdef DEBUG_ASSERT

#if defined(MSVC)
#   define BUILTIN_ASSERT(msg, func, line) _wassert(msg, func, line)
#elif defined(CLANG) || defined(GNUC)
#   define BUILTIN_ASSERT(msg, func, line) __assert(msg, func, line)
#endif

#define ASSERT(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] \
    { \
        BUILTIN_ASSERT( \
            mini::detail::ConvertAssertMsg(#expr __VA_OPT__(,) __VA_ARGS__), \
            mini::detail::ConvertAssertLoc(), __LINE__); \
        std::unreachable(); \
    }

#define CONSTEXPR_ASSERT(expr, ...) \
    if (!std::is_constant_evaluated()) ASSERT(expr, __VA_ARGS__)

#define VERIFY(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] \
    { \
        BUILTIN_ASSERT( \
            mini::detail::ConvertAssertMsg(#expr __VA_OPT__(,) __VA_ARGS__), \
            mini::detail::ConvertAssertLoc(), __LINE__); \
        std::unreachable(); \
    }

#define ENSURE(expr, ...) \
    if (!mini::detail::TestExpr(expr) && \
        mini::detail::EnsureHelper(#expr __VA_OPT__(,) __VA_ARGS__)) [[unlikely]]

#else // DEBUG_ASSERT

#define BUILTIN_ASSERT(msg, func, line) ((void)0)
#define ASSERT(expr, ...) ((void)0)
#define CONSTEXPR_ASSERT(expr, ...) ((void)0)

#define VERIFY(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] mini::Engine::Abort(__VA_ARGS__)

#define ENSURE(expr, ...) \
    if (!mini::detail::TestExpr(expr) && \
        mini::detail::EnsureHelper(#expr __VA_OPT__(,) __VA_ARGS__)) [[unlikely]]

#endif // DEBUG_ASSERT

namespace mini::detail
{

template <typename... Args>
struct FalseArgs : FalseT {};

#ifdef DEBUG_ASSERT

CHAR_T* ConvertAssertMsg(char const*, char const* = nullptr);
CHAR_T* ConvertAssertLoc(std::source_location const& = std::source_location::current());

#endif // DEBUG_ASSERT

bool EnsureHelper(char const*, char const* = nullptr,
                  std::source_location const& = std::source_location::current());

[[force_inline]] constexpr bool TestExpr(bool arg) noexcept
{
    return arg;
}

template <typename T>
[[force_inline]] constexpr bool TestExpr(T const& arg) noexcept
{
    return !!(arg);
}

template <typename T>
[[force_inline]] constexpr bool TestExpr(T* const pointer) noexcept
{
    return pointer != nullptr;
}

} // namespace mini::detail
