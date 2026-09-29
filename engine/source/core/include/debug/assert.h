#ifndef ASSERTION_H
#define ASSERTION_H

#include "config.h"

#if NOASSERT
#  if DEBUG
#    define ASSERT(expr, ...) ((void)0)
#  else
#    define ASSERT(expr, ...)                                      \
        if (::mini::debug::Evaluate(expr) == false) [[unlikely]] { \
            ::mini::Unreachable();                                 \
        }
#  endif
#  define VERIFY(expr, ...)                                      \
      if (::mini::debug::Evaluate(expr) == false) [[unlikely]] { \
          ::mini::Unreachable();                                 \
      }

#  define ENSURE(expr, ...)                                                                                           \
      if (::mini::debug::Evaluate(expr) ? false : (::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__), true)) \
          [[unlikely]]
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

#  define ENSURE(expr, ...)                                                                                      \
      if (::mini::debug::Evaluate(expr)                                                                          \
              ? false                                                                                            \
              : (::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__), BUILTIN_TRAP(), true)) [[unlikely]]
#endif

#define UNSUPPORTED(msg, ...) static_assert(::mini::FalseArgT<__VA_ARGS__>::value, msg)

#endif // ASSERTION_H