#include <bit>

#include "test_macro.h"

import mini.test;

#ifdef DEBUG_TEST_BIT
#  define TEST_BIT(func, type) TEST_ENSURE((func<type>() == 0));
#else
#  define TEST_BIT(func, type)          \
      static_assert(func<type>() == 0); \
      TEST_ENSURE((func<type>() == 0));
#endif

using mini::NumericLimit;
using mini::bit::countLeftOne;
using mini::bit::countLeftZero;
using mini::bit::countRightOne;
using mini::bit::countRightZero;
using mini::bit::rotateLeft;
using mini::bit::rotateRight;

template <typename T>
constexpr int TestRotateLeft()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < digits; ++i) {
        TEST_ENSURE(rotateLeft(T(1), i) == std::rotl(T(1), i));
    }

    for (mini::int32 i = 0; i > -digits; --i) {
        TEST_ENSURE(rotateRight(T(1), i) == std::rotr(T(1), i));
    }

    return 0;
}

template <typename T>
constexpr int TestRotateRight()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < digits; ++i) {
        TEST_ENSURE(rotateRight(T(1), i) == std::rotr(T(1), i));
    }

    for (mini::int32 i = 0; i > -digits; --i) {
        TEST_ENSURE(rotateRight(T(1), i) == std::rotr(T(1), i));
    }

    return 0;
}

template <typename T>
constexpr int TestCountLeftZero()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(countLeftZero(num) == static_cast<mini::uint32>(std::countl_zero(num)));
    }

    for (mini::int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(countLeftZero(num) == static_cast<mini::uint32>(std::countl_zero(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(countLeftZero(max) == static_cast<mini::uint32>(std::countl_zero(max)));

    return 0;
}

template <typename T>
constexpr int TestCountLeftOne()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(countLeftOne(num) == static_cast<mini::uint32>(std::countl_one(num)));
    }

    for (mini::int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(countLeftOne(num) == static_cast<mini::uint32>(std::countl_one(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(countLeftOne(max) == static_cast<mini::uint32>(std::countl_one(max)));

    return 0;
}

template <typename T>
constexpr int TestCountRightZero()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(countRightZero(num) == static_cast<mini::uint32>(std::countr_zero(num)));
    }

    for (mini::int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(countRightZero(num) == static_cast<mini::uint32>(std::countr_zero(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(countRightZero(max) == static_cast<mini::uint32>(std::countr_zero(max)));

    return 0;
}

template <typename T>
constexpr int TestCountRightOne()
{
    constexpr mini::int32 digits = static_cast<mini::int32>(NumericLimit<T>::digits);

    for (mini::int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(countRightOne(num) == static_cast<mini::uint32>(std::countr_one(num)));
    }

    for (mini::int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(countRightOne(num) == static_cast<mini::uint32>(std::countr_one(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(countRightOne(max) == static_cast<mini::uint32>(std::countr_one(max)));

    return 0;
}

int main()
{
    TEST_BIT(TestRotateLeft, mini::uint8);
    TEST_BIT(TestRotateLeft, mini::uint16);
    TEST_BIT(TestRotateLeft, mini::uint32);
    TEST_BIT(TestRotateLeft, mini::uint64);

    TEST_BIT(TestRotateRight, mini::uint8);
    TEST_BIT(TestRotateRight, mini::uint16);
    TEST_BIT(TestRotateRight, mini::uint32);
    TEST_BIT(TestRotateRight, mini::uint64);

    TEST_BIT(TestCountLeftZero, mini::uint8);
    TEST_BIT(TestCountLeftZero, mini::uint16);
    TEST_BIT(TestCountLeftZero, mini::uint32);
    TEST_BIT(TestCountLeftZero, mini::uint64);

    TEST_BIT(TestCountLeftOne, mini::uint8);
    TEST_BIT(TestCountLeftOne, mini::uint16);
    TEST_BIT(TestCountLeftOne, mini::uint32);
    TEST_BIT(TestCountLeftOne, mini::uint64);

    TEST_BIT(TestCountRightZero, mini::uint8);
    TEST_BIT(TestCountRightZero, mini::uint16);
    TEST_BIT(TestCountRightZero, mini::uint32);
    TEST_BIT(TestCountRightZero, mini::uint64);

    TEST_BIT(TestCountRightOne, mini::uint8);
    TEST_BIT(TestCountRightOne, mini::uint16);
    TEST_BIT(TestCountRightOne, mini::uint32);
    TEST_BIT(TestCountRightOne, mini::uint64);

    return 0;
}