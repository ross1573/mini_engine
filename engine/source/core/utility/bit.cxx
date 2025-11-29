module;

#include "bit.h"

export module mini.core:bit;

import :type;

#if PLATFORM_WINDOWS
import :bit_impl;
#endif

namespace mini::bits {

export template <UnsignedIntegralT T>
inline constexpr T RotateLeft(T num, int32 count) noexcept
{
    constexpr uint32 digits = NumericLimit<T>::digits;

    if !consteval {
        if constexpr (sizeof(T) == 1 && HAS_BUILTIN_ROTL_1) {
            return BUILTIN_ROTL_1(num, count);
        }
        else if constexpr (sizeof(T) == 2 && HAS_BUILTIN_ROTL_2) {
            return BUILTIN_ROTL_2(num, count);
        }
        else if constexpr (sizeof(T) == 4 && HAS_BUILTIN_ROTL_4) {
            return BUILTIN_ROTL_4(num, count);
        }
        else if constexpr (sizeof(T) == 8 && HAS_BUILTIN_ROTL_8) {
            return BUILTIN_ROTL_8(num, count);
        }
    }

    int32 remainder = count % digits;
    if (remainder == 0) {
        return num;
    }

    return (remainder > 0) ? ((num << remainder) | (num >> (digits - remainder)))
                           : ((num >> -remainder) | (num << (digits + remainder)));
}

export template <UnsignedIntegralT T>
inline constexpr T RotateRight(T num, int32 count) noexcept
{
    constexpr uint32 digits = NumericLimit<T>::digits;

    if !consteval {
        if constexpr (sizeof(T) == 1 && HAS_BUILTIN_ROTR_1) {
            return BUILTIN_ROTR_1(num, count);
        }
        else if constexpr (sizeof(T) == 2 && HAS_BUILTIN_ROTR_2) {
            return BUILTIN_ROTR_2(num, count);
        }
        else if constexpr (sizeof(T) == 4 && HAS_BUILTIN_ROTR_4) {
            return BUILTIN_ROTR_4(num, count);
        }
        else if constexpr (sizeof(T) == 8 && HAS_BUILTIN_ROTR_8) {
            return BUILTIN_ROTR_8(num, count);
        }
    }

    int32 remainder = count % digits;
    if (remainder == 0) {
        return num;
    }

    return (remainder > 0) ? ((num >> remainder) | (num << (digits - remainder)))
                           : ((num << -remainder) | (num >> (digits + remainder)));
}

export template <UnsignedIntegralT T>
inline constexpr uint32 CountLeftZero(T num) noexcept
{
    static_assert(HAS_BUILTIN_CLZLL, "CLZ is not implemented on this platform");

    constexpr uint32 digits = NumericLimit<T>::digits;
    if constexpr (HAS_BUILTIN_CLZG) {
        return static_cast<uint32>(BUILTIN_CLZG(num, static_cast<int32>(digits)));
    }

    if (num == 0) {
        return digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned int) && HAS_BUILTIN_CLZ) {
        return static_cast<uint32>(BUILTIN_CLZ(num)) -
               (NumericLimit<unsigned int>::digits - digits);
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long) && HAS_BUILTIN_CLZL) {
        return static_cast<uint32>(BUILTIN_CLZL(num)) -
               (NumericLimit<unsigned long>::digits - digits);
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long long) && HAS_BUILTIN_CLZLL) {
        return static_cast<uint32>(BUILTIN_CLZLL(num)) -
               (NumericLimit<unsigned long long>::digits - digits);
    }
    else {
        uint32 result = 0;
        uint32 iter = 0;

        constexpr uint32 ullDigits = NumericLimit<unsigned long long>::digits;
        while (true) {
            num = RotateLeft(num, ullDigits);
            iter = CountLeftZero(static_cast<unsigned long long>(num));
            if (iter != ullDigits) {
                break;
            }

            result += iter;
        }

        return result + iter;
    }
}

export template <UnsignedIntegralT T>
inline constexpr uint32 CountLeftOne(T num) noexcept
{
    return num != NumericLimit<T>::max ? CountLeftZero(static_cast<T>(~num))
                                       : NumericLimit<T>::digits;
}

export template <UnsignedIntegralT T>
inline constexpr uint32 CountRightZero(T num) noexcept
{
    static_assert(HAS_BUILTIN_CTZLL, "CTZ is not implemented on this platform");

    constexpr uint32 digits = NumericLimit<T>::digits;
    if constexpr (HAS_BUILTIN_CTZG) {
        return static_cast<uint32>(BUILTIN_CTZG(num, static_cast<int32>(digits)));
    }

    if (num == 0) {
        return digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned int) && HAS_BUILTIN_CTZ) {
        return static_cast<uint32>(BUILTIN_CTZ(num));
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long) && HAS_BUILTIN_CTZL) {
        return static_cast<uint32>(BUILTIN_CTZL(num));
    }
    else if constexpr (sizeof(T) <= sizeof(unsigned long long) && HAS_BUILTIN_CTZLL) {
        return static_cast<uint32>(BUILTIN_CTZLL(num));
    }
    else {
        uint32 result = 0;
        constexpr uint32 ullDigits = NumericLimit<unsigned long long>::digits;
        while (static_cast<unsigned long long>(num) == static_cast<unsigned long long>(0)) {
            result += ullDigits;
            num >>= ullDigits;
        }

        return result + CountRightZero(static_cast<unsigned long long>(num));
    }
}

export template <UnsignedIntegralT T>
inline constexpr uint32 CountRightOne(T num) noexcept
{
    return num != NumericLimit<T>::max ? CountRightZero(static_cast<T>(~num))
                                       : NumericLimit<T>::digits;
}

} // namespace mini::bits