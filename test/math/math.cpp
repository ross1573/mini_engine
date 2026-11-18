#include <cmath>
#include <numeric>

#include "test_macro.h"

import mini.test;

constexpr auto int32Max = mini::NumericLimit<mini::int32>::max;
constexpr auto int64Min = mini::NumericLimit<mini::int64>::min;
constexpr auto int64Max = mini::NumericLimit<mini::int64>::max;
constexpr auto uint64Max = mini::NumericLimit<mini::uint64>::max;

constexpr int testMin()
{
    TEST_ENSURE(mini::min(1, 2) == 1);
    TEST_ENSURE(mini::min(-1, 2) == -1);
    TEST_ENSURE(mini::min(1.5, 2.5) == 1.5);
    TEST_ENSURE(mini::min(-0.5f, 0.5f) == -0.5f);
    TEST_ENSURE(mini::min(int64Min, 0) == int64Min);
    TEST_ENSURE(mini::min(5u, 3u) == 3u);
    TEST_ENSURE(mini::min(7, 7) == 7);
    TEST_ENSURE(mini::min(static_cast<mini::int8>(-128), int64Max) == -128);
    TEST_ENSURE(mini::min(-1e300, 1e300) == -1e300);

    return 0;
}

constexpr int testMax()
{
    TEST_ENSURE(mini::max(1, 2) == 2);
    TEST_ENSURE(mini::max(-1, -2) == -1);
    TEST_ENSURE(mini::max(1.5f, 2.5) == 2.5);
    TEST_ENSURE(mini::max(-0.5, -1.5) == -0.5);
    TEST_ENSURE(mini::max(uint64Max, 0u) == uint64Max);
    TEST_ENSURE(mini::max(5u, 3u) == 5u);
    TEST_ENSURE(mini::max(-7, -7) == -7);
    TEST_ENSURE(mini::max(static_cast<mini::int8>(127), int64Min) == 127);
    TEST_ENSURE(mini::max(-1e300, 1e300) == 1e300);

    return 0;
}

constexpr int testAbsIntegral()
{
    TEST_ENSURE(mini::abs(0) == 0);
    TEST_ENSURE(mini::abs(-5) == 5);
    TEST_ENSURE(mini::abs(5) == 5);
    TEST_ENSURE(mini::abs(static_cast<mini::int8>(-127)) == 127);
    TEST_ENSURE(mini::abs(static_cast<mini::int16>(-32767)) == 32767);
    TEST_ENSURE(mini::abs(static_cast<mini::int64>(-1) * 10000000000) == 10000000000);
    TEST_ENSURE(mini::abs(int32Max) == int32Max);
    TEST_ENSURE(mini::abs(-int32Max) == int32Max);
    TEST_ENSURE(mini::abs(-int64Max) == int64Max);
    TEST_ENSURE(mini::abs(uint64Max) == uint64Max);

    return 0;
}

constexpr int testAbsFloating()
{
    TEST_ENSURE(mini::abs(-2.5) == 2.5);
    TEST_ENSURE(mini::abs(2.5f) == 2.5f);
    TEST_ENSURE(mini::abs(-0.0) == 0.0);
    TEST_ENSURE(mini::abs(-1e300) == 1e300);
    TEST_ENSURE(mini::abs(1e-300) == 1e-300);

    return 0;
}

constexpr int testPowIntegral()
{
    TEST_ENSURE(mini::pow(2, 0u) == 1);
    TEST_ENSURE(mini::pow(2, 1u) == 2);
    TEST_ENSURE(mini::pow(2, 10u) == 1024);
    TEST_ENSURE(mini::pow(-3, 3u) == -27);
    TEST_ENSURE(mini::pow(-3, 4u) == 81);
    TEST_ENSURE(mini::pow(0, 5u) == 0);
    TEST_ENSURE(mini::pow(0, 0u) == 1);
    TEST_ENSURE(mini::pow(1, 1000u) == 1);
    TEST_ENSURE(mini::pow(static_cast<mini::int64>(3), 20u) == 3486784401);
    TEST_ENSURE(mini::pow(static_cast<mini::uint64>(2), 63u) == static_cast<mini::uint64>(1) << 63);
    TEST_ENSURE(mini::pow(static_cast<mini::uint64>(3), 40u) == 12157665459056928801ull);
    TEST_ENSURE(mini::pow(-1, 7u) == -1);
    TEST_ENSURE(mini::pow(-1, 8u) == 1);
    TEST_ENSURE(mini::pow(2, 10ull) == 1024);

    return 0;
}

int testPowFloating()
{
    TEST_ENSURE(mini::pow(1.1f, 1.2f) == std::pow(1.1f, 1.2f));
    TEST_ENSURE(mini::pow(1.1, 1.2) == std::pow(1.1, 1.2));
    TEST_ENSURE(mini::pow(2.0, -1.0) == std::pow(2.0, -1.0));

    return 0;
}

int testSqrtIntegral()
{
    TEST_ENSURE(mini::sqrt(0) == static_cast<float>(std::sqrt(0)));
    TEST_ENSURE(mini::sqrt(2) == static_cast<float>(std::sqrt(2)));
    TEST_ENSURE(mini::sqrt(1000000u) == static_cast<float>(std::sqrt(1000000u)));
    TEST_ENSURE(mini::sqrt(16) == 4.f);
    TEST_ENSURE(mini::sqrt(static_cast<mini::uint64>(1) << 62) == 2147483648.f);

    return 0;
}

int testSqrtFloating()
{
    TEST_ENSURE(mini::sqrt(2.1f) == std::sqrt(2.1f));
    TEST_ENSURE(mini::sqrt(2.1) == std::sqrt(2.1));
    TEST_ENSURE(mini::sqrt(6.25) == 2.5);
    TEST_ENSURE(mini::sqrt(0.25f) == 0.5f);

    return 0;
}

constexpr int testGcd()
{
    TEST_ENSURE(mini::gcd(0, 0) == 0);
    TEST_ENSURE(mini::gcd(0, 7) == 7);
    TEST_ENSURE(mini::gcd(7, 0) == 7);
    TEST_ENSURE(mini::gcd(1, 7) == 1);
    TEST_ENSURE(mini::gcd(12, 18) == 6);
    TEST_ENSURE(mini::gcd(-12, 18) == 6);
    TEST_ENSURE(mini::gcd(12, -18) == 6);
    TEST_ENSURE(mini::gcd(-12, -18) == 6);
    TEST_ENSURE(mini::gcd(17, 17) == 17);
    TEST_ENSURE(mini::gcd(1024, 96) == 32);
    TEST_ENSURE(mini::gcd(12ll, 18) == 6);
    TEST_ENSURE(mini::gcd(int64Max, int64Max) == int64Max);
    TEST_ENSURE(mini::gcd(uint64Max, 3u) == 3u);
    TEST_ENSURE(mini::gcd(uint64Max, uint64Max) == uint64Max);
    TEST_ENSURE(mini::gcd(2147483646, 1073741823) == std::gcd(2147483646, 1073741823));

    TEST_ENSURE(mini::gcd(static_cast<mini::uint64>(1) << 63, static_cast<mini::uint64>(1) << 40) ==
                static_cast<mini::uint64>(1) << 40);
    TEST_ENSURE(mini::gcd(static_cast<mini::int64>(1) << 40, static_cast<mini::int64>(3) << 38) ==
                static_cast<mini::int64>(1) << 38);
    TEST_ENSURE(mini::gcd(static_cast<mini::uint16>(48), static_cast<mini::uint16>(180)) ==
                std::gcd(static_cast<mini::uint16>(48), static_cast<mini::uint16>(180)));

    for (mini::int32 a = -30; a <= 30; ++a) {
        for (mini::int32 b = -30; b <= 30; ++b) {
            TEST_ENSURE(mini::gcd(a, b) == std::gcd(a, b));
        }
    }

    return 0;
}

constexpr int testLcm()
{
    TEST_ENSURE(mini::lcm(0, 0) == 0);
    TEST_ENSURE(mini::lcm(0, 5) == 0);
    TEST_ENSURE(mini::lcm(5, 0) == 0);
    TEST_ENSURE(mini::lcm(1, 5) == 5);
    TEST_ENSURE(mini::lcm(4, 6) == 12);
    TEST_ENSURE(mini::lcm(-4, 6) == 12);
    TEST_ENSURE(mini::lcm(4, -6) == 12);
    TEST_ENSURE(mini::lcm(7, 13) == 91);
    TEST_ENSURE(mini::lcm(12, 12) == 12);
    TEST_ENSURE(mini::lcm(-4, -6) == 12);
    TEST_ENSURE(mini::lcm(6u, 4u) == 12u);
    TEST_ENSURE(mini::lcm(int64Max, 1) == int64Max);
    TEST_ENSURE(mini::lcm(int64Max, int64Max) == int64Max);
    TEST_ENSURE(mini::lcm(2147483646, 1073741823) == std::lcm(2147483646, 1073741823));

    TEST_ENSURE(mini::lcm(static_cast<mini::int64>(1) << 40, static_cast<mini::int64>(1) << 41) ==
                static_cast<mini::int64>(1) << 41);
    TEST_ENSURE(mini::lcm(static_cast<mini::uint16>(48), static_cast<mini::uint16>(180)) ==
                std::lcm(static_cast<mini::uint16>(48), static_cast<mini::uint16>(180)));

    for (mini::int32 a = -30; a <= 30; ++a) {
        for (mini::int32 b = -30; b <= 30; ++b) {
            TEST_ENSURE(mini::lcm(a, b) == std::lcm(a, b));
        }
    }

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testMin);
    TEST_CONSTEXPR(testMax);
    TEST_CONSTEXPR(testAbsIntegral);
    TEST_CONSTEXPR(testAbsFloating);
    TEST_CONSTEXPR(testPowIntegral);
    TEST_FUNCTION(testPowFloating);
    TEST_FUNCTION(testSqrtIntegral);
    TEST_FUNCTION(testSqrtFloating);
    TEST_CONSTEXPR(testGcd);
    TEST_CONSTEXPR(testLcm);

    return 0;
}
