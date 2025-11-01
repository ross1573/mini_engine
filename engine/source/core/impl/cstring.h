#pragma once

#include <string.h>

#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_memchr)
#  define BUILTIN_MEMCHR __builtin_memchr
#else
#  define BUILTIN_MEMCHR memchr
#endif

#if __has_builtin(__builtin_memcpy)
#  define BUILTIN_MEMCPY __builtin_memcpy
#else
#  define BUILTIN_MEMCPY memcpy
#endif

#if __has_builtin(__builtin_memmove)
#  define BUILTIN_MEMMOVE __builtin_memmove
#else
#  define BUILTIN_MEMMOVE memmove
#endif

#if __has_builtin(__builtin_memset)
#  define BUILTIN_MEMSET __builtin_memset
#else
#  define BUILTIN_MEMSET memset
#endif

#if __has_builtin(__builtin_memcmp)
#  define BUILTIN_MEMCMP __builtin_memcmp
#else
#  define BUILTIN_MEMCMP memcmp
#endif

#if __has_builtin(__builtin_strchr)
#  define BUILTIN_STRCHR __builtin_strchr
#else
#  define BUILTIN_STRCHR strchr
#endif

#if __has_builtin(__builtin_strcmp)
#  define BUILTIN_STRCMP __builtin_strcmp
#else
#  define BUILTIN_STRCMP strcmp
#endif

#if __has_builtin(__builtin_strlen)
#  define BUILTIN_STRLEN __builtin_strlen
#else
#  define BUILTIN_STRLEN strlen
#endif

#if __has_builtin(__builtin_strncmp)
#  define BUILTIN_STRNCMP __builtin_strncmp
#else
#  define BUILTIN_STRNCMP strncmp
#endif