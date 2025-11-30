export module mini.core:math_base;

import :type;

namespace mini {

template <IntegralT T>
inline constexpr bool MulOverflow(T const x, T const y, T* result)
{
    if constexpr (UnsignedT<T>) {
        *result = static_cast<T>(x * y);
        return (x != 0) && (y > NumericLimit<T>::max / x);
    }

    using Unsigned = UnsignedOfT<T>;

    Unsigned const ux = x < 0 ? (0 - static_cast<Unsigned>(x)) : static_cast<Unsigned>(x);
    Unsigned const uy = y < 0 ? (0 - static_cast<Unsigned>(y)) : static_cast<Unsigned>(y);
    Unsigned const ur = ux * uy;

    bool const isNegative = (x < 0) ^ (y < 0);
    *result = isNegative ? (0 - ur) : ur;

    if (ux == 0 || uy == 0) {
        return false;
    }

    constexpr Unsigned max = static_cast<Unsigned>(NumericLimit<T>::max);
    return isNegative ? (ux > (max + Unsigned(1)) / uy) : ux > max / uy;
}

} // namespace mini