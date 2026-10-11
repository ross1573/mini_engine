export module mini.core:ratio;

import :type;
import :math_base;
import :math_operation;

namespace mini {

export template <int64 NumN, int64 DenomN = 1>
class Ratio;

template <typename T>
constexpr bool isRatioT = false;

template <int64 NumN, int64 DenomN>
constexpr bool isRatioT<Ratio<NumN, DenomN>> = true;

export template <typename T>
concept RatioT = isRatioT<T>;

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

    static constexpr int64 sign(int64 val) { return val == 0 ? 0 : (val < 0 ? -1 : 1); }

    static_assert(abs(NumN) >= 0, "Numerator is out of range");
    static_assert(abs(DenomN) >= 0, "Denominator is out of range");
    static_assert(DenomN != 0, "integer division by zero");

    static constexpr int64 unum = abs(NumN);
    static constexpr int64 uden = abs(DenomN);
    static constexpr int64 flag = sign(NumN) * sign(DenomN);
    static constexpr int64 g = gcd(unum, uden);

public:
    static constexpr int64 num = flag * unum / g;
    static constexpr int64 denom = uden / g;
};

template <int64 LhsN, int64 RhsN>
consteval int64 checkedAdd()
{
    constexpr uint64 ux = static_cast<uint64>(LhsN);
    constexpr uint64 uy = static_cast<uint64>(RhsN);
    constexpr int64 result = static_cast<int64>(ux + uy);

    if constexpr (LhsN > 0 && RhsN > 0) {
        static_assert(result > 0, "integer add overflow");
    } else if constexpr (LhsN < 0 && RhsN < 0) {
        static_assert(result < 0, "integer add overflow");
    }

    return result;
}

template <int64 LhsN, int64 RhsN>
consteval int64 checkedSub()
{
    constexpr uint64 ux = static_cast<uint64>(LhsN);
    constexpr uint64 uy = static_cast<uint64>(RhsN);
    constexpr int64 result = static_cast<int64>(ux - uy);

    if constexpr (LhsN <= 0 && RhsN > 0) {
        static_assert(result < 0, "integer sub overflow");
    } else if constexpr (LhsN >= 0 && RhsN < 0) {
        static_assert(result > 0, "integer sub overflow");
    }

    return result;
}

template <int64 LhsN, int64 RhsN>
consteval int64 checkedMul()
{
    constexpr uint64 ux = LhsN < 0 ? (0 - static_cast<uint64>(LhsN)) : static_cast<uint64>(LhsN);
    constexpr uint64 uy = RhsN < 0 ? (0 - static_cast<uint64>(RhsN)) : static_cast<uint64>(RhsN);
    constexpr uint64 ur = ux * uy;

    constexpr bool isNeg = (LhsN < 0) ^ (RhsN < 0);
    constexpr int64 result = static_cast<int64>(isNeg ? (0 - ur) : ur);

    if constexpr (ux != 0 && uy != 0) {
        constexpr uint64 max = static_cast<uint64>(NumericLimit<int64>::max);
        constexpr uint64 s = isNeg ? max + static_cast<int64>(1) : max;
        static_assert(ux <= s / uy, "integer mul overflow");
    }

    return result;
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto multiply(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcd1 = gcd(x.num, y.denom);
    constexpr int64 gcd2 = gcd(x.denom, y.num);

    constexpr int64 num = checkedMul<x.num / gcd1, y.num / gcd2>();
    constexpr int64 den = checkedMul<y.denom / gcd1, x.denom / gcd2>();

    return Ratio<num, den>{};
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto divide(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = gcd(x.num, y.num);
    constexpr int64 gcdDen = gcd(x.denom, y.denom);

    constexpr int64 num = checkedMul<x.num / gcdNum, y.denom / gcdDen>();
    constexpr int64 den = checkedMul<y.num / gcdNum, x.denom / gcdDen>();

    return Ratio<num, den>{};
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto add(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = gcd(x.num, y.num);
    constexpr int64 gcdDen = gcd(x.denom, y.denom);

    constexpr int64 num =
        checkedAdd<checkedMul<x.num / gcdNum, y.denom / gcdDen>(), checkedMul<y.num / gcdNum, x.denom / gcdDen>()>();

    constexpr Ratio<gcdNum, x.denom / gcdDen> r1;
    constexpr Ratio<num, y.denom> r2;

    return multiply(r1, r2);
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto subtract(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    constexpr int64 gcdNum = gcd(x.num, y.num);
    constexpr int64 gcdDen = gcd(x.denom, y.denom);

    constexpr int64 num =
        checkedSub<checkedMul<x.num / gcdNum, y.denom / gcdDen>(), checkedMul<y.num / gcdNum, x.denom / gcdDen>()>();

    constexpr Ratio<gcdNum, x.denom / gcdDen> r1;
    constexpr Ratio<num, y.denom> r2;

    return multiply(r1, r2);
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto equal(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return x.num == y.num && x.denom == y.denom;
}

template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto compare(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
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
    } else {
        return compare(Ratio<y.denom, ry>{}, Ratio<x.denom, rx>{});
    }
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator+(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return add(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator-(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return subtract(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator*(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return multiply(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator/(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return divide(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval bool operator==(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return equal(x, y);
}

export template <int64 NumN, int64 DenomN, int64 NumU, int64 DenomU>
consteval auto operator<=>(Ratio<NumN, DenomN> x, Ratio<NumU, DenomU> y)
{
    return compare(x, y);
}

} // namespace mini