#ifndef CONFIG_H
#define CONFIG_H

#if defined(__has_builtin)
#  define HAS_BUILTIN(x) __has_builtin(x)
#else
#  define HAS_BUILTIN(x) 0
#endif // defined(__has_builtin)

#ifndef CONCAT_INNER
#  define CONCAT_INNER(x, y) x##y
#endif
#ifndef CONCAT
#  define CONCAT(x, y) CONCAT_INNER(x, y)
#endif

#ifndef JOIN_VA_ARGS
#  define JOIN_VA_ARGS(...) __VA_ARGS__
#endif

#endif // CONFIG_H