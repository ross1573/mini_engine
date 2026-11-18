#include "test_macro.h"

import mini.test;

using mini::Ratio;
using mini::RatioAddT;
using mini::RatioDivideT;
using mini::RatioMultiplyT;
using mini::RatioSubtractT;

constexpr auto int64Max = mini::NumericLimit<mini::int64>::max;
constexpr auto int64Min = mini::NumericLimit<mini::int64>::min;

template <typename R, mini::int64 NumN, mini::int64 DenomN>
consteval void ensureReduced()
{
    static_assert(R::num == NumN, "numerator does not match");
    static_assert(R::denom == DenomN, "denominator does not match");
}

template <typename R1, typename R2, typename Result>
consteval void ensureAdd()
{
    auto result = R1{} + R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(mini::SameAsT<RatioAddT<R1, R2>, Result>);
    static_assert(mini::SameAsT<RatioAddT<R1, R2>, RatioAddT<R2, R1>>);
}

template <typename R1, typename R2, typename Result>
consteval void ensureSubtract()
{
    auto result = R1{} - R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(mini::SameAsT<RatioSubtractT<R1, R2>, Result>);
    static_assert(RatioAddT<RatioSubtractT<R1, R2>, R2>{} == R1{});
}

template <typename R1, typename R2, typename Result>
consteval void ensureMultiply()
{
    auto result = R1{} * R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(mini::SameAsT<RatioMultiplyT<R1, R2>, Result>);
    static_assert(mini::SameAsT<RatioMultiplyT<R1, R2>, RatioMultiplyT<R2, R1>>);
}

template <typename R1, typename R2, typename Result>
consteval void ensureDivide()
{
    auto result = R1{} / R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(mini::SameAsT<RatioDivideT<R1, R2>, Result>);
    static_assert(RatioMultiplyT<RatioDivideT<R1, R2>, R2>{} == R1{});
}

[[maybe_unused]] consteval void testReduce()
{
    ensureReduced<Ratio<1>, 1, 1>();
    ensureReduced<Ratio<0>, 0, 1>();
    ensureReduced<Ratio<0, 5>, 0, 1>();
    ensureReduced<Ratio<0, -5>, 0, 1>();
    ensureReduced<Ratio<4, 6>, 2, 3>();
    ensureReduced<Ratio<-4, 6>, -2, 3>();
    ensureReduced<Ratio<4, -6>, -2, 3>();
    ensureReduced<Ratio<-4, -6>, 2, 3>();
    ensureReduced<Ratio<7, 7>, 1, 1>();
    ensureReduced<Ratio<1000000000, 1000>, 1000000, 1>();
    ensureReduced<Ratio<3600, 86400>, 1, 24>();
    ensureReduced<Ratio<17, 13>, 17, 13>();
    ensureReduced<Ratio<int64Max, int64Max>, 1, 1>();
    ensureReduced<Ratio<1, int64Max>, 1, int64Max>();
    ensureReduced<Ratio<-int64Max, int64Max>, -1, 1>();
}

[[maybe_unused]] consteval void testAdd()
{
    ensureAdd<Ratio<2, 3>, Ratio<3, 5>, Ratio<19, 15>>();
    ensureAdd<Ratio<2, 3>, Ratio<4, 3>, Ratio<2, 1>>();
    ensureAdd<Ratio<1, 3>, Ratio<7, 6>, Ratio<3, 2>>();
    ensureAdd<Ratio<1, 2>, Ratio<-1, 2>, Ratio<0, 1>>();
    ensureAdd<Ratio<0>, Ratio<5, 7>, Ratio<5, 7>>();
    ensureAdd<Ratio<-1, 3>, Ratio<-1, 6>, Ratio<-1, 2>>();
    ensureAdd<Ratio<4, 6>, Ratio<2, -4>, Ratio<1, 6>>();
    ensureAdd<Ratio<1, 1000>, Ratio<1, 1000000>, Ratio<1001, 1000000>>();
    ensureAdd<Ratio<int64Max, 2>, Ratio<int64Min + 2, 2>, Ratio<1, 2>>();
    ensureAdd<Ratio<1, int64Max>, Ratio<1, int64Max>, Ratio<2, int64Max>>();
}

[[maybe_unused]] consteval void testSubtract()
{
    ensureSubtract<Ratio<2, 3>, Ratio<3, 5>, Ratio<1, 15>>();
    ensureSubtract<Ratio<2, 3>, Ratio<4, 3>, Ratio<-2, 3>>();
    ensureSubtract<Ratio<1, 3>, Ratio<-7, 6>, Ratio<3, 2>>();
    ensureSubtract<Ratio<5, 7>, Ratio<5, 7>, Ratio<0, 1>>();
    ensureSubtract<Ratio<0>, Ratio<5, 7>, Ratio<-5, 7>>();
    ensureSubtract<Ratio<-1, 2>, Ratio<1, 2>, Ratio<-1, 1>>();
    ensureSubtract<Ratio<60>, Ratio<1, 1000>, Ratio<59999, 1000>>();
    ensureSubtract<Ratio<int64Max, 2>, Ratio<int64Max - 1, 2>, Ratio<1, 2>>();
    ensureSubtract<Ratio<-3, 4>, Ratio<-5, 6>, Ratio<1, 12>>();
}

[[maybe_unused]] consteval void testMultiply()
{
    ensureMultiply<Ratio<2, 3>, Ratio<3, 5>, Ratio<2, 5>>();
    ensureMultiply<Ratio<2, 3>, Ratio<5, 3>, Ratio<10, 9>>();
    ensureMultiply<Ratio<2, 3>, Ratio<0>, Ratio<0, 1>>();
    ensureMultiply<Ratio<2, 3>, Ratio<1>, Ratio<2, 3>>();
    ensureMultiply<Ratio<-2, 3>, Ratio<3, -2>, Ratio<1, 1>>();
    ensureMultiply<Ratio<-2, 3>, Ratio<3, 2>, Ratio<-1, 1>>();
    ensureMultiply<Ratio<60>, Ratio<1, 1000>, Ratio<3, 50>>();
    ensureMultiply<Ratio<int64Max, 3>, Ratio<3, int64Max>, Ratio<1, 1>>();
}

[[maybe_unused]] consteval void testDivide()
{
    ensureDivide<Ratio<2, 3>, Ratio<5, 3>, Ratio<2, 5>>();
    ensureDivide<Ratio<2, 3>, Ratio<3, 5>, Ratio<10, 9>>();
    ensureDivide<Ratio<2, 3>, Ratio<2, 3>, Ratio<1, 1>>();
    ensureDivide<Ratio<0>, Ratio<2, 3>, Ratio<0, 1>>();
    ensureDivide<Ratio<-2, 3>, Ratio<2, 3>, Ratio<-1, 1>>();
    ensureDivide<Ratio<3600>, Ratio<60>, Ratio<60, 1>>();
    ensureDivide<Ratio<1, 1000>, Ratio<1, 1000000000>, Ratio<1000000, 1>>();
    ensureDivide<Ratio<1>, Ratio<-3, 4>, Ratio<-4, 3>>();
    ensureDivide<Ratio<-3, 4>, Ratio<-3, 8>, Ratio<2, 1>>();
}

[[maybe_unused]] consteval void testEqual()
{
    static_assert(Ratio<0, 3>{} == Ratio<0, 6>{});
    static_assert(Ratio<2, 4>{} == Ratio<1, 2>{});
    static_assert(Ratio<-1, 2>{} == Ratio<1, -2>{});
    static_assert(Ratio<-2, -3>{} == Ratio<2, 3>{});
}

[[maybe_unused]] consteval void testNotEqual()
{
    static_assert(Ratio<1, 2>{} != Ratio<1, 3>{});
    static_assert(Ratio<1, 2>{} != Ratio<-1, 2>{});
    static_assert(Ratio<0>{} != Ratio<1, int64Max>{});
    static_assert(Ratio<int64Max>{} != Ratio<int64Max - 1>{});
}

[[maybe_unused]] consteval void testLess()
{
    static_assert(Ratio<-2, 3>{} < Ratio<3, 5>{});
    static_assert(Ratio<1, 3>{} < Ratio<7, 6>{});
    static_assert(Ratio<0, 3>{} < Ratio<7, 6>{});
    static_assert(Ratio<-5, 3>{} < Ratio<-3, 2>{});
    static_assert(Ratio<7, 3>{} < Ratio<5, 2>{});
    static_assert(Ratio<1, int64Max>{} < Ratio<1, int64Max - 1>{});
    static_assert(Ratio<int64Max - 1, int64Max>{} < Ratio<1>{});
    static_assert(!(Ratio<1, 2>{} < Ratio<2, 4>{}));
    static_assert(Ratio<-7, 2>{} < Ratio<-10, 3>{});
    static_assert(Ratio<-5, 2>{} < Ratio<-2>{});
    static_assert(Ratio<2>{} < Ratio<5, 2>{});
    static_assert(!(Ratio<-1, 2>{} < Ratio<-1, 2>{}));
}

[[maybe_unused]] consteval void testGreater()
{
    static_assert(Ratio<2, 3>{} > Ratio<3, 5>{});
    static_assert(Ratio<0, 3>{} > Ratio<-7, 6>{});
    static_assert(Ratio<13, 8>{} > Ratio<8, 5>{});
    static_assert(Ratio<int64Max, 1>{} > Ratio<int64Max, 2>{});
    static_assert(Ratio<int64Max, 1>{} > Ratio<int64Min + 1, 1>{});
    static_assert(Ratio<1, int64Max>{} > Ratio<0>{});
    static_assert(!(Ratio<1, 2>{} > Ratio<2, 4>{}));
    static_assert(Ratio<-5, 3>{} > Ratio<-7, 4>{});
    static_assert(Ratio<-2>{} > Ratio<-5, 2>{});
    static_assert(Ratio<-1, int64Max>{} > Ratio<-1, int64Max - 1>{});
}

[[maybe_unused]] consteval void testLessEqual()
{
    static_assert(Ratio<1, 2>{} <= Ratio<2, 4>{});
    static_assert(Ratio<1, 3>{} <= Ratio<1, 2>{});
    static_assert(Ratio<-1, 2>{} <= Ratio<0>{});
    static_assert(!(Ratio<1, 2>{} <= Ratio<1, 3>{}));
}

[[maybe_unused]] consteval void testGreaterEqual()
{
    static_assert(Ratio<1, 2>{} >= Ratio<2, 4>{});
    static_assert(Ratio<1, 2>{} >= Ratio<1, 3>{});
    static_assert(Ratio<0>{} >= Ratio<-1, 2>{});
    static_assert(!(Ratio<1, 3>{} >= Ratio<1, 2>{}));
}

[[maybe_unused]] consteval void testThreeWay()
{
    static_assert((Ratio<1, 2>{} <=> Ratio<2, 4>{}) == 0);
    static_assert((Ratio<1, 3>{} <=> Ratio<1, 2>{}) < 0);
    static_assert((Ratio<-1, 3>{} <=> Ratio<-1, 2>{}) > 0);
    static_assert((Ratio<int64Max, 1>{} <=> Ratio<int64Min + 1, 1>{}) > 0);
}

int main()
{
    return 0;
}
