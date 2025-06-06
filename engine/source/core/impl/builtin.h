#pragma once

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

#if __has_builtin(__builtin_wcschr)
#  define BUILTIN_WCSCHR __builtin_wcschr
#else
#  define BUILTIN_WCSCHR wcschr
#endif

#if __has_builtin(__builtin_wcscmp)
#  define BUILTIN_WCSCMP __builtin_wcscmp
#else
#  define BUILTIN_WCSCMP wcscmp
#endif

#if __has_builtin(__builtin_wcslen)
#  define BUILTIN_WCSLEN __builtin_wcslen
#else
#  define BUILTIN_WCSLEN wcslen
#endif

#if __has_builtin(__builtin_wcsncmp)
#  define BUILTIN_WCSNCMP __builtin_wcsncmp
#else
#  define BUILTIN_WCSNCMP wcsncmp
#endif

#if __has_builtin(__builtin_wmemchr)
#  define BUILTIN_WMEMCHR __builtin_wmemchr
#else
#  define BUILTIN_WMEMCHR wmemchr
#endif

#if __has_builtin(__builtin_wmemcpy)
#  define BUILTIN_WMEMCPY __builtin_wmemcpy
#else
#  define BUILTIN_WMEMCPY wmemcpy
#endif

#if __has_builtin(__builtin_wmemmove)
#  define BUILTIN_WMEMMOVE __builtin_wmemmove
#else
#  define BUILTIN_WMEMMOVE wmemmove
#endif

#if __has_builtin(__builtin_wmemset)
#  define BUILTIN_WMEMSET __builtin_wmemset
#else
#  define BUILTIN_WMEMSET wmemset
#endif