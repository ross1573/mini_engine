#pragma once

#include <cassert>
#include <source_location>

#if DEBUG && !defined(NOASSERT)
#  define DEBUG_ASSERT 1
#else
#  define DEBUG_ASSERT 0
#endif // DEBUG && !NOASSERT

#if defined(__has_builtin) && __has_builtin(__builtin_unreachable)
#  define BUILTIN_UNREACHABLE() __builtin_unreachable()
#elif MSVC
#  define BUILTIN_UNREACHABLE() __assume(false)
#else
#  define BUILTIN_UNREACHABLE()
#endif // __has_builtin(__builtin_unreachable)

#if defined(__has_attribute) && __has_attribute(diagnose_if)
#  define DIAGNOSE(type, ...)   __attribute__((diagnose_if(__VA_ARGS__, type)))
#  define DIAGNOSE_WARNING(...) DIAGNOSE("warning", __VA_ARGS__)
#  define DIAGNOSE_ERROR(...)   DIAGNOSE("error", __VA_ARGS__)
#else
#  define DIAGNOSE(type, ...)
#  define DIAGNOSE_WARNING(...)
#  define DIAGNOSE_ERROR(...)
#endif // __has_attribute(diagnose_if)

#define ASSERT_EXPR(expr)      if (!mini::detail::TestExpr(expr)) [[unlikely]]
#define ENSURE_EXPR(expr)      if (!expr) [[unlikely]]
#define ENSURE_EVAL(expr, var) const bool var = mini::detail::TestExpr(expr)
#define ENSURE_LOG(expr, ...)  mini::detail::EnsureHelper(#expr __VA_OPT__(, ) __VA_ARGS__)
#define UNREACHABLE()          mini::detail::Unreachable();
#define ENSURE_CONCAT_IN(x, y) x##y
#define ENSURE_CONCAT(x, y)    ENSURE_CONCAT_IN(x, y)

#if DEBUG_ASSERT
#  if PLATFORM_WINDOWS
#    define BUILTIN_ASSERT(msg, func, line) _wassert(msg, func, line)
#  else
#    define BUILTIN_ASSERT(msg, func, line) __assert(msg, func, line)
#  endif

#  define ASSERT_INNER(expr, ...)                                               \
      BUILTIN_ASSERT(mini::detail::AssertMsg(#expr __VA_OPT__(, ) __VA_ARGS__), \
                     mini::detail::AssertLoc(), __LINE__)

#  define ENSURE_INNER(expr, var, ...)                   \
      ENSURE_EVAL(expr, var);                            \
      ENSURE_EXPR(var) {                                 \
          ENSURE_LOG(expr __VA_OPT__(, ) __VA_ARGS__);   \
          ASSERT_INNER(expr __VA_OPT__(, ) __VA_ARGS__); \
          UNREACHABLE();                                 \
      }                                                  \
      ENSURE_EXPR(var)

#  define ASSERT(expr, ...)                              \
      ASSERT_EXPR(expr) {                                \
          ASSERT_INNER(expr __VA_OPT__(, ) __VA_ARGS__); \
          UNREACHABLE();                                 \
      }

#  define VERIFY(expr, ...)                              \
      ASSERT_EXPR(expr) {                                \
          ASSERT_INNER(expr __VA_OPT__(, ) __VA_ARGS__); \
          UNREACHABLE();                                 \
      }

#  define ENSURE(expr, ...)                                                              \
      ENSURE_INNER(expr, ENSURE_CONCAT(ensure_, __COUNTER__) __VA_OPT__(, ) __VA_ARGS__)

#else
#  define BUILTIN_ASSERT(msg, func, line) ((void)0)
#  define ASSERT(expr, ...)               ((void)0)

#  define ENSURE_INNER(expr, var, ...)                 \
      ENSURE_EVAL(expr, var);                          \
      ENSURE_EXPR(var) {                               \
          ENSURE_LOG(expr __VA_OPT__(, ) __VA_ARGS__); \
      }                                                \
      ENSURE_EXPR(var)

#  define VERIFY(expr, ...) \
      ASSERT_EXPR(expr) {}

#  define ENSURE(expr, ...)                                                              \
      ENSURE_INNER(expr, ENSURE_CONCAT(ensure_, __COUNTER__) __VA_OPT__(, ) __VA_ARGS__)

#endif // DEBUG_ASSERT

#define NEVER_CALLED(msg, ...)                                 \
    static_assert(detail::FalseArgs<__VA_ARGS__>::value, msg); \
    UNREACHABLE();

namespace mini::detail {

template <typename... Args>
struct FalseArgs {
    static constexpr bool value = false;
};

inline constexpr void Unreachable()
{
    BUILTIN_UNREACHABLE();
}

#if PLATFORM_WINDOWS
#  define CHAR_T wchar_t*
#else
#  define CHAR_T char*
#endif

ASSERT_API CHAR_T AssertMsg(char const*, char const* = nullptr);
ASSERT_API CHAR_T AssertLoc(std::source_location const& = std::source_location::current());
ASSERT_API void EnsureHelper(char const*, char const* = nullptr,
                             std::source_location const& = std::source_location::current());

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

#undef CHAR_T
#undef BUILTIN_UNREACHABLE
#undef BUILTIN_ASSSERT