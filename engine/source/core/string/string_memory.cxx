module;

#include "memory/cstring.h"
#include "memory/cwstring.h"

export module mini.core:string_memory;

import :type;
import :numeric;

namespace mini::memory {

export template <CharT T>
inline constexpr T const* StringSearch(T const* str, T value) noexcept
{
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            return BUILTIN_STRCHR(reinterpret_cast<char const*>(str), char(value));
        } else if constexpr (SameAsT<T, wchar>) {
            return BUILTIN_WCSCHR(str, value);
        }
    }

    for (; *str != T(0) && *str != value; ++str);
    return str;
}

export template <CharT T>
inline constexpr T const* StringSearch(T const* str, T value, SizeT count) noexcept
{
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            return BUILTIN_MEMCHR(str, reinterpret_cast<char>(value), count);
        } else if constexpr (SameAsT<T, wchar>) {
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
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            auto len = BUILTIN_STRLEN(reinterpret_cast<char const*>(str));
            return static_cast<SizeT>(len);
        } else if constexpr (SameAsT<T, wchar>) {
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
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            void* ptr = BUILTIN_MEMCHR(reinterpret_cast<char const*>(str), char(0), count);
            return ptr == nullptr ? 0 : static_cast<SizeT>(static_cast<T const*>(ptr) - str);
        } else if constexpr (SameAsT<T, wchar>) {
            wchar const* end = BUILTIN_WMEMCHR(str, wchar(0), count);
            return end == nullptr ? 0 : static_cast<SizeT>(end - str);
        }
    }

    T const* pos = str;
    for (; count && *pos != T(0); --count, ++pos);
    return static_cast<SizeT>(pos - str);
}

export template <CharT T>
inline constexpr T* StringFill(T* dst, T value, SizeT count) noexcept
{
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            void* ptr = BUILTIN_MEMSET(static_cast<void*>(dst), static_cast<int32>(value),
                                       static_cast<size_t>(count));
            return static_cast<T*>(ptr);
        } else if constexpr (SameAsT<T, wchar>) {
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
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            char const* l = reinterpret_cast<char const*>(s1);
            char const* r = reinterpret_cast<char const*>(s2);
            return static_cast<int32>(BUILTIN_STRCMP(l, r));
        } else if constexpr (SameAsT<T, wchar>) {
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
    if !consteval {
        if constexpr (AnyOfT<T, char, char8>) {
            char const* l = reinterpret_cast<char const*>(s1);
            char const* r = reinterpret_cast<char const*>(s2);
            return static_cast<int32>(BUILTIN_STRNCMP(l, r, count));
        } else if constexpr (SameAsT<T, wchar>) {
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