#ifndef BUILTIN_CWSTRING_H
#define BUILTIN_CWSTRING_H

#include <wchar.h>

#if HAS_BUILTIN(__builtin_wmemchr)
#  define BUILTIN_WMEMCHR __builtin_wmemchr
#else
#  define BUILTIN_WMEMCHR wmemchr
#endif

#if HAS_BUILTIN(__builtin_wmemcpy)
#  define BUILTIN_WMEMCPY __builtin_wmemcpy
#else
#  define BUILTIN_WMEMCPY wmemcpy
#endif

#if HAS_BUILTIN(__builtin_wmemmove)
#  define BUILTIN_WMEMMOVE __builtin_wmemmove
#else
#  define BUILTIN_WMEMMOVE wmemmove
#endif

#if HAS_BUILTIN(__builtin_wmemset)
#  define BUILTIN_WMEMSET __builtin_wmemset
#else
#  define BUILTIN_WMEMSET wmemset
#endif

#if HAS_BUILTIN(__builtin_wmemcmp)
#  define BUILTIN_WMEMCMP __builtin_wmemcmp
#else
#  define BUILTIN_WMEMCMP wmemcmp
#endif

#if HAS_BUILTIN(__builtin_wcschr)
#  define BUILTIN_WCSCHR __builtin_wcschr
#else
#  define BUILTIN_WCSCHR wcschr
#endif

#if HAS_BUILTIN(__builtin_wcscmp)
#  define BUILTIN_WCSCMP __builtin_wcscmp
#else
#  define BUILTIN_WCSCMP wcscmp
#endif

#if HAS_BUILTIN(__builtin_wcslen)
#  define BUILTIN_WCSLEN __builtin_wcslen
#else
#  define BUILTIN_WCSLEN wcslen
#endif

#if HAS_BUILTIN(__builtin_wcsncmp)
#  define BUILTIN_WCSNCMP __builtin_wcsncmp
#else
#  define BUILTIN_WCSNCMP wcsncmp
#endif

#endif // BUILTIN_CWSTRING_H