export module mini.core:bit_base;

import :type;
import :numeric;

namespace mini::bit {

template <UnsignedIntegralT T>
[[nodiscard]] inline constexpr uint32 Clz(T x)
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

template <UnsignedIntegralT T>
[[nodiscard]] inline constexpr uint32 Ctz(T x)
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

} // namespace mini::bit
