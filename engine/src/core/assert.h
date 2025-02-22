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
#   define BUILTIN_ASSERT(msg, func, line) _wassert(msg, func, line);
#elif defined(CLANG) || defined(GNU)
#   define BUILTIN_ASSERT(msg, func, line) __assert(msg, func, line);
#endif

#define ASSERT(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] \
    { \
        BUILTIN_ASSERT( \
            mini::detail::ConvertAssertMsg(#expr __VA_OPT__(, ) __VA_ARGS__), \
            mini::detail::ConvertAssertLoc(), __LINE__); \
        std::unreachable(); \
    }

#define VERIFY(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] \
    { \
        BUILTIN_ASSERT( \
            mini::detail::ConvertAssertMsg(#expr __VA_OPT__(, ) __VA_ARGS__), \
            mini::detail::ConvertAssertLoc(), __LINE__); \
        std::unreachable(); \
    }

#define ENSURE(...) \
    if (!mini::detail::TestExpr(__VA_ARGS__)) [[unlikely]]

#define CONSTEXPR_ASSERT(expr, ...) \
    if (!std::is_constant_evaluated()) ASSERT(expr, __VA_ARGS__)

#else // DEBUG_ASSERT

#define ASSERT(expr, ...) ((void)0)
#define VERIFY(expr, ...) if (!mini::detail::TestExpr(expr)) [[unlikely]] mini::Engine::Abort(__VA_ARGS__);
#define ENSURE(...) if (!mini::detail::TestExpr(__VA_ARGS__)) [[unlikely]]
#define CONSTEXPR_ASSERT(expr, ...) ((void)0)

#endif // DEBUG_ASSERT

namespace mini::detail
{

template <typename... Args>
struct FalseArgs : FalseT {};

#ifdef DEBUG_ASSERT

PLATFORM_CHAR* ConvertAssertMsg(char const*, char const* = nullptr);
PLATFORM_CHAR* ConvertAssertLoc(std::source_location const& = std::source_location::current());

#endif // DEBUG_ASSERT

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