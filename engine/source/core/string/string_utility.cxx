module;

#include <cstring>
#include <cwchar>

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

export module mini.core:string_utility;

import :type;
import :utility;

namespace mini::memory {

export template <CharT T>
inline constexpr T const* StringSearch(T const* str, T value) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            return BUILTIN_STRCHR(reinterpret_cast<char const*>(str), char(value));
        }
        else if constexpr (SameAsT<T, wchar>) {
            return BUILTIN_WCSCHR(str, value);
        }
    }

    for (; *str != T(0) && *str != value; ++str);
    return str;
}

export template <CharT T>
inline constexpr T const* StringSearch(T const* str, T value, SizeT count) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            return BUILTIN_MEMCHR(str, reinterpret_cast<char>(value), count);
        }
        else if constexpr (SameAsT<T, wchar>) {
            return BUILTIN_WMEMCHR(str, value, count);
        }
    }

    for (; count; --count) {
        if (*str == value) {
            return str;
        }
    }

    return nullptr;
}

export template <CharT T>
inline constexpr SizeT StringLength(T const* str) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            auto len = BUILTIN_STRLEN(reinterpret_cast<char const*>(str));
            return static_cast<SizeT>(len);
        }
        else if constexpr (SameAsT<T, wchar>) {
            return static_cast<SizeT>(BUILTIN_WCSLEN(str));
        }
    }

    T const* pos = str;
    for (; *pos != T(0); ++pos);
    return static_cast<SizeT>(pos - str);
}

export template <CharT T>
inline constexpr SizeT StringLength(T const* str, SizeT count) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            void* ptr = BUILTIN_MEMCHR(reinterpret_cast<char const*>(str), char(0), count);
            return ptr == nullptr ? 0 : static_cast<SizeT>(static_cast<T const*>(ptr) - str);
        }
        else if constexpr (SameAsT<T, wchar>) {
            wchar const* end = BUILTIN_WMEMCHR(str, wchar(0), count);
            return end == nullptr ? 0 : static_cast<SizeT>(end - str);
        }
    }

    T const* pos = str;
    for (; count && *pos != T(0); --count, ++pos);
    return static_cast<SizeT>(pos - str);
}

export template <CharT T>
inline constexpr T* StringCopy(T* dst, T const* src, SizeT len) noexcept
{
    if (!IsConstantEvaluated()) {
        void* ptr = BUILTIN_MEMCPY(static_cast<void*>(dst), static_cast<void const*>(src),
                                   static_cast<size_t>(len) * sizeof(T));
        return static_cast<T*>(ptr);
    }

    for (T* d = dst; len != 0; --len) {
        *d++ = *src++;
    }

    return dst;
}

export template <CharT T>
inline constexpr T* StringCopy(T* dst, T const* src) noexcept
{
    SizeT len = StringLength(src) + 1;
    return StringCopy(dst, src, len);
}

export template <CharT T>
inline constexpr T* StringMove(T* dst, T const* src, SizeT len) noexcept
{
    if (!IsConstantEvaluated()) {
        void* ptr = BUILTIN_MEMMOVE(static_cast<void*>(dst), static_cast<void const*>(src),
                                    static_cast<size_t>(len) * sizeof(T));
        return static_cast<T*>(ptr);
    }

    if (static_cast<T const*>(dst) < src) {
        for (T* d = dst; len != 0; --len) {
            *d++ = *src++;
        }
    }
    else {
        T const* s = src + len;
        T* d = dst + len;

        for (; len != 0; --len) {
            *(--d) = *(--s);
        }
    }

    return dst;
}

export template <CharT T>
inline constexpr T* StringMove(T* dst, T const* src) noexcept
{
    SizeT len = StringLength(src) + 1;
    return StringMove(dst, src, len);
}

export template <CharT T>
inline constexpr T* StringFill(T* dst, T value, SizeT count) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            void* ptr = BUILTIN_MEMSET(static_cast<void*>(dst), static_cast<int>(value),
                                       static_cast<size_t>(count));
            return static_cast<T*>(ptr);
        }
        else if constexpr (SameAsT<T, wchar>) {
            return BUILTIN_WMEMSET(dst, value, static_cast<size_t>(count));
        }
    }

    for (T* d = dst; count != 0; --count) {
        *d++ = value;
    }

    return dst;
}

export template <CharT T>
inline constexpr int32 StringCompare(T const* s1, T const* s2) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            char const* l = reinterpret_cast<char const*>(s1);
            char const* r = reinterpret_cast<char const*>(s2);
            return static_cast<int32>(BUILTIN_STRCMP(l, r));
        }
        else if constexpr (SameAsT<T, wchar>) {
            return static_cast<int32>(BUILTIN_WCSCMP(s1, s2));
        }
    }

    T lch = T(0);
    T rch = T(0);

    for (; lch == rch; ++s1, ++s2) {
        lch = *s1;
        rch = *s2;

        if (lch == T(0)) {
            return static_cast<int32>(lch - rch);
        }
    }

    return static_cast<int32>(lch - rch);
}

export template <CharT T>
inline constexpr int32 StringCompare(T const* s1, T const* s2, SizeT count) noexcept
{
    if (!IsConstantEvaluated()) {
        if constexpr (AnyOfT<T, char, char8>) {
            char const* l = reinterpret_cast<char const*>(s1);
            char const* r = reinterpret_cast<char const*>(s2);
            return static_cast<int32>(BUILTIN_STRNCMP(l, r, count));
        }
        else if constexpr (SameAsT<T, wchar>) {
            return static_cast<int32>(BUILTIN_WCSNCMP(s1, s2, count));
        }
    }

    for (; count != 0; --count, ++s1, ++s2) {
        T lch = *s1;
        T rch = *s2;

        if (lch != rch) {
            return static_cast<int32>(lch - rch);
        }
    }

    return 0;
}

} // namespace mini::memory