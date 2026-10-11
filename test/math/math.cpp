#include <algorithm>
#include <cmath>
#include <numeric>

#include "test_macro.h"

import mini.test;

template <mini::FloatingT T>
int TestMin(T a, T b)
{
    TEST_ENSURE(mini::min(a, b) == std::min(a, b));
    return 0;
}

template <mini::FloatingT T>
int TestMax(T a, T b)
{
    TEST_ENSURE(mini::max(a, b) == std::max(a, b));
    return 0;
}

template <mini::ArithmeticT T>
int TestAbs(T a)
{
    TEST_ENSURE(mini::abs(a) == std::abs(a));
    return 0;
}

template <mini::FloatingT T>
int TestPow(T a, T b)
{
    TEST_ENSURE(mini::pow(a, b) == std::pow(a, b));
    return 0;
}

template <mini::IntegralT T, mini::IntegralT U>
int TestPowInt(T a, U b)
{
    TEST_ENSURE(mini::pow(a, b) == static_cast<mini::int64>(std::pow(a, b)));
    return 0;
}

template <mini::IntegralT T>
int TestSqrt(T a)
{
    TEST_ENSURE(mini::sqrt(a) == static_cast<float>(std::sqrt(a)));
    return 0;
}

template <mini::FloatingT T>
int TestSqrt(T a)
{
    TEST_ENSURE(mini::sqrt(a) == std::sqrt(a));
    return 0;
}

template <mini::IntegralT T, mini::IntegralT U>
int TestGcd(T a, U b)
{
    TEST_ENSURE(mini::gcd(a, b) == std::gcd(a, b));
    return 0;
}

template <mini::IntegralT T, mini::IntegralT U>
int TestLcm(T a, U b)
{
    TEST_ENSURE(mini::lcm(a, b) == std::lcm(a, b));
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

    TEST_ENSURE(TestGcd(3, 17) == 0);
    TEST_ENSURE(TestGcd(42, 28) == 0);

    TEST_ENSURE(TestLcm(12, 15) == 0);
    TEST_ENSURE(TestLcm(3, 4) == 0);

    return 0;
}