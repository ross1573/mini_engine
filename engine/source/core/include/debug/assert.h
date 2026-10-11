#ifndef ASSERTION_H
#define ASSERTION_H

#include "config.h"

#if NOASSERT
#  if DEBUG
#    define ASSERT(expr, ...) ((void)0)
#  else
#    define ASSERT(expr, ...)                                      \
        if (::mini::debug::evaluate(expr) == false) [[unlikely]] { \
            ::mini::unreachable();                                 \
        }
#  endif
#  define VERIFY(expr, ...)                                      \
      if (::mini::debug::evaluate(expr) == false) [[unlikely]] { \
          ::mini::unreachable();                                 \
      }

#  define ENSURE(expr, ...)                                                                                           \
      if (::mini::debug::evaluate(expr) ? false : (::mini::debug::logEnsure(#expr __VA_OPT__(, ) __VA_ARGS__), true)) \
          [[unlikely]]
#else
#  define ASSERT(expr, ...)                                           \
      if (::mini::debug::evaluate(expr) == false) [[unlikely]] {      \
          ::mini::debug::logAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::unreachable();                                      \
      }

#  define VERIFY(expr, ...)                                           \
      if (::mini::debug::evaluate(expr) == false) [[unlikely]] {      \
          ::mini::debug::logAssert(#expr __VA_OPT__(, ) __VA_ARGS__); \
          BUILTIN_TRAP();                                             \
          ::mini::unreachable();                                      \
      }

#  define ENSURE(expr, ...)                                                                                      \
      if (::mini::debug::evaluate(expr)                                                                          \
              ? false                                                                                            \
              : (::mini::debug::logEnsure(#expr __VA_OPT__(, ) __VA_ARGS__), BUILTIN_TRAP(), true)) [[unlikely]]
#endif

#define UNSUPPORTED(msg, ...) static_assert(::mini::FalseArgT<__VA_ARGS__>::value, msg)

#endif // ASSERTION_H