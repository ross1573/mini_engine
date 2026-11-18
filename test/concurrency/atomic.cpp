#include <chrono>
#include <thread>

#include "test_macro.h"

import mini.test;

using mini::Atomic;
using mini::MemoryOrder;

template <size_t CapacityN>
struct Packed {
    alignas(1) mini::byte buffer[CapacityN];
};

struct Unaligned {
public:
    int a;
    char b;

    Unaligned() noexcept = default;
    Unaligned(mini::int32 num) noexcept
    {
        a = num;
        b = 0;
    }
    operator mini::int32() const noexcept { return a; }
};

enum class Fruit : mini::uint8 {
    apple,
    banana,
    cherry,
};

struct NonAtomic {
public:
    mini::int32 v[5];

    NonAtomic() noexcept = default;
    NonAtomic(mini::int32 num) noexcept { mini::memory::fillRange(&v[0], &v[5], num); }
    operator mini::int32() const noexcept { return v[0]; }
};

template <size_t N>
Packed<N> makePacked(mini::uint8 value)
{
    Packed<N> packed;
    memset(packed.buffer, value, sizeof(packed.buffer));
    return packed;
}

template <typename T>
mini::int32 testCtor()
{
    Atomic<T> a;
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == static_cast<mini::int32>(T()));

    Atomic<T> direct(3);
    TEST_ENSURE(static_cast<mini::int32>(direct.load(MemoryOrder::sequential)) == 3);

    return 0;
}

template <typename T>
mini::int32 testLoad()
{
    Atomic<T> a(5);
    TEST_ENSURE(a.load(MemoryOrder::relaxed) == 5);
    TEST_ENSURE(a.load(MemoryOrder::consume) == 5);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 5);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == 5);

    Atomic<T> volatile v(7);
    TEST_ENSURE(v.load(MemoryOrder::acquire) == 7);

    return 0;
}

template <typename T>
mini::int32 testStore()
{
    Atomic<T> a(0);
    a.store(1, MemoryOrder::relaxed);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 1);
    a.store(2, MemoryOrder::release);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 2);
    a.store(3, MemoryOrder::sequential);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 3);

    Atomic<T> volatile v(0);
    v.store(4, MemoryOrder::release);
    TEST_ENSURE(v.load(MemoryOrder::acquire) == 4);

    return 0;
}

template <typename T>
mini::int32 testExchange()
{
    Atomic<T> a(0);
    TEST_ENSURE(a.exchange(1, MemoryOrder::relaxed) == 0);
    TEST_ENSURE(a.exchange(2, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.exchange(3, MemoryOrder::consume) == 2);
    TEST_ENSURE(a.exchange(4, MemoryOrder::release) == 3);
    TEST_ENSURE(a.exchange(5, MemoryOrder::acquireRelease) == 4);
    TEST_ENSURE(a.exchange(6, MemoryOrder::sequential) == 5);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 6);

    Atomic<T> volatile v(0);
    TEST_ENSURE(v.exchange(1, MemoryOrder::acquireRelease) == 0);
    TEST_ENSURE(v.load(MemoryOrder::acquire) == 1);

    return 0;
}

template <typename T>
mini::int32 testCompareExchangeStrong()
{
    Atomic<T> a(0);
    T value(0);

    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::relaxed) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::relaxed) == true);
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::consume) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::consume) == true);
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquire) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquire) == true);
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::release) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::release) == true);
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == true);
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::sequential) == false);
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::sequential) == true);

    Atomic<T> b(10);
    T expected(3);
    TEST_ENSURE(b.compareExchangeStrong(expected, 20, MemoryOrder::sequential) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 10);
    TEST_ENSURE(static_cast<mini::int32>(b.load(MemoryOrder::sequential)) == 10);
    TEST_ENSURE(b.compareExchangeStrong(expected, 20, MemoryOrder::sequential) == true);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 10);
    TEST_ENSURE(static_cast<mini::int32>(b.load(MemoryOrder::sequential)) == 20);

    expected = 0;
    TEST_ENSURE(b.compareExchangeStrong(expected, 30, MemoryOrder::acquireRelease, MemoryOrder::acquire) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 20);
    TEST_ENSURE(b.compareExchangeStrong(expected, 30, MemoryOrder::release, MemoryOrder::relaxed) == true);
    TEST_ENSURE(static_cast<mini::int32>(b.load(MemoryOrder::acquire)) == 30);

    Atomic<T> volatile v(8);
    expected = 0;
    TEST_ENSURE(v.compareExchangeStrong(expected, 9, MemoryOrder::sequential, MemoryOrder::sequential) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 8);
    TEST_ENSURE(v.compareExchangeStrong(expected, 9, MemoryOrder::acquireRelease, MemoryOrder::acquire) == true);
    TEST_ENSURE(static_cast<mini::int32>(v.load(MemoryOrder::sequential)) == 9);

    expected = 0;
    TEST_ENSURE(v.compareExchangeStrong(expected, 10, MemoryOrder::sequential) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 9);
    TEST_ENSURE(v.compareExchangeStrong(expected, 10, MemoryOrder::acquireRelease) == true);
    TEST_ENSURE(static_cast<mini::int32>(v.load(MemoryOrder::sequential)) == 10);

    return 0;
}

template <typename T>
mini::int32 testCompareExchangeWeak()
{
    Atomic<T> a(0);
    T value(0);

    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::relaxed) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::relaxed)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::consume) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::consume)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquire) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::acquire)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::release) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::release)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquireRelease) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::acquireRelease)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::sequential) == false);
    while (!a.compareExchangeWeak(value, 2, MemoryOrder::sequential)) { }
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 2);

    Atomic<T> b(30);
    T expected(0);
    TEST_ENSURE(b.compareExchangeWeak(expected, 40, MemoryOrder::sequential, MemoryOrder::sequential) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 30);
    while (!b.compareExchangeWeak(expected, 40, MemoryOrder::acquire, MemoryOrder::relaxed)) { }
    TEST_ENSURE(static_cast<mini::int32>(b.load(MemoryOrder::acquire)) == 40);

    Atomic<T> volatile v(9);
    expected = 0;
    TEST_ENSURE(v.compareExchangeWeak(expected, 10, MemoryOrder::sequential) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 9);
    while (!v.compareExchangeWeak(expected, 10, MemoryOrder::sequential)) { }
    TEST_ENSURE(static_cast<mini::int32>(v.load(MemoryOrder::sequential)) == 10);

    expected = 0;
    TEST_ENSURE(v.compareExchangeWeak(expected, 11, MemoryOrder::release, MemoryOrder::relaxed) == false);
    TEST_ENSURE(static_cast<mini::int32>(expected) == 10);
    while (!v.compareExchangeWeak(expected, 11, MemoryOrder::release, MemoryOrder::relaxed)) { }
    TEST_ENSURE(static_cast<mini::int32>(v.load(MemoryOrder::sequential)) == 11);

    return 0;
}

template <typename T>
mini::int32 testConversion()
{
    Atomic<T> a(1);
    a.store(7, MemoryOrder::sequential);
    TEST_ENSURE(static_cast<mini::int32>(static_cast<T>(a)) == 7);

    Atomic<T> volatile v(8);
    TEST_ENSURE(static_cast<mini::int32>(static_cast<T>(v)) == 8);

    return 0;
}

template <typename T>
mini::int32 testAssign()
{
    Atomic<T> a(0);
    TEST_ENSURE(static_cast<mini::int32>(a = T(5)) == 5);
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == 5);
    TEST_ENSURE(static_cast<mini::int32>(a = T(-3)) == -3);
    TEST_ENSURE(static_cast<mini::int32>(a.load(MemoryOrder::sequential)) == -3);

    Atomic<T> volatile v(0);
    TEST_ENSURE(static_cast<mini::int32>(v = T(7)) == 7);
    TEST_ENSURE(static_cast<mini::int32>(v.load(MemoryOrder::sequential)) == 7);

    return 0;
}

template <typename T>
mini::int32 testFetchAdd()
{
    constexpr T max = mini::NumericLimit<T>::max;
    constexpr T min = mini::NumericLimit<T>::min;

    Atomic<T> a(1);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::relaxed) == T(1));
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquire) == T(2));
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::consume) == T(3));
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::release) == T(4));
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquireRelease) == T(5));
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::sequential) == T(6));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(7));

    a.store(max, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::sequential) == max);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == min);

    Atomic<T> volatile v(5);
    TEST_ENSURE(v.fetchAdd(2, MemoryOrder::relaxed) == T(5));
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == T(7));

    return 0;
}

template <typename T>
mini::int32 testFetchSub()
{
    constexpr T max = mini::NumericLimit<T>::max;
    constexpr T min = mini::NumericLimit<T>::min;

    Atomic<T> a(7);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::relaxed) == T(7));
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquire) == T(6));
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::consume) == T(5));
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::release) == T(4));
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquireRelease) == T(3));
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::sequential) == T(2));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(1));

    a.store(min, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::sequential) == min);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == max);

    Atomic<T> volatile v(7);
    TEST_ENSURE(v.fetchSub(3, MemoryOrder::relaxed) == T(7));
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == T(4));

    return 0;
}

template <typename T>
mini::int32 testFetchAnd()
{
    Atomic<T> a(0x7F);
    TEST_ENSURE(a.fetchAnd(0x7E, MemoryOrder::relaxed) == T(0x7F));
    TEST_ENSURE(a.fetchAnd(0x7C, MemoryOrder::acquire) == T(0x7E));
    TEST_ENSURE(a.fetchAnd(0x78, MemoryOrder::consume) == T(0x7C));
    TEST_ENSURE(a.fetchAnd(0x70, MemoryOrder::release) == T(0x78));
    TEST_ENSURE(a.fetchAnd(0x60, MemoryOrder::acquireRelease) == T(0x70));
    TEST_ENSURE(a.fetchAnd(0x40, MemoryOrder::sequential) == T(0x60));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0x40));

    a.store(0x0F, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchAnd(0x3C, MemoryOrder::sequential) == T(0x0F));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0x0C));

    Atomic<T> volatile v(12);
    TEST_ENSURE(v.fetchAnd(8, MemoryOrder::relaxed) == T(12));
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == T(8));

    return 0;
}

template <typename T>
mini::int32 testFetchOr()
{
    Atomic<T> a(0);
    TEST_ENSURE(a.fetchOr(0x01, MemoryOrder::relaxed) == T(0x00));
    TEST_ENSURE(a.fetchOr(0x02, MemoryOrder::acquire) == T(0x01));
    TEST_ENSURE(a.fetchOr(0x04, MemoryOrder::consume) == T(0x03));
    TEST_ENSURE(a.fetchOr(0x08, MemoryOrder::release) == T(0x07));
    TEST_ENSURE(a.fetchOr(0x10, MemoryOrder::acquireRelease) == T(0x0F));
    TEST_ENSURE(a.fetchOr(0x20, MemoryOrder::sequential) == T(0x1F));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0x3F));

    a.store(0, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchOr(0x0F, MemoryOrder::sequential) == T(0));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0x0F));

    Atomic<T> volatile v(4);
    TEST_ENSURE(v.fetchOr(8, MemoryOrder::relaxed) == T(4));
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == T(12));

    return 0;
}

template <typename T>
mini::int32 testFetchXor()
{
    Atomic<T> a(3);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::relaxed) == T(3));
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::acquire) == T(1));
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::consume) == T(3));
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::release) == T(1));
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::acquireRelease) == T(3));
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::sequential) == T(1));

    a.store(0x0C, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchXor(0x0C, MemoryOrder::sequential) == T(0x0C));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0));

    Atomic<T> volatile v(8);
    TEST_ENSURE(v.fetchXor(8, MemoryOrder::relaxed) == T(8));
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == T(0));

    return 0;
}

template <typename T>
mini::int32 testFloatingFetchAdd()
{
    Atomic<T> a(T(0.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::relaxed) == T(0.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::acquire) == T(1.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::consume) == T(2.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::release) == T(3.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::acquireRelease) == T(4.5));
    TEST_ENSURE(a.fetchAdd(T(1), MemoryOrder::sequential) == T(5.5));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(6.5));

    Atomic<T> volatile v(T(0.5));
    TEST_ENSURE(v.fetchAdd(T(1), MemoryOrder::relaxed) == T(0.5));
    TEST_ENSURE(v.load(MemoryOrder::sequential) == T(1.5));

    return 0;
}

template <typename T>
mini::int32 testFloatingFetchSub()
{
    Atomic<T> a(T(6.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::relaxed) == T(6.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::acquire) == T(5.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::consume) == T(4.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::release) == T(3.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::acquireRelease) == T(2.5));
    TEST_ENSURE(a.fetchSub(T(1), MemoryOrder::sequential) == T(1.5));
    TEST_ENSURE(a.load(MemoryOrder::sequential) == T(0.5));

    Atomic<T> volatile v(T(1.5));
    TEST_ENSURE(v.fetchSub(T(0.25), MemoryOrder::acquireRelease) == T(1.5));
    TEST_ENSURE(v.load(MemoryOrder::sequential) == T(1.25));

    return 0;
}

mini::int32 testLockFree()
{
    TEST_ENSURE(Atomic<Packed<1>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<Packed<2>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<Packed<3>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<Packed<4>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<Packed<5>>{}.lockFree() == Atomic<Packed<8>>{}.lockFree());
    TEST_ENSURE(Atomic<Packed<9>>{}.lockFree() == Atomic<Packed<16>>{}.lockFree());
    TEST_ENSURE(Atomic<Packed<8>>{}.lockFree() != Atomic<Packed<17>>{}.lockFree());
    TEST_ENSURE(Atomic<Unaligned>{}.lockFree() == true);
    TEST_ENSURE(Atomic<NonAtomic>{}.lockFree() == false);

    Atomic<Packed<3>> volatile v3;
    Atomic<NonAtomic> volatile nonAtomic;

    TEST_ENSURE(v3.lockFree() == true);
    TEST_ENSURE(nonAtomic.lockFree() == false);

    return 0;
}

mini::int32 testPointerFetchAdd()
{
    mini::int32 arr[8] = {};
    Atomic<mini::int32*> a(arr);

    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::relaxed) == arr);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquire) == arr + 1);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::consume) == arr + 2);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::release) == arr + 3);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquireRelease) == arr + 4);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::sequential) == arr + 5);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 6);

    a.store(arr, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchAdd(3, MemoryOrder::sequential) == arr);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 3);

    Atomic<mini::int32*> volatile v(arr);
    TEST_ENSURE(v.fetchAdd(4, MemoryOrder::relaxed) == arr);
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == arr + 4);

    return 0;
}

mini::int32 testPointerFetchSub()
{
    mini::int32 arr[8] = {};
    Atomic<mini::int32*> a(arr + 7);

    TEST_ENSURE(a.fetchSub(1, MemoryOrder::relaxed) == arr + 7);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquire) == arr + 6);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::consume) == arr + 5);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::release) == arr + 4);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquireRelease) == arr + 3);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::sequential) == arr + 2);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 1);

    a.store(arr + 3, MemoryOrder::sequential);
    TEST_ENSURE(a.fetchSub(2, MemoryOrder::sequential) == arr + 3);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 1);

    Atomic<mini::int32*> volatile v(arr + 4);
    TEST_ENSURE(v.fetchSub(1, MemoryOrder::relaxed) == arr + 4);
    TEST_ENSURE(v.load(MemoryOrder::relaxed) == arr + 3);

    return 0;
}

mini::int32 testPointerExchange()
{
    mini::int32 arr[4] = {};
    Atomic<mini::int32*> a(arr);

    TEST_ENSURE(a.exchange(arr + 1, MemoryOrder::sequential) == arr);
    TEST_ENSURE(a.exchange(nullptr, MemoryOrder::acquireRelease) == arr + 1);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == nullptr);
    TEST_ENSURE((a = arr + 2) == arr + 2);

    mini::int32* expected = arr;
    TEST_ENSURE(a.compareExchangeStrong(expected, arr + 3, MemoryOrder::sequential) == false);
    TEST_ENSURE(expected == arr + 2);
    TEST_ENSURE(a.compareExchangeStrong(expected, arr + 3, MemoryOrder::sequential) == true);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 3);

    expected = nullptr;
    TEST_ENSURE(a.compareExchangeWeak(expected, arr, MemoryOrder::acquire, MemoryOrder::relaxed) == false);
    TEST_ENSURE(expected == arr + 3);
    while (!a.compareExchangeWeak(expected, arr, MemoryOrder::acquire, MemoryOrder::relaxed)) { }
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr);

    Atomic<mini::int32*> volatile v(arr);
    TEST_ENSURE(v.exchange(arr + 1, MemoryOrder::relaxed) == arr);
    expected = arr + 1;
    TEST_ENSURE(v.compareExchangeStrong(expected, arr + 2, MemoryOrder::sequential, MemoryOrder::sequential) == true);
    TEST_ENSURE(v.load(MemoryOrder::sequential) == arr + 2);

    return 0;
}

mini::int32 testFence()
{
    constexpr mini::int32 rounds = 100;

    for (mini::int32 round = 0; round < rounds; ++round) {
        mini::int32 data = 0;
        Atomic<mini::int32> flag(0);
        mini::int32 observed = -1;

        std::thread consumer([&data, &flag, &observed]() {
            while (flag.load(MemoryOrder::relaxed) == 0) {
                std::this_thread::yield();
            }
            Atomic<mini::int32>::threadFence(MemoryOrder::acquire);
            observed = data;
        });

        data = round + 1;
        Atomic<mini::int32>::threadFence(MemoryOrder::release);
        flag.store(1, MemoryOrder::relaxed);
        consumer.join();

        TEST_ENSURE(observed == round + 1);
    }

    return 0;
}

template <typename T>
mini::int32 testDistinctValuesOf(T first, T second)
{
    Atomic<T> a(first);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == first);
    a.store(second, MemoryOrder::release);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == second);
    TEST_ENSURE(static_cast<T>(a) == second);
    TEST_ENSURE((a = first) == first);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == first);

    TEST_ENSURE(a.exchange(second, MemoryOrder::acquireRelease) == first);
    TEST_ENSURE(a.exchange(first, MemoryOrder::sequential) == second);

    T expected = second;
    TEST_ENSURE(a.compareExchangeStrong(expected, second, MemoryOrder::sequential) == false);
    TEST_ENSURE(expected == first);
    TEST_ENSURE(a.compareExchangeStrong(expected, second, MemoryOrder::sequential) == true);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == second);

    expected = first;
    TEST_ENSURE(a.compareExchangeWeak(expected, first, MemoryOrder::acquireRelease, MemoryOrder::acquire) == false);
    TEST_ENSURE(expected == second);
    while (!a.compareExchangeWeak(expected, first, MemoryOrder::acquireRelease, MemoryOrder::acquire)) { }
    TEST_ENSURE(a.load(MemoryOrder::sequential) == first);

    Atomic<T> volatile v(first);
    TEST_ENSURE(v.exchange(second, MemoryOrder::sequential) == first);
    TEST_ENSURE((v = first) == first);
    TEST_ENSURE(v.load(MemoryOrder::sequential) == first);

    return 0;
}

mini::int32 testDistinctValues()
{
    TEST_ENSURE(testDistinctValuesOf<bool>(false, true) == 0);
    TEST_ENSURE(testDistinctValuesOf<Fruit>(Fruit::apple, Fruit::cherry) == 0);
    TEST_ENSURE(testDistinctValuesOf<mini::uint16>(1, 0xFFFF) == 0);
    TEST_ENSURE(testDistinctValuesOf<mini::float32>(1.5f, -0.25f) == 0);
    TEST_ENSURE(testDistinctValuesOf<mini::float64>(1.5, -0.25) == 0);

    return 0;
}

mini::int32 testPointerArithmeticScale()
{
    NonAtomic arr[6] = {};
    Atomic<NonAtomic*> a(arr);

    TEST_ENSURE(a.fetchAdd(2, MemoryOrder::sequential) == arr);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 2);
    TEST_ENSURE(a.fetchAdd(-1, MemoryOrder::sequential) == arr + 2);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 1);
    TEST_ENSURE(a.fetchSub(-4, MemoryOrder::sequential) == arr + 1);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr + 5);
    TEST_ENSURE(a.fetchSub(5, MemoryOrder::sequential) == arr + 5);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == arr);

    char bytes[4] = {};
    Atomic<char*> c(bytes);
    TEST_ENSURE(c.fetchAdd(3, MemoryOrder::relaxed) == bytes);
    TEST_ENSURE(c.fetchSub(1, MemoryOrder::relaxed) == bytes + 3);
    TEST_ENSURE(c.load(MemoryOrder::relaxed) == bytes + 2);

    mini::int64 const values[3] = {1, 2, 3};
    Atomic<mini::int64 const*> cp(values);
    TEST_ENSURE(*cp.fetchAdd(2, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(*cp.load(MemoryOrder::relaxed) == 3);

    return 0;
}

template <typename T>
mini::int32 testFloatingSpecial()
{
    T volatile zero = T(0);
    T nan = zero / zero;

    Atomic<T> a(nan);
    T old = a.fetchAdd(T(1), MemoryOrder::sequential);
    TEST_ENSURE(old != old);
    T current = a.load(MemoryOrder::sequential);
    TEST_ENSURE(current != current);

    old = a.fetchSub(T(1), MemoryOrder::sequential);
    TEST_ENSURE(old != old);

    Atomic<T> b(T(1));
    TEST_ENSURE(b.fetchAdd(T(-2.5), MemoryOrder::sequential) == T(1));
    TEST_ENSURE(b.load(MemoryOrder::sequential) == T(-1.5));
    TEST_ENSURE(b.fetchSub(T(-0.5), MemoryOrder::sequential) == T(-1.5));
    TEST_ENSURE(b.load(MemoryOrder::sequential) == T(-1));

    return 0;
}

template <typename T>
mini::int32 testConcurrentFloating()
{
    constexpr mini::int32 threadCount = 4;
    constexpr mini::int32 iteration = 10000;

    Atomic<T> sum(T(0));
    Atomic<T> volatile difference(T(threadCount * iteration));

    auto worker = [&sum, &difference]() {
        for (mini::int32 i = 0; i < iteration; ++i) {
            sum.fetchAdd(T(1), MemoryOrder::relaxed);
            difference.fetchSub(T(1), MemoryOrder::acquireRelease);
        }
    };

    std::thread threads[threadCount];
    for (std::thread& thread : threads) {
        thread = std::thread(worker);
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    TEST_ENSURE(sum.load(MemoryOrder::sequential) == T(threadCount * iteration));
    TEST_ENSURE(difference.load(MemoryOrder::sequential) == T(0));

    return 0;
}

mini::int32 testWaitMismatch()
{
    Atomic<mini::int32> a(5);
    a.wait(4, MemoryOrder::acquire);
    TEST_ENSURE(a.load(MemoryOrder::relaxed) == 5);

    Atomic<mini::int32> const volatile cv(5);
    cv.wait(4, MemoryOrder::acquire);
    TEST_ENSURE(cv.load(MemoryOrder::relaxed) == 5);

    return 0;
}

mini::int32 testNotifyWithoutWaiter()
{
    Atomic<mini::int32> a(5);
    a.notify();
    a.notifyAll();
    TEST_ENSURE(a.load(MemoryOrder::relaxed) == 5);

    Atomic<mini::int32> const volatile cv(5);
    cv.notify();
    cv.notifyAll();
    TEST_ENSURE(cv.load(MemoryOrder::relaxed) == 5);

    return 0;
}

mini::int32 testVolatileWait()
{
    Atomic<mini::int32> volatile flag(0);
    Atomic<mini::int32> woken(0);

    std::thread thread([&flag, &woken]() {
        while (flag.load(MemoryOrder::acquire) == 0) {
            flag.wait(0, MemoryOrder::acquire);
        }
        woken.fetchAdd(1, MemoryOrder::release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    flag.store(1, MemoryOrder::release);
    flag.notifyAll();
    thread.join();

    TEST_ENSURE(woken.load(MemoryOrder::acquire) == 1);

    return 0;
}

template <typename T>
mini::int32 testContentionWaitOf(T zero, T one)
{
    auto equals = [](T const& lhs, T const& rhs) { return memcmp(&lhs, &rhs, sizeof(T)) == 0; };

    Atomic<T> mismatch(one);
    mismatch.wait(zero, MemoryOrder::acquire);
    TEST_ENSURE(equals(mismatch.load(MemoryOrder::relaxed), one));
    mismatch.notify();
    mismatch.notifyAll();

    Atomic<T> single(zero);
    Atomic<mini::int32> singleWoken(0);
    std::thread singleThread([&single, &singleWoken, &equals, zero]() {
        while (equals(single.load(MemoryOrder::acquire), zero)) {
            single.wait(zero, MemoryOrder::acquire);
        }
        singleWoken.fetchAdd(1, MemoryOrder::release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    single.store(one, MemoryOrder::release);
    single.notify();
    singleThread.join();
    TEST_ENSURE(singleWoken.load(MemoryOrder::acquire) == 1);

    constexpr mini::int32 threadCount = 4;
    Atomic<T> flag(zero);
    Atomic<mini::int32> woken(0);
    std::thread threads[threadCount];
    for (std::thread& thread : threads) {
        thread = std::thread([&flag, &woken, &equals, zero]() {
            while (equals(flag.load(MemoryOrder::acquire), zero)) {
                flag.wait(zero, MemoryOrder::acquire);
            }
            woken.fetchAdd(1, MemoryOrder::release);
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    flag.store(one, MemoryOrder::release);
    flag.notifyAll();
    for (std::thread& thread : threads) {
        thread.join();
    }
    TEST_ENSURE(woken.load(MemoryOrder::acquire) == threadCount);

    return 0;
}

mini::int32 testContentionWait()
{
    TEST_ENSURE(testContentionWaitOf<NonAtomic>(NonAtomic(0), NonAtomic(1)) == 0);
    TEST_ENSURE(testContentionWaitOf<Packed<3>>(makePacked<3>(0), makePacked<3>(0xA5)) == 0);
    TEST_ENSURE(testContentionWaitOf<mini::uint8>(0, 0xFF) == 0);
    TEST_ENSURE(testContentionWaitOf<mini::uint16>(0, 0x100) == 0);
    TEST_ENSURE(testContentionWaitOf<mini::int32>(0, 1) == 0);
    TEST_ENSURE(testContentionWaitOf<mini::int64>(0, 0x100000000LL) == 0);

    return 0;
}

mini::int32 testConcurrentCompareExchange()
{
    constexpr mini::int32 threadCount = 4;
    constexpr mini::int32 iteration = 10000;

    Atomic<NonAtomic> object(0);
    Atomic<mini::int32> torn(0);

    auto worker = [&object, &torn]() {
        for (mini::int32 i = 0; i < iteration; ++i) {
            NonAtomic expected = object.load(MemoryOrder::relaxed);
            NonAtomic desired;
            do {
                for (mini::int32 k = 1; k < 5; ++k) {
                    if (expected.v[k] != expected.v[0]) {
                        torn.fetchAdd(1, MemoryOrder::relaxed);
                    }
                }
                desired = NonAtomic(expected.v[0] + 1);
            } while (!object.compareExchangeWeak(expected, desired, MemoryOrder::acquireRelease, MemoryOrder::relaxed));
        }
    };

    std::thread threads[threadCount];
    for (std::thread& thread : threads) {
        thread = std::thread(worker);
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    NonAtomic result = object.load(MemoryOrder::sequential);
    for (mini::int32 k = 0; k < 5; ++k) {
        TEST_ENSURE(result.v[k] == threadCount * iteration);
    }
    TEST_ENSURE(torn.load(MemoryOrder::sequential) == 0);

    return 0;
}

mini::int32 testProducerConsumer()
{
    Atomic<mini::uint32> atomic = 0;
    mini::uint32 consumerCount = 0;
    mini::uint32 producerCount = 0;

    auto consumer = [&atomic, &consumerCount](mini::uint32 end) {
        for (mini::uint32 value = 0; value < end; ++consumerCount, ++value) {
            atomic.wait(value, MemoryOrder::acquire);
        }
    };

    auto producer = [&atomic, &producerCount](mini::uint32 end) {
        for (mini::uint32 value = 0; value < end; ++producerCount, ++value) {
            atomic.fetchAdd(1, MemoryOrder::release);
            atomic.notify();
        }
    };

    constexpr mini::uint32 count = 1 << 16;
    auto t1 = std::thread(producer, count);
    auto t2 = std::thread(consumer, count);
    t1.join();
    t2.join();

    TEST_ENSURE(atomic.load(MemoryOrder::acquire) == count);
    TEST_ENSURE(consumerCount == count);
    TEST_ENSURE(producerCount == count);

    return 0;
}

mini::int32 testSpinLockExclusion()
{
    constexpr mini::int32 iterations = 100000;
    Atomic<mini::int32> lock(0);
    mini::int32 counter = 0;

    auto increment = [&lock, &counter]() {
        for (mini::int32 i = 0; i < iterations; ++i) {
            while (lock.exchange(1, MemoryOrder::acquire) == 1) {
                lock.wait(1, MemoryOrder::relaxed);
            }

            ++counter;
            lock.store(0, MemoryOrder::release);
            lock.notify();
        }
    };

    std::thread first(increment);
    std::thread second(increment);
    increment();
    first.join();
    second.join();

    TEST_ENSURE(counter == iterations * 3);
    TEST_ENSURE(lock.load(MemoryOrder::relaxed) == 0);

    return 0;
}

int main()
{
    TEST_FUNCTION_TEMPLATE(testCtor, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCtor, Unaligned);
    TEST_FUNCTION_TEMPLATE(testCtor, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testLoad, mini::int32);
    TEST_FUNCTION_TEMPLATE(testLoad, Unaligned);
    TEST_FUNCTION_TEMPLATE(testLoad, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testStore, mini::int32);
    TEST_FUNCTION_TEMPLATE(testStore, Unaligned);
    TEST_FUNCTION_TEMPLATE(testStore, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testExchange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testExchange, Unaligned);
    TEST_FUNCTION_TEMPLATE(testExchange, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testCompareExchangeStrong, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCompareExchangeStrong, Unaligned);
    TEST_FUNCTION_TEMPLATE(testCompareExchangeStrong, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testCompareExchangeWeak, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCompareExchangeWeak, Unaligned);
    TEST_FUNCTION_TEMPLATE(testCompareExchangeWeak, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testConversion, mini::int32);
    TEST_FUNCTION_TEMPLATE(testConversion, Unaligned);
    TEST_FUNCTION_TEMPLATE(testConversion, NonAtomic);

    TEST_FUNCTION_TEMPLATE(testAssign, mini::int32);
    TEST_FUNCTION_TEMPLATE(testAssign, Unaligned);
    TEST_FUNCTION_TEMPLATE(testAssign, NonAtomic);
    TEST_FUNCTION_TEMPLATE(testAssign, mini::float32);

    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::int8);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::uint8);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::int16);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::uint16);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::int32);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::uint32);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::int64);
    TEST_FUNCTION_TEMPLATE(testFetchAdd, mini::uint64);

    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::int8);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::uint8);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::int16);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::uint16);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::int32);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::uint32);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::int64);
    TEST_FUNCTION_TEMPLATE(testFetchSub, mini::uint64);

    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::int8);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::uint8);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::int16);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::uint16);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::int32);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::uint32);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::int64);
    TEST_FUNCTION_TEMPLATE(testFetchAnd, mini::uint64);

    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::int8);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::uint8);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::int16);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::uint16);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::int32);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::uint32);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::int64);
    TEST_FUNCTION_TEMPLATE(testFetchOr, mini::uint64);

    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::int8);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::uint8);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::int16);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::uint16);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::int32);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::uint32);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::int64);
    TEST_FUNCTION_TEMPLATE(testFetchXor, mini::uint64);

    TEST_FUNCTION_TEMPLATE(testFloatingFetchAdd, mini::float32);
    TEST_FUNCTION_TEMPLATE(testFloatingFetchAdd, mini::float64);

    TEST_FUNCTION_TEMPLATE(testFloatingFetchSub, mini::float32);
    TEST_FUNCTION_TEMPLATE(testFloatingFetchSub, mini::float64);

    TEST_FUNCTION(testLockFree);
    TEST_FUNCTION(testPointerFetchAdd);
    TEST_FUNCTION(testPointerFetchSub);
    TEST_FUNCTION(testPointerExchange);
    TEST_FUNCTION(testFence);

    TEST_FUNCTION(testDistinctValues);

    TEST_FUNCTION(testPointerArithmeticScale);

    TEST_FUNCTION_TEMPLATE(testFloatingSpecial, mini::float32);
    TEST_FUNCTION_TEMPLATE(testFloatingSpecial, mini::float64);
    TEST_FUNCTION_TEMPLATE(testConcurrentFloating, mini::float32);
    TEST_FUNCTION_TEMPLATE(testConcurrentFloating, mini::float64);

    TEST_FUNCTION(testWaitMismatch);
    TEST_FUNCTION(testNotifyWithoutWaiter);
    TEST_FUNCTION(testVolatileWait);
    TEST_FUNCTION(testContentionWait);

    TEST_FUNCTION(testConcurrentCompareExchange);
    TEST_FUNCTION(testProducerConsumer);
    TEST_FUNCTION(testSpinLockExclusion);

    return 0;
}
