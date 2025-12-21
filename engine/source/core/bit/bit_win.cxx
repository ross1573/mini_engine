module;

#include <intrin.h>

export module mini.bit:platform;

import mini.type;
import :base;

namespace mini::bit {

inline constexpr uint32 _clz(uint32 x)
{
    if consteval {
        return Clz(x);
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
        return Clz(x);
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

inline constexpr uint32 _clzl(unsigned long x)
{
    if constexpr (sizeof(unsigned long) == 4) {
        return _clz(x);
    }
    else if constexpr (sizeof(unsigned long) == 8) {
        return _clzll(x);
    }

    return Clz(x);
}

inline constexpr uint32 _ctz(uint32 x)
{
    if consteval {
        return Ctz(x);
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
        return Ctz(x);
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

inline constexpr uint32 _ctzl(unsigned long x)
{
    if constexpr (sizeof(unsigned long) == 4) {
        return _ctz(x);
    }
    else if constexpr (sizeof(unsigned long) == 8) {
        return _ctzll(x);
    }

    return Ctz(x);
}

} // namespace mini::bit