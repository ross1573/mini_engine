module;

#include "bit.h"
#include "memory/cstring.h"

export module mini.core:bit_operation;

import :type;
import :memory_operation;
import :bit_base;

#if PLATFORM_WINDOWS
import :bit_platform;
#endif

namespace mini::bit {

export template <typename To, typename From>
constexpr To bitCast(From const& from) noexcept
    requires(sizeof(To) == sizeof(From))
{
    if consteval {
        return BUILTIN_BIT_CAST(To, from);
    } else {
        To result;
        BUILTIN_MEMCPY(addressof(from), addressof(result), sizeof(From));
        return result;
    }
}

export template <UnsignedIntegralT T>
constexpr T rotateLeft(T num, int32 count) noexcept
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    if !consteval {
        if constexpr (sizeof(T) == 1 && HAS_BUILTIN_ROTL_1) {
            return BUILTIN_ROTL_1(num, count);
        } else if constexpr (sizeof(T) == 2 && HAS_BUILTIN_ROTL_2) {
            return BUILTIN_ROTL_2(num, count);
        } else if constexpr (sizeof(T) == 4 && HAS_BUILTIN_ROTL_4) {
            return BUILTIN_ROTL_4(num, count);
        } else if constexpr (sizeof(T) == 8 && HAS_BUILTIN_ROTL_8) {
            return BUILTIN_ROTL_8(num, count);
        }
    }

    int32 remainder = count % digits;
    if (remainder == 0) {
        return num;
    }

    return (remainder > 0) ? static_cast<T>((num << remainder) | (num >> (digits - remainder)))
                           : static_cast<T>((num >> -remainder) | (num << (digits + remainder)));
}

export template <UnsignedIntegralT T>
constexpr T rotateRight(T num, int32 count) noexcept
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    if !consteval {
        if constexpr (sizeof(T) == 1 && HAS_BUILTIN_ROTR_1) {
            return BUILTIN_ROTR_1(num, count);
        } else if constexpr (sizeof(T) == 2 && HAS_BUILTIN_ROTR_2) {
            return BUILTIN_ROTR_2(num, count);
        } else if constexpr (sizeof(T) == 4 && HAS_BUILTIN_ROTR_4) {
            return BUILTIN_ROTR_4(num, count);
        } else if constexpr (sizeof(T) == 8 && HAS_BUILTIN_ROTR_8) {
            return BUILTIN_ROTR_8(num, count);
        }
    }

    int32 remainder = count % digits;
    if (remainder == 0) {
        return num;
    }

    return (remainder > 0) ? static_cast<T>((num >> remainder) | (num << (digits - remainder)))
                           : static_cast<T>((num << -remainder) | (num >> (digits + remainder)));
}

export template <UnsignedIntegralT T>
constexpr uint32 countLeftZero(T num) noexcept
{
    constexpr uint32 digits = NumericLimit<T>::digits;
    if constexpr (HAS_BUILTIN_CLZG) {
        return static_cast<uint32>(BUILTIN_CLZG(num, static_cast<int32>(digits)));
    }

    if (num == 0) {
        return digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return static_cast<uint32>(BUILTIN_CLZ(num)) - (NumericLimit<unsigned int>::digits - digits);
    } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
        return static_cast<uint32>(BUILTIN_CLZL(num)) - (NumericLimit<unsigned long>::digits - digits);
    } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
        return static_cast<uint32>(BUILTIN_CLZLL(num)) - (NumericLimit<unsigned long long>::digits - digits);
    } else {
        uint32 result = 0;
        uint32 iter = 0;

        constexpr uint32 ullDigits = NumericLimit<unsigned long long>::digits;
        while (true) {
            num = rotateLeft(num, ullDigits);
            iter = countLeftZero(static_cast<unsigned long long>(num));
            if (iter != ullDigits) {
                break;
            }

            result += iter;
        }

        return result + iter;
    }
}

export template <UnsignedIntegralT T>
constexpr uint32 countLeftOne(T num) noexcept
{
    return num != NumericLimit<T>::max ? countLeftZero(static_cast<T>(~num)) : NumericLimit<T>::digits;
}

export template <UnsignedIntegralT T>
constexpr uint32 countRightZero(T num) noexcept
{
    constexpr uint32 digits = NumericLimit<T>::digits;
    if constexpr (HAS_BUILTIN_CTZG) {
        return static_cast<uint32>(BUILTIN_CTZG(num, static_cast<int32>(digits)));
    }

    if (num == 0) {
        return digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return static_cast<uint32>(BUILTIN_CTZ(num));
    } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
        return static_cast<uint32>(BUILTIN_CTZL(num));
    } else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
        return static_cast<uint32>(BUILTIN_CTZLL(num));
    } else {
        uint32 result = 0;
        constexpr uint32 ullDigits = NumericLimit<unsigned long long>::digits;
        while (static_cast<unsigned long long>(num) == static_cast<unsigned long long>(0)) {
            result += ullDigits;
            num >>= ullDigits;
        }

        return result + countRightZero(static_cast<unsigned long long>(num));
    }
}

export template <UnsignedIntegralT T>
constexpr uint32 countRightOne(T num) noexcept
{
    return num != NumericLimit<T>::max ? countRightZero(static_cast<T>(~num)) : NumericLimit<T>::digits;
}

} // namespace mini::bit