#ifndef CONFIG_H
#define CONFIG_H

#if defined(__has_builtin)
#  define HAS_BUILTIN(x) __has_builtin(x)
#else
#  define HAS_BUILTIN(x) 0
#endif // HAS_BUILTIN

#ifndef CONCAT_INNER
#  define CONCAT_INNER(x, y) x##y
#endif
#ifndef CONCAT
#  define CONCAT(x, y) CONCAT_INNER(x, y)
#endif // CONCAT

#ifndef JOIN_VA_ARGS
#  define JOIN_VA_ARGS(...) __VA_ARGS__
#endif // JOIN_VA_ARGS

#if MSVC
#  define BUILTIN_TRAP() __debugbreak()
#elif HAS_BUILTIN(__builtin_debugtrap)
#  define BUILTIN_TRAP() __builtin_debugtrap()
#elif HAS_BUILTIN(__builtin_trap)
#  define BUILTIN_TRAP() __builtin_trap()
#else
#  include <signal.h>
#  define BUILTIN_TRAP() raise(SIGTRAP)
#endif // BUILTIN_TRAP

#if HAS_BUILTIN(__builtin_unreachable)
#  define BUILTIN_UNREACHABLE() __builtin_unreachable()
#elif MSVC
#  define BUILTIN_UNREACHABLE() __assume(false)
#else
#  define BUILTIN_UNREACHABLE()
#endif // BUILTIN_UNREACHABLE

#endif // CONFIG_H