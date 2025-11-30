export module mini.core:math_base;

import :type;

namespace mini {

template <SignedIntegralT T>
inline constexpr bool MulOverflow(T x, T y, T& result)
{
    using Unsigned = UnsignedOfT<T>;

    constexpr Unsigned ux = x < 0 ? (0 - static_cast<Unsigned>(x)) : static_cast<Unsigned>(x);
    constexpr Unsigned uy = y < 0 ? (0 - static_cast<Unsigned>(y)) : static_cast<Unsigned>(y);
    constexpr Unsigned ur = ux * uy;

    constexpr bool isNegative = (x < 0) ^ (y < 0);
    result = isNegative ? (0 - ur) : ur;

    if (ux == 0 || uy == 0) {
        return false;
    }

    constexpr Unsigned max = static_cast<Unsigned>(NumericLimit<T>::max);
    return isNegative ? (ux > (max + Unsigned(1)) / uy) : ux > max / uy;
}

} // namespace mini