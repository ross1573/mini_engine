#include "test_macro.h"

import mini.test;
import mini.math;

using namespace mini;

template <typename R1, typename R2, typename Result>
consteval void TestAdd()
{
    auto result = R1{} + R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(SameAsT<RatioAddT<R1, R2>, Result>);
}

template <typename R1, typename R2, typename Result>
consteval void TestSub()
{
    auto result = R1{} - R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(SameAsT<RatioSubtractT<R1, R2>, Result>);
}

template <typename R1, typename R2, typename Result>
consteval void TestMul()
{
    auto result = R1{} * R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(SameAsT<RatioMultiplyT<R1, R2>, Result>);
}

template <typename R1, typename R2, typename Result>
consteval void TestDiv()
{
    auto result = R1{} / R2{};
    static_assert(result.num == Result::num, "numerator does not match");
    static_assert(result.denom == Result::denom, "denominator does not match");
    static_assert(SameAsT<RatioDivideT<R1, R2>, Result>);
}

int main()
{
    TestAdd<Ratio<2, 3>, Ratio<3, 5>, Ratio<19, 15>>();
    TestAdd<Ratio<2, 3>, Ratio<4, 3>, Ratio<2, 1>>();
    TestAdd<Ratio<1, 3>, Ratio<7, 6>, Ratio<3, 2>>();

    TestSub<Ratio<2, 3>, Ratio<3, 5>, Ratio<1, 15>>();
    TestSub<Ratio<2, 3>, Ratio<4, 3>, Ratio<-2, 3>>();
    TestSub<Ratio<1, 3>, Ratio<-7, 6>, Ratio<3, 2>>();

    TestMul<Ratio<2, 3>, Ratio<3, 5>, Ratio<2, 5>>();
    TestMul<Ratio<2, 3>, Ratio<5, 3>, Ratio<10, 9>>();

    TestDiv<Ratio<2, 3>, Ratio<5, 3>, Ratio<2, 5>>();
    TestDiv<Ratio<2, 3>, Ratio<3, 5>, Ratio<10, 9>>();

    static_assert(Ratio<2, 3>{} > Ratio<3, 5>{});
    static_assert(Ratio<-2, 3>{} < Ratio<3, 5>{});
    static_assert(Ratio<-2, -3>{} > Ratio<3, 5>{});
    static_assert(Ratio<4, 6>{} > Ratio<6, 10>{});
    static_assert(Ratio<-4, 6>{} < Ratio<6, 10>{});
    static_assert(Ratio<-4, -6>{} > Ratio<6, 10>{});
    static_assert(Ratio<1, 3>{} < Ratio<7, 6>{});
    static_assert(Ratio<0, 3>{} < Ratio<7, 6>{});
    static_assert(Ratio<0, 3>{} > Ratio<-7, 6>{});
    static_assert(Ratio<0, 3>{} == Ratio<0, 6>{});

    constexpr auto int64Max = NumericLimit<int64>::max;
    constexpr auto int64Min = NumericLimit<int64>::min;
    TestAdd<Ratio<int64Max, 2>, Ratio<int64Min + 2, 2>, Ratio<1, 2>>();
    TestSub<Ratio<int64Max, 2>, Ratio<int64Max - 1, 2>, Ratio<1, 2>>();
    TestMul<Ratio<int64Max, 1>, Ratio<1, 1>, Ratio<int64Max, 1>>();

    static_assert(Ratio<int64Max, 1>{} > Ratio<int64Max, 2>{});
    static_assert(Ratio<int64Max, 1>{} > Ratio<int64Min + 1, 1>{});

    return 0;
}