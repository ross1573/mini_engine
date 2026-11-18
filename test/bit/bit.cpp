#include <bit>

#include "test_macro.h"

import mini.test;

using mini::bit::bitCast;
using mini::bit::countLeftOne;
using mini::bit::countLeftZero;
using mini::bit::countRightOne;
using mini::bit::countRightZero;
using mini::bit::rotateLeft;
using mini::bit::rotateRight;

constexpr auto int32Max = mini::NumericLimit<mini::int32>::max;
constexpr auto int32Min = mini::NumericLimit<mini::int32>::min;

template <typename T>
constexpr mini::int32 digitsOf()
{
    return static_cast<mini::int32>(mini::NumericLimit<T>::digits);
}

template <typename T>
constexpr T repeat(mini::uint8 byte)
{
    T result = 0;
    for (mini::size_t i = 0; i < sizeof(T); ++i) {
        result = static_cast<T>(static_cast<T>(result << 4) << 4);
        result = static_cast<T>(result | byte);
    }

    return result;
}

template <typename T>
constexpr T pattern(mini::int32 index)
{
    switch (index) {
        case 0:  return T(0);
        case 1:  return T(1);
        case 2:  return mini::NumericLimit<T>::max;
        case 3:  return static_cast<T>(mini::NumericLimit<T>::max - 1);
        case 4:  return static_cast<T>(T(1) << (digitsOf<T>() - 1));
        case 5:  return repeat<T>(0x55);
        case 6:  return repeat<T>(0xAA);
        case 7:  return repeat<T>(0x0F);
        case 8:  return repeat<T>(0xF0);
        case 9:  return static_cast<T>(0x8001u);
        default: return static_cast<T>(0xA5C3u);
    }
}

constexpr mini::int32 patternCount = 11;

template <typename T, typename FuncT, typename RefFuncT>
constexpr bool matches(FuncT func, RefFuncT ref)
{
    auto matches = [&](T num) { return func(num) == static_cast<mini::uint32>(ref(num)); };

    for (mini::int32 i = 0; i < 256; ++i) {
        if (!matches(static_cast<T>(i))) {
            return false;
        }
    }

    for (mini::int32 i = 0; i < patternCount; ++i) {
        if (!matches(pattern<T>(i))) {
            return false;
        }
    }

    for (mini::int32 i = 0; i < digitsOf<T>(); ++i) {
        T bit = static_cast<T>(T(1) << i);
        T bitInverse = static_cast<T>(~bit);
        if (!matches(bit) || !matches(bitInverse)) {
            return false;
        }

        T lowMask = static_cast<T>(bit - 1);
        T lowMaskInverse = static_cast<T>(~lowMask);
        if (!matches(lowMask) || !matches(lowMaskInverse)) {
            return false;
        }
    }

    return true;
}

template <typename T>
constexpr int testRotateLeft()
{
    constexpr mini::int32 digits = digitsOf<T>();

    for (mini::int32 count = 0; count < patternCount; ++count) {
        T num = pattern<T>(count);
        for (mini::int32 i = -2 * digits; i <= 2 * digits; ++i) {
            TEST_ENSURE(rotateLeft(num, i) == std::rotl(num, i));
        }

        TEST_ENSURE(rotateLeft(num, int32Max) == std::rotl(num, int32Max));
        TEST_ENSURE(rotateLeft(num, int32Min) == std::rotl(num, int32Min));
    }

    return 0;
}

template <typename T>
constexpr int testRotateRight()
{
    constexpr mini::int32 digits = digitsOf<T>();

    for (mini::int32 count = 0; count < patternCount; ++count) {
        T num = pattern<T>(count);
        for (mini::int32 i = -2 * digits; i <= 2 * digits; ++i) {
            TEST_ENSURE(rotateRight(num, i) == std::rotr(num, i));
        }

        TEST_ENSURE(rotateRight(num, int32Max) == std::rotr(num, int32Max));
        TEST_ENSURE(rotateRight(num, int32Min) == std::rotr(num, int32Min));
    }

    return 0;
}

template <typename T>
constexpr int testCountLeftZero()
{
    TEST_ENSURE(matches<T>([](T num) { return countLeftZero(num); }, [](T num) { return std::countl_zero(num); }));

    return 0;
}

template <typename T>
constexpr int testCountLeftOne()
{
    TEST_ENSURE(matches<T>([](T num) { return countLeftOne(num); }, [](T num) { return std::countl_one(num); }));

    return 0;
}

template <typename T>
constexpr int testCountRightZero()
{
    TEST_ENSURE(matches<T>([](T num) { return countRightZero(num); }, [](T num) { return std::countr_zero(num); }));

    return 0;
}

template <typename T>
constexpr int testCountRightOne()
{
    TEST_ENSURE(matches<T>([](T num) { return countRightOne(num); }, [](T num) { return std::countr_one(num); }));

    return 0;
}

constexpr int testBitCast()
{
    TEST_ENSURE(bitCast<mini::uint32>(1.f) == 0x3F800000u);
    TEST_ENSURE(bitCast<mini::uint32>(-2.5f) == 0xC0200000u);
    TEST_ENSURE(bitCast<float>(0x3F800000u) == 1.f);
    TEST_ENSURE(bitCast<mini::uint64>(1.0) == 0x3FF0000000000000ull);
    TEST_ENSURE(bitCast<double>(0xC004000000000000ull) == -2.5);

    float floats[] = {0.f, -0.f, 1.f, -1.5f, 3.14159f, 1e-30f, 6.5e30f};
    for (float f : floats) {
        TEST_ENSURE(bitCast<mini::uint32>(f) == std::bit_cast<mini::uint32>(f));
        TEST_ENSURE(bitCast<float>(bitCast<mini::uint32>(f)) == f);
    }

    double doubles[] = {0.0, -0.0, 1.0, -1.5, 3.141592653589793, 1e-300, 6.5e300};
    for (double d : doubles) {
        TEST_ENSURE(bitCast<mini::uint64>(d) == std::bit_cast<mini::uint64>(d));
        TEST_ENSURE(bitCast<double>(bitCast<mini::uint64>(d)) == d);
    }

    for (mini::int32 i = 0; i < patternCount; ++i) {
        mini::uint32 u32 = pattern<mini::uint32>(i);
        mini::uint64 u64 = pattern<mini::uint64>(i);
        TEST_ENSURE(bitCast<mini::uint32>(bitCast<float>(u32)) == u32);
        TEST_ENSURE(bitCast<mini::uint64>(bitCast<double>(u64)) == u64);
        TEST_ENSURE(bitCast<mini::uint32>(bitCast<mini::int32>(u32)) == u32);
        TEST_ENSURE(bitCast<mini::int64>(u64) == std::bit_cast<mini::int64>(u64));
    }

    TEST_ENSURE(bitCast<mini::int32>(0xFFFFFFFFu) == -1);
    TEST_ENSURE(bitCast<mini::uint8>(static_cast<mini::int8>(-128)) == 0x80u);
    TEST_ENSURE(bitCast<mini::int16>(static_cast<mini::uint16>(0x8000u)) == mini::NumericLimit<mini::int16>::min);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testRotateLeft, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testRotateLeft, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testRotateLeft, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testRotateLeft, mini::uint64);

    TEST_CONSTEXPR_TEMPLATE(testRotateRight, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testRotateRight, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testRotateRight, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testRotateRight, mini::uint64);

    TEST_CONSTEXPR_TEMPLATE(testCountLeftZero, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftZero, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftZero, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftZero, mini::uint64);

    TEST_CONSTEXPR_TEMPLATE(testCountLeftOne, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftOne, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftOne, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testCountLeftOne, mini::uint64);

    TEST_CONSTEXPR_TEMPLATE(testCountRightZero, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testCountRightZero, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testCountRightZero, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testCountRightZero, mini::uint64);

    TEST_CONSTEXPR_TEMPLATE(testCountRightOne, mini::uint8);
    TEST_CONSTEXPR_TEMPLATE(testCountRightOne, mini::uint16);
    TEST_CONSTEXPR_TEMPLATE(testCountRightOne, mini::uint32);
    TEST_CONSTEXPR_TEMPLATE(testCountRightOne, mini::uint64);

    TEST_CONSTEXPR(testBitCast);

    return 0;
}
