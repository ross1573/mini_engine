#pragma once

#include <cassert>
#include <source_location>

#define CONCAT_INNER(x, y) x ## y
#define CONCAT(x, y) CONCAT_INNER(x, y)

#if DEBUG && !defined(NOASSERT)
#   define DEBUG_ASSERT 1
#else
#   define DEBUG_ASSERT 0
#endif // DEBUG && !NOASSERT

#if MSVC
#   define BUILTIN_UNREACHABLE __assume(false);
#   define BUILTIN_CONSTANT_EVAL __builtin_is_constant_evaluated()
#else
#   define BUILTIN_UNREACHABLE __builtin_unreachable();
#   define BUILTIN_CONSTANT_EVAL __builtin_is_constant_evaluated()
#endif // MSVC

#define _MD mini::detail
#define ASSERT_EXPR(expr) if (!_MD::TestExpr(expr)) [[unlikely]]
#define ENSURE_EXPR(expr) if (!expr) [[unlikely]]
#define ENSURE_EVAL(expr, var) const bool var = _MD::TestExpr(expr);
#define ENSURE_LOG(expr, ...) _MD::EnsureHelper(#expr __VA_OPT__(,) __VA_ARGS__);

#if DEBUG_ASSERT
#   if MSVC
#       define BUILTIN_ASSERT(msg, func, line) _wassert(msg, func, line)
#   else
#       define BUILTIN_ASSERT(msg, func, line) __assert(msg, func, line)
#   endif
#   define ASSERT_INNER(expr, ...) BUILTIN_ASSERT(_MD::AssertMsg(#expr __VA_OPT__(,) __VA_ARGS__), _MD::AssertLoc(), __LINE__);
#   define ENSURE_INNER(expr, var, ...) ENSURE_EVAL(expr, var) ENSURE_EXPR(var) \
        { ENSURE_LOG(expr __VA_OPT__(,) __VA_ARGS__) ASSERT_INNER(expr __VA_OPT__(,) __VA_ARGS__) BUILTIN_UNREACHABLE } ENSURE_EXPR(var)

#   define ASSERT(expr, ...) ASSERT_EXPR(expr) { ASSERT_INNER(expr __VA_OPT__(,) __VA_ARGS__) BUILTIN_UNREACHABLE }
#   define VERIFY(expr, ...) ASSERT_EXPR(expr) { ASSERT_INNER(expr __VA_OPT__(,) __VA_ARGS__) BUILTIN_UNREACHABLE }
#   define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(,) __VA_ARGS__)
#   define CONSTEXPR_ASSERT(expr, ...) if (BUILTIN_CONSTANT_EVAL) ASSERT(expr __VA_OPT__(,) __VA_ARGS__)
#else
#   define BUILTIN_ASSERT(msg, func, line) ((void)0)
#   define ENSURE_INNER(expr, var, ...) ENSURE_EVAL(expr, var) ENSURE_EXPR(var) \
        { ENSURE_LOG(expr __VA_OPT__(,) __VA_ARGS__) } ENSURE_EXPR(var)

#   define ASSERT(expr, ...) ((void)0)
#   define VERIFY(expr, ...) ASSERT_EXPR(expr) {}
#   define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(,) __VA_ARGS__)
#   define CONSTEXPR_ASSERT(expr, ...) ((void)0)
#endif // DEBUG_ASSERT

#define NEVER_CALLED(msg, ...) static_assert(detail::FalseArgs<__VA_ARGS__>::value, msg); BUILTIN_UNREACHABLE

namespace mini::detail
{

template <typename... Args>
struct FalseArgs { static constexpr bool value = false; };

[[noinline]] CORE_API CHAR_T* AssertMsg(char const*, char const* = nullptr);
[[noinline]] CORE_API CHAR_T* AssertLoc(std::source_location const& = std::source_location::current());
[[noinline]] CORE_API void EnsureHelper(char const*, char const* = nullptr,
                                        std::source_location const& = std::source_location::current());

CORE_API int ConcatStrings(char* dest, int destLen, char const** src, int srcCount);
CORE_API int IntegerToASCII(char* dest, int destLen, int src);
CORE_API int SourceLocationToString(char* dest, int destLen, std::source_location const& loc);

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

#if PLATFORM_WINDOWS && defined(_MINWINDEF_)

inline constexpr bool TestExpr(HINSTANCE instance) noexcept
{
    return instance != nullptr;
}

inline constexpr bool TestExpr(HRESULT result) noexcept
{
    return SUCCEEDED(result);
}

#endif

#if PLATFORM_WINDOWS && defined(__d3dcommon_h__)

[[noinline]] CORE_API void EnsureHelper(char const*, ID3DBlob*,
                                        std::source_location const& = std::source_location::current());

#endif

} // namespace mini::detail
