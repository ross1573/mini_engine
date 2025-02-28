#pragma once

#include <cassert>
#include <utility>
#include <source_location>
#include "core/define.h"
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

#define ENSURE_INNER(expr, var, ...) \
    auto var = mini::detail::TestExpr(expr); \
    if (!var) [[unlikely]] mini::detail::EnsureHelper(#expr __VA_OPT__(,) __VA_ARGS__); \
    if (!var) [[unlikely]]

#define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__), __VA_ARGS__)

#else // DEBUG_ASSERT

#define BUILTIN_ASSERT(msg, func, line) ((void)0)
#define ASSERT(expr, ...) ((void)0)
#define CONSTEXPR_ASSERT(expr, ...) ((void)0)

#define VERIFY(expr, ...) \
    if (!mini::detail::TestExpr(expr)) [[unlikely]] {}
    //if (!mini::detail::TestExpr(expr)) [[unlikely]] mini::Engine::Abort(__VA_ARGS__)

#define ENSURE_INNER(expr, var, ...) \
    auto var = mini::detail::TestExpr(expr); \
    if (!var) [[unlikely]] mini::detail::EnsureHelper(#expr __VA_OPT__(,) __VA_ARGS__); \
    if (!var) [[unlikely]]

#define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__), __VA_ARGS__)

#endif // DEBUG_ASSERT

#define NEVER_CALLED(msg, ...) \
    static_assert(detail::FalseArgs<__VA_ARGS__>::value, msg); \
    std::unreachable()

namespace mini::detail
{

template <typename... Args>
struct FalseArgs { static constexpr bool value = false; };

#ifdef DEBUG_ASSERT

[[noinline]] CHAR_T* ConvertAssertMsg(char const*, char const* = nullptr);
[[noinline]] CHAR_T* ConvertAssertLoc(std::source_location const& =
                                      std::source_location::current());

#endif // DEBUG_ASSERT

[[noinline]] void EnsureHelper(char const*, char const* = nullptr,
                               std::source_location const& =
                               std::source_location::current());

inline constexpr bool TestExpr(bool arg) noexcept
{
    return arg;
}

template <typename T>
inline constexpr bool TestExpr(T const& arg) noexcept
{
    return !!(arg);
}

template <typename T>
inline constexpr bool TestExpr(T* const pointer) noexcept
{
    return pointer != nullptr;
}

} // namespace mini::detail
