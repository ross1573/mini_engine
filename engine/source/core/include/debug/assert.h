#ifndef ASSERTION_H
#define ASSERTION_H

#include "config.h"

#if MSVC
#  define BUILTIN_TRAP() __debugbreak()
#elif HAS_BUILTIN(__builtin_debugtrap)
#  define BUILTIN_TRAP() __builtin_debugtrap()
#elif HAS_BUILTIN(__builtin_trap)
#  define BUILTIN_TRAP() __builtin_trap()
#else
#  include <signal.h>
#  define BUILTIN_TRAP() raise(SIGTRAP)
#endif

#if NOASSERT
#  define VERIFY_INNER(expr)                                           \
      if (::mini::debug::EvaluateExpr(expr) == false) [[unlikely]] { }

#  define ENSURE_INNER(expr, var, ...)                                \
      const bool var = ::mini::debug::EvaluateExpr(expr) == false;    \
      if (var) [[unlikely]] {                                         \
          ::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__); \
      }                                                               \
      if (var) [[unlikely]]

#  define ASSERT(expr, ...) ((void)0)
#  define VERIFY(expr, ...) VERIFY_INNER(expr)
#  define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(, ) __VA_ARGS__)
#else
#  define ENSURE_INNER(expr, var, ...)                                \
      const bool var = ::mini::debug::EvaluateExpr(expr) == false;    \
      if (var) [[unlikely]] {                                         \
          ::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
      }                                                               \
      if (var) [[unlikely]]

#  define ASSERT(expr, ...)                                           \
      if (::mini::debug::EvaluateExpr(expr) == false) [[unlikely]] {  \
          ::mini::debug::LogAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::Unreachable();                                      \
      }

#  define VERIFY(expr, ...)                                           \
      if (::mini::debug::EvaluateExpr(expr) == false) [[unlikely]] {  \
          ::mini::debug::LogAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::Unreachable();                                      \
      }

#  define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(, ) __VA_ARGS__)
#endif

#undef BUILTIN_ASSSERT

#define UNSUPPORTED(msg, ...) static_assert(::mini::FalseArgT<__VA_ARGS__>::value, msg)

#endif // ASSERTION_H