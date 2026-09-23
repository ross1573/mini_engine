#ifndef ASSERTION_H
#define ASSERTION_H

#include "config.h"

#if NOASSERT
#  if DEBUG
#    define ASSERT(expr, ...) ((void)0)
#  else
#    define ASSERT(expr, ...)                              \
        if (!::mini::debug::Evaluate(expr)) [[unlikely]] { \
            ::mini::Unreachable();                         \
        }
#  endif
#  define VERIFY(expr, ...)                              \
      if (!::mini::debug::Evaluate(expr)) [[unlikely]] { \
          ::mini::Unreachable();                         \
      }

#  define ENSURE(expr, ...) if (!::mini::debug::LogEvaluate(expr, #expr __VA_OPT__(, ) __VA_ARGS__)) [[unlikely]]
#else
#  define ASSERT(expr, ...)                                           \
      if (::mini::debug::Evaluate(expr) == false) [[unlikely]] {      \
          ::mini::debug::LogAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::Unreachable();                                      \
      }

#  define VERIFY(expr, ...)                                           \
      if (::mini::debug::Evaluate(expr) == false) [[unlikely]] {      \
          ::mini::debug::LogAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::Unreachable();                                      \
      }

#  define ENSURE_INNER(expr, var, ...)                                \
      const bool var = ::mini::debug::Evaluate(expr) == false;        \
      if (var) [[unlikely]] {                                         \
          ::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
      }                                                               \
      if (var) [[unlikely]]

#  define ENSURE(expr, ...) ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(, ) __VA_ARGS__)
#endif

#define UNSUPPORTED(msg, ...) static_assert(::mini::FalseArgT<__VA_ARGS__>::value, msg)

#endif // ASSERTION_H