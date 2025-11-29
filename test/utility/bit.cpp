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

using namespace mini;
using namespace mini::bits;

template <typename T>
static constexpr int TestRotateLeft()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < digits; ++i) {
        TEST_ENSURE(RotateLeft(T(1), i) == std::rotl(T(1), i));
    }

    for (int32 i = 0; i > -digits; --i) {
        TEST_ENSURE(RotateRight(T(1), i) == std::rotr(T(1), i));
    }

    return 0;
}

template <typename T>
static constexpr int TestRotateRight()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < digits; ++i) {
        TEST_ENSURE(RotateRight(T(1), i) == std::rotr(T(1), i));
    }

    for (int32 i = 0; i > -digits; --i) {
        TEST_ENSURE(RotateRight(T(1), i) == std::rotr(T(1), i));
    }

    return 0;
}

template <typename T>
static constexpr int TestCountLeftZero()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(CountLeftZero(num) == static_cast<uint32>(std::countl_zero(num)));
    }

    for (int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(CountLeftZero(num) == static_cast<uint32>(std::countl_zero(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(CountLeftZero(max) == static_cast<uint32>(std::countl_zero(max)));

    return 0;
}

template <typename T>
static constexpr int TestCountLeftOne()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(CountLeftOne(num) == static_cast<uint32>(std::countl_one(num)));
    }

    for (int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(CountLeftOne(num) == static_cast<uint32>(std::countl_one(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(CountLeftOne(max) == static_cast<uint32>(std::countl_one(max)));

    return 0;
}

template <typename T>
static constexpr int TestCountRightZero()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(CountRightZero(num) == static_cast<uint32>(std::countr_zero(num)));
    }

    for (int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(CountRightZero(num) == static_cast<uint32>(std::countr_zero(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(CountRightZero(max) == static_cast<uint32>(std::countr_zero(max)));

    return 0;
}

template <typename T>
static constexpr int TestCountRightOne()
{
    constexpr int32 digits = static_cast<int32>(NumericLimit<T>::digits);

    for (int32 i = 0; i < 100; ++i) {
        T num = static_cast<T>(i);
        TEST_ENSURE(CountRightOne(num) == static_cast<uint32>(std::countr_one(num)));
    }

    for (int32 i = 0; i < digits; ++i) {
        T num = static_cast<T>(static_cast<T>(1) << i);
        TEST_ENSURE(CountRightOne(num) == static_cast<uint32>(std::countr_one(num)));
    }

    T max = NumericLimit<T>::max;
    TEST_ENSURE(CountRightOne(max) == static_cast<uint32>(std::countr_one(max)));

    return 0;
}

int main()
{
    TEST_BIT(TestRotateLeft, uint8);
    TEST_BIT(TestRotateLeft, uint16);
    TEST_BIT(TestRotateLeft, uint32);
    TEST_BIT(TestRotateLeft, uint64);

    TEST_BIT(TestRotateRight, uint8);
    TEST_BIT(TestRotateRight, uint16);
    TEST_BIT(TestRotateRight, uint32);
    TEST_BIT(TestRotateRight, uint64);

    TEST_BIT(TestCountLeftZero, uint8);
    TEST_BIT(TestCountLeftZero, uint16);
    TEST_BIT(TestCountLeftZero, uint32);
    TEST_BIT(TestCountLeftZero, uint64);

    TEST_BIT(TestCountLeftOne, uint8);
    TEST_BIT(TestCountLeftOne, uint16);
    TEST_BIT(TestCountLeftOne, uint32);
    TEST_BIT(TestCountLeftOne, uint64);

    TEST_BIT(TestCountRightZero, uint8);
    TEST_BIT(TestCountRightZero, uint16);
    TEST_BIT(TestCountRightZero, uint32);
    TEST_BIT(TestCountRightZero, uint64);

    TEST_BIT(TestCountRightOne, uint8);
    TEST_BIT(TestCountRightOne, uint16);
    TEST_BIT(TestCountRightOne, uint32);
    TEST_BIT(TestCountRightOne, uint64);

    return 0;
}