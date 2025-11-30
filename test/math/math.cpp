#include <algorithm>
#include <cmath>
#include <numeric>

#include "test_macro.h"

import mini.test;

using namespace mini;

template <FloatingT T>
static int TestMin(T a, T b)
{
    TEST_ENSURE(Min(a, b) == std::min(a, b));
    return 0;
}

template <FloatingT T>
static int TestMax(T a, T b)
{
    TEST_ENSURE(Max(a, b) == std::max(a, b));
    return 0;
}

template <ArithmeticT T>
static int TestAbs(T a)
{
    TEST_ENSURE(Abs(a) == std::abs(a));
    return 0;
}

template <FloatingT T>
static int TestPow(T a, T b)
{
    TEST_ENSURE(Pow(a, b) == std::pow(a, b));
    return 0;
}

template <IntegralT T, IntegralT U>
static int TestPowInt(T a, U b)
{
    TEST_ENSURE(PowInt(a, b) == static_cast<int64>(std::pow(a, b)));
    return 0;
}

template <IntegralT T>
static int TestSqrt(T a)
{
    TEST_ENSURE(Sqrt(a) == static_cast<float>(std::sqrt(a)));
    return 0;
}

template <FloatingT T>
static int TestSqrt(T a)
{
    TEST_ENSURE(Sqrt(a) == std::sqrt(a));
    return 0;
}

template <IntegralT T, IntegralT U>
static int TestGcd(T a, U b)
{
    TEST_ENSURE(Gcd(a, b) == std::gcd(a, b));
    return 0;
}

int main()
{
    TEST_ENSURE(TestMax(1.1f, 1.2f) == 0);
    TEST_ENSURE(TestMax(1.1, 1.2) == 0);

    TEST_ENSURE(TestMin(1.1f, 1.2f) == 0);
    TEST_ENSURE(TestMin(1.1, 1.2) == 0);

    TEST_ENSURE(TestAbs(1.1f) == 0);
    TEST_ENSURE(TestAbs(1.1) == 0);
    TEST_ENSURE(TestAbs(-1.1f) == 0);
    TEST_ENSURE(TestAbs(-1.1) == 0);

    TEST_ENSURE(TestPow(1.1f, 1.2f) == 0);
    TEST_ENSURE(TestPow(1.1, 1.2) == 0);
    TEST_ENSURE(TestPowInt(17, 3u) == 0);

    TEST_ENSURE(TestSqrt(2) == 0);
    TEST_ENSURE(TestSqrt(2.1f) == 0);
    TEST_ENSURE(TestSqrt(2.1) == 0);

    TEST_ENSURE(TestGcd(3u, 17ul) == 0);
    TEST_ENSURE(TestGcd(42u, 28ul) == 0);

    return 0;
}