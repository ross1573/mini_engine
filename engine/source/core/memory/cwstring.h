#pragma once

#include <wchar.h>

#ifndef __has_builtin
#  define __has_builtin(x) 0
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

#if __has_builtin(__builtin_wmemcmp)
#  define BUILTIN_WMEMCMP __builtin_wmemcmp
#else
#  define BUILTIN_WMEMCMP wmemcmp
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