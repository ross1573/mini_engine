export module mini.core:ratio;

import :type;
import :math_base;
import :math_operation;

namespace mini {

export template <int64 NumN, int64 DenomN = 1>
class Ratio;

template <typename T>
constexpr bool IsRatioT = false;

template <int64 NumN, int64 DenomN>
constexpr bool IsRatioT<Ratio<NumN, DenomN>> = true;

export template <typename T>
concept RatioT = IsRatioT<T>;

export template <RatioT R1, RatioT R2>
using RatioAddT = decltype(R1{} + R2{});

export template <RatioT R1, RatioT R2>
using RatioSubtractT = decltype(R1{} - R2{});

export template <RatioT R1, RatioT R2>
using RatioMultiplyT = decltype(R1{} * R2{});

export template <RatioT R1, RatioT R2>
using RatioDivideT = decltype(R1{} / R2{});

template <int64 NumN, int64 DenomN>
class Ratio {
private:
    template <int64 NumU, int64 DenomU>
    friend class Ratio;

    static constexpr int64 Sign(int64 n) { return n == 0 ? 0 : (n < 0 ? -1 : 1); }

    static_assert(Abs(NumN) >= 0, "Numerator is out of range");
    static_assert(Abs(DenomN) >= 0, "Denominator is out of range");
    static_assert(DenomN != 0, "integer division by zero");

    static constexpr int64 unum = Abs(NumN);
    static constexpr int64 uden = Abs(DenomN);
    static constexpr int64 sign = Sign(NumN) * Sign(DenomN);
    static constexpr int64 gcd = Gcd(unum, uden);

public:
    static inline constexpr int64 num = sign * unum / gcd;
    static inline constexpr int64 denom = uden / gcd;
};

template <int64 X, int64 Y>
consteval int64 CheckedAdd()
{
    constexpr uint64 ux = static_cast<uint64>(X);
    constexpr uint64 uy = static_cast<uint64>(Y);
    constexpr int64 result = static_cast<int64>(ux + uy);

    if constexpr (X > 0 && Y > 0) {
        static_assert(result > 0, "integer add overflow");
    }
    else if constexpr (X < 0 && Y < 0) {
        static_assert(result < 0, "integer add overflow");
    }

    return result;
}

template <int64 X, int64 Y>
consteval int64 CheckedSub()
{
    constexpr uint64 ux = static_cast<uint64>(X);
    constexpr uint64 uy = static_cast<uint64>(Y);
    constexpr int64 result = static_cast<int64>(ux - uy);

    if constexpr (X <= 0 && Y > 0) {
        static_assert(result < 0, "integer sub overflow");
    }
    else if constexpr (X >= 0 && Y < 0) {
        static_assert(result > 0, "integer sub overflow");
    }

    return result;
}

template <int64 X, int64 Y>
consteval int64 CheckedMul()
{
    constexpr uint64 ux = X < 0 ? (0 - static_cast<uint64>(X)) : static_cast<uint64>(X);
    constexpr uint64 uy = Y < 0 ? (0 - static_cast<uint64>(Y)) : static_cast<uint64>(Y);
    constexpr uint64 ur = ux * uy;

    constexpr bool isNeg = (X < 0) ^ (Y < 0);
    constexpr int64 result = static_cast<int64>(isNeg ? (0 - ur) : ur);

    if constexpr (ux != 0 && uy != 0) {
        constexpr uint64 max = static_cast<uint64>(NumericLimit<int64>::max);
        constexpr uint64 s = isNeg ? max + int64(1) : max;
        static_assert(ux <= s / uy, "integer mul overflow");
    }

    return result;
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Multiply(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcd1 = Gcd(x.num, y.denom);
    constexpr int64 gcd2 = Gcd(x.denom, y.num);

    constexpr int64 num = CheckedMul<x.num / gcd1, y.num / gcd2>();
    constexpr int64 den = CheckedMul<y.denom / gcd1, x.denom / gcd2>();

    return Ratio<num, den>{};
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Divide(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = Gcd(x.num, y.num);
    constexpr int64 gcdDen = Gcd(x.denom, y.denom);

    constexpr int64 num = CheckedMul<x.num / gcdNum, y.denom / gcdDen>();
    constexpr int64 den = CheckedMul<y.num / gcdNum, x.denom / gcdDen>();

    return Ratio<num, den>{};
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Add(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = Gcd(x.num, y.num);
    constexpr int64 gcdDen = Gcd(x.denom, y.denom);

    constexpr int64 num = CheckedAdd<CheckedMul<x.num / gcdNum, y.denom / gcdDen>(),
                                     CheckedMul<y.num / gcdNum, x.denom / gcdDen>()>();

    constexpr Ratio<gcdNum, x.denom / gcdDen> r1;
    constexpr Ratio<num, y.denom> r2;

    return Multiply(r1, r2);
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Subtract(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = Gcd(x.num, y.num);
    constexpr int64 gcdDen = Gcd(x.denom, y.denom);

    constexpr int64 num = CheckedSub<CheckedMul<x.num / gcdNum, y.denom / gcdDen>(),
                                     CheckedMul<y.num / gcdNum, x.denom / gcdDen>()>();

    constexpr Ratio<gcdNum, x.denom / gcdDen> r1;
    constexpr Ratio<num, y.denom> r2;

    return Multiply(r1, r2);
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Equal(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return x.num == y.num && x.denom == y.denom;
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto Compare(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 sx = x.num == 0 ? 0 : (x.num > 0 ? 1 : -1);
    constexpr int64 sy = y.num == 0 ? 0 : (y.num > 0 ? 1 : -1);
    if constexpr (sx != sy) {
        return sx <=> sy;
    }

    constexpr int64 qx = x.num / x.denom;
    constexpr int64 qy = y.num / y.denom;
    if constexpr (qx != qy) {
        return qx <=> qy;
    }

    constexpr int64 rx = x.num % x.denom;
    constexpr int64 ry = y.num % y.denom;
    if constexpr (rx == 0 || ry == 0) {
        return rx <=> ry;
    }
    else {
        return Compare(Ratio<y.denom, ry>{}, Ratio<x.denom, rx>{});
    }
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator+(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Add(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator-(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Subtract(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator*(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Multiply(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator/(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Divide(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval bool operator==(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Equal(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator<=>(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return Compare(x, y);
}

} // namespace mini