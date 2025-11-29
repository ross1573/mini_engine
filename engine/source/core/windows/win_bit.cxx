module;

#include <intrin.h>

export module mini.core:bit_impl;

import :type;

namespace mini::bit {

template <UnsignedIntegralT T>
inline constexpr uint32 clz(T x)
{
    constexpr uint32 digits = NumericLimit<T>::digits;
    uint32 n = digits;
    uint32 c = digits >> 1;

    do {
        T y = static_cast<T>(x >> c);
        if (y) {
            n -= c;
            x = y;
        }

        c >>= 1;
    } while (c);

    return n - static_cast<uint32>(x);
}

inline constexpr uint32 _clz(uint32 x)
{
    if consteval {
        return clz(x);
    }

// TODO: optimize with AVX inst
#if ARCH_X86 || ARCH_X86_64
    unsigned long result = 0;
    _BitScanReverse(&result, x);
    return 31 ^ static_cast<uint32>(result);
#elif ARCH_ARM64
    return _CountLeadingZeros(x);
#else
#  error "unsupported architecture"
#endif // ARCH_X86 || ARCH_X86_64
}

inline constexpr uint32 _clzll(uint64 x)
{
    if consteval {
        return clz(x);
    }

// TODO: optimize with AVX inst
#if ARCH_X86
    unsigned long result = 0;
    if (_BitScanReverse(&result, static_cast<uint32>(x >> 32))) {
        return 63 ^ static_cast<uint32>(result + 32);
    }

    _BitScanReverse(&result, static_cast<uint32>(x));
    return 63 ^ static_cast<uint32>(result);
#elif ARCH_X86_64
    unsigned long result = 0;
    _BitScanReverse64(&result, x);
    return 63 ^ static_cast<uint32>(result);
#elif ARCH_ARM64
    return _CountLeadingZeros64(x);
#else
#  error "unsupported architecture"
#endif // ARCH_X86
}

template <UnsignedIntegralT T>
inline constexpr uint32 ctz(T x)
{
    constexpr uint32 digits = NumericLimit<T>::digits;
    uint32 n = digits;
    uint32 c = digits >> 1;
    x = static_cast<T>(~x & (x - 1));

    do {
        T y = static_cast<T>(x >> c);
        if (y) {
            n -= c;
            x = y;
        }

        c >>= 1;
    } while (c);

    return digits - n + static_cast<uint32>(x);
}

inline constexpr uint32 _ctz(uint32 x)
{
    if consteval {
        return ctz(x);
    }

// TODO: optimize with AVX inst
#if ARCH_X86 || ARCH_X86_64
    unsigned long result = 0;
    _BitScanForward(&result, x);
    return result;
#elif ARCH_ARM64
    return _CountTrailingZeros(x);
#else
#  error "unsupported architecture"
#endif // ARCH_X86 || ARCH_X86_64
}

inline constexpr uint32 _ctzll(uint64 x)
{
    if consteval {
        return ctz(x);
    }

// TODO: optimize with AVX inst
#if ARCH_X86 || ARCH_X86_64
    unsigned long result = 0;
    if (_BitScanForward(&result, static_cast<uint32>(x >> 32))) {
        return static_cast<uint32>(result + 32);
    }

    _BitScanForward(&result, static_cast<uint32>(x));
    return static_cast<uint32>(result);
#elif ARCH_ARM64
    return _CountTrailingZeros64(x);
#else
#  error "unsupported architecture"
#endif
}

} // namespace mini::bit