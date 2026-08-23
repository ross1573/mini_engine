module;

#include "cstring.h"
#include "cwstring.h"

export module mini.core:cmemory;

import :type;

namespace mini::memory {

export template <TrivialT T>
inline constexpr void MemCopy(T* dest, T const* src, size_t len) noexcept
{
    if !consteval {
        BUILTIN_MEMCPY(dest, src, len * sizeof(T));
        return;
    }

    for (; len; --len) {
        *dest++ = *src++;
    }
}

export template <TrivialT T>
inline constexpr void MemCopyBackward(T* dest, T const* src, size_t len) noexcept
{
    if !consteval {
        BUILTIN_MEMMOVE(dest - len, src - len, len * sizeof(T));
        return;
    }

    for (; len; --len) {
        *(--dest) = *(--src);
    }
}

export template <TrivialT T>
inline constexpr void MemMove(T* dest, T const* src, size_t len) noexcept
{
    if !consteval {
        BUILTIN_MEMMOVE(dest, src, len * sizeof(T));
        return;
    }

    if (static_cast<T const*>(dest) < src) {
        MemCopy(dest, src, len);
    } else {
        MemCopyBackward(dest + len, src + len, len);
    }
}

export template <TrivialT T>
inline constexpr int32 MemCompare(T const* x, T const* y, size_t len) noexcept
{
    if !consteval {
        return BUILTIN_MEMCMP(x, y, len * sizeof(T));
    }

    for (; len; --len) {
        if (*x != *y) {
            return (*x < *y) ? -1 : 1;
        }

        ++x;
        ++y;
    }

    return 0;
}

} // namespace mini::memory