#include <chrono>
#include <thread>

#include "test_macro.h"

import mini.test;

using mini::Atomic;
using mini::MemoryOrder;
using mini::NumericLimit;

template <size_t CapacityN>
struct S {
    alignas(1) mini::byte buffer[CapacityN];
};

struct Unaligned {
public:
    int a;
    char b;

    Unaligned() noexcept = default;
    Unaligned(mini::int32 num) noexcept { a = num; }
    operator mini::int32() const noexcept { return a; }
};

struct NonAtomic {
public:
    mini::int32 v[5];

    NonAtomic() noexcept = default;
    NonAtomic(mini::int32 num) noexcept { mini::memory::fillRange(&v[0], &v[5], num); }
    operator mini::int32() const noexcept { return v[0]; }
};

mini::int32 TestAtomicLockFree()
{
    static_assert(sizeof(Atomic<S<1>>) == 1);
    static_assert(sizeof(Atomic<S<2>>) == 2);
    static_assert(sizeof(Atomic<S<3>>) == 4);
    static_assert(sizeof(Atomic<S<4>>) == 4);
    static_assert(sizeof(Atomic<S<5>>) == 8);
    static_assert(sizeof(Atomic<S<8>>) == 8);
    static_assert(sizeof(Atomic<S<9>>) == 16);
    static_assert(sizeof(Atomic<S<16>>) == 16);
    static_assert(sizeof(Atomic<S<17>>) == 17);
    static_assert(sizeof(Atomic<Unaligned>) == 8);
    static_assert(sizeof(Atomic<NonAtomic>) == sizeof(NonAtomic));

    static_assert(Atomic<S<1>>::alwaysLockFree());
    static_assert(Atomic<S<2>>::alwaysLockFree());
    static_assert(Atomic<S<3>>::alwaysLockFree());
    static_assert(Atomic<S<4>>::alwaysLockFree());
    static_assert(Atomic<S<5>>::alwaysLockFree() == Atomic<S<8>>::alwaysLockFree());
    static_assert(Atomic<S<9>>::alwaysLockFree() == Atomic<S<16>>::alwaysLockFree());
    static_assert(Atomic<S<8>>::alwaysLockFree() != Atomic<S<17>>::alwaysLockFree());
    static_assert(Atomic<Unaligned>::alwaysLockFree());
    static_assert(!Atomic<NonAtomic>::alwaysLockFree());

    TEST_ENSURE(Atomic<S<1>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<S<2>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<S<3>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<S<4>>{}.lockFree() == true);
    TEST_ENSURE(Atomic<S<5>>{}.lockFree() == Atomic<S<8>>{}.lockFree());
    TEST_ENSURE(Atomic<S<9>>{}.lockFree() == Atomic<S<16>>{}.lockFree());
    TEST_ENSURE(Atomic<S<8>>{}.lockFree() != Atomic<S<17>>{}.lockFree());
    TEST_ENSURE(Atomic<Unaligned>{}.lockFree() == true);
    TEST_ENSURE(Atomic<NonAtomic>{}.lockFree() == false);

    TEST_ENSURE(static_cast<Atomic<S<1>> volatile>(Atomic<S<1>>{}).lockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<2>> volatile>(Atomic<S<2>>{}).lockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<3>> volatile>(Atomic<S<3>>{}).lockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<4>> volatile>(Atomic<S<4>>{}).lockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<5>> volatile>(Atomic<S<5>>{}).lockFree() ==
                static_cast<Atomic<S<8>> volatile>(Atomic<S<8>>{}).lockFree());
    TEST_ENSURE(static_cast<Atomic<S<9>> volatile>(Atomic<S<9>>{}).lockFree() ==
                static_cast<Atomic<S<16>> volatile>(Atomic<S<16>>{}).lockFree());
    TEST_ENSURE(static_cast<Atomic<S<8>> volatile>(Atomic<S<8>>{}).lockFree() !=
                static_cast<Atomic<S<17>> volatile>(Atomic<S<17>>{}).lockFree());
    TEST_ENSURE(static_cast<Atomic<Unaligned> volatile>(Atomic<Unaligned>{}).lockFree() == true);
    TEST_ENSURE(static_cast<Atomic<NonAtomic> volatile>(Atomic<NonAtomic>{}).lockFree() == false);

    return 0;
}

template <typename T>
mini::int32 TestLoad()
{
    Atomic<T> a(5);

    TEST_ENSURE(a.load(MemoryOrder::relaxed) == 5);
    TEST_ENSURE(a.load(MemoryOrder::consume) == 5);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 5);
    TEST_ENSURE(a.load(MemoryOrder::sequential) == 5);

    return 0;
}

template <typename T>
mini::int32 TestStore()
{
    Atomic<T> a(5);

    a.store(42, MemoryOrder::relaxed);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 42);
    a.store(42, MemoryOrder::release);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 42);
    a.store(42, MemoryOrder::sequential);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 42);

    return 0;
}

template <typename T>
mini::int32 TestExchange()
{
    Atomic<T> a(0);

    TEST_ENSURE(a.exchange(1, MemoryOrder::relaxed) == 0);
    TEST_ENSURE(a.exchange(2, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.exchange(3, MemoryOrder::consume) == 2);
    TEST_ENSURE(a.exchange(4, MemoryOrder::release) == 3);
    TEST_ENSURE(a.exchange(5, MemoryOrder::acquireRelease) == 4);
    TEST_ENSURE(a.exchange(6, MemoryOrder::sequential) == 5);
    TEST_ENSURE(a.load(MemoryOrder::acquire) == 6);

    return 0;
}

template <typename T>
mini::int32 TestCompareExchange()
{
    Atomic<T> a(0);
    T value(0);

    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::relaxed) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::relaxed) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::consume) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::consume) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquire) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquire) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::release) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::release) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::sequential) == false)
    TEST_ENSURE(a.compareExchangeStrong(value, 2, MemoryOrder::sequential) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::relaxed) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::relaxed) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::consume) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::consume) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquire) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquire) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::release) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::release) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquireRelease) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::acquireRelease) == true)
    value = 1;
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::sequential) == false)
    TEST_ENSURE(a.compareExchangeWeak(value, 2, MemoryOrder::sequential) == true)

    return 0;
}

mini::int32 TestIntegralFetchOperators()
{
    Atomic<mini::int32> a(1);

    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquire) == 2);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::release) == 4);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquireRelease) == 5);
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::sequential) == 6);

    TEST_ENSURE(a.fetchSub(1, MemoryOrder::relaxed) == 7);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquire) == 6);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::consume) == 5);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::release) == 4);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::sequential) == 2);

    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::consume) == 1);
    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::release) == 1);
    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::acquireRelease) == 1);
    TEST_ENSURE(a.fetchAnd(3, MemoryOrder::sequential) == 1);

    TEST_ENSURE(a.fetchOr(2, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.fetchOr(2, MemoryOrder::acquire) == 3);
    TEST_ENSURE(a.fetchOr(2, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.fetchOr(2, MemoryOrder::release) == 3);
    TEST_ENSURE(a.fetchOr(2, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.fetchOr(2, MemoryOrder::sequential) == 3);

    TEST_ENSURE(a.fetchXor(2, MemoryOrder::relaxed) == 3);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::release) == 1);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.fetchXor(2, MemoryOrder::sequential) == 1);

    return 0;
}

mini::int32 TestFloatingFetchOperators()
{
    mini::float32 result = 1.23f;
    Atomic<mini::float32> a = result;

    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::relaxed) == result);
    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::acquire) == (result += 1.0f));
    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::consume) == (result += 1.0f));
    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::release) == (result += 1.0f));
    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::acquireRelease) == (result += 1.0f));
    TEST_ENSURE(a.fetchAdd(1.0f, MemoryOrder::sequential) == (result += 1.0f));

    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::relaxed) == (result += 1.0f));
    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::acquire) == (result -= 1.0f));
    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::consume) == (result -= 1.0f));
    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::release) == (result -= 1.0f));
    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::acquireRelease) == (result -= 1.0f));
    TEST_ENSURE(a.fetchSub(1.0f, MemoryOrder::sequential) == (result -= 1.0f));

    return 0;
}

mini::int32 TestPointerFetchOperators()
{
    mini::FixedArray<mini::int32, 7> sarr;
    auto iter = sarr.begin();
    Atomic<mini::int32*> a = iter.address();

    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::relaxed) == (iter++).address());
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquire) == (iter++).address());
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::consume) == (iter++).address());
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::release) == (iter++).address());
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::acquireRelease) == (iter++).address());
    TEST_ENSURE(a.fetchAdd(1, MemoryOrder::sequential) == (iter++).address());

    TEST_ENSURE(a.fetchSub(1, MemoryOrder::relaxed) == (iter--).address());
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquire) == (iter--).address());
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::consume) == (iter--).address());
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::release) == (iter--).address());
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::acquireRelease) == (iter--).address());
    TEST_ENSURE(a.fetchSub(1, MemoryOrder::sequential) == (iter--).address());

    return 0;
}

mini::int32 TestProducerConsumer()
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

    constexpr mini::uint32 count = NumericLimit<mini::int32>::max >> 8;
    auto t1 = std::thread(producer, count);
    auto t2 = std::thread(consumer, count);
    t1.join();
    t2.join();

    TEST_ENSURE(atomic.load(MemoryOrder::acquire) == count);
    TEST_ENSURE(consumerCount == count);
    TEST_ENSURE(producerCount == count);

    return 0;
}

template <typename T>
mini::int32 TestSpinLock()
{
    Atomic<mini::int32> atomic(1);
    Atomic<mini::int32> lock(0);

    auto spinLock = [&atomic, &lock]() -> mini::int32 {
        while (lock.exchange(1, MemoryOrder::acquire) == 1) {
            lock.wait(1, MemoryOrder::relaxed);
        }

        mini::int32 value = atomic.load(MemoryOrder::relaxed);
        lock.store(0, MemoryOrder::release);
        lock.notify();

        return value;
    };

    std::thread thread([&spinLock]() {
        while (spinLock() > 0) { }
    });

    // test if hangs
    for (mini::uint32 i = 0; i < NumericLimit<mini::uint16>::max; ++i) {
        spinLock();
    }

    atomic.store(-2, MemoryOrder::sequential);
    thread.join();
    return 0;
}

int main()
{
    TEST_ENSURE(TestAtomicLockFree() == 0);

    TEST_ENSURE(TestLoad<mini::int32>() == 0);
    TEST_ENSURE(TestLoad<Unaligned>() == 0);
    TEST_ENSURE(TestLoad<NonAtomic>() == 0);

    TEST_ENSURE(TestStore<mini::int32>() == 0);
    TEST_ENSURE(TestStore<Unaligned>() == 0);
    TEST_ENSURE(TestStore<NonAtomic>() == 0);

    TEST_ENSURE(TestExchange<mini::int32>() == 0);
    TEST_ENSURE(TestExchange<Unaligned>() == 0);
    TEST_ENSURE(TestExchange<NonAtomic>() == 0);

    TEST_ENSURE(TestCompareExchange<mini::int32>() == 0);
    TEST_ENSURE(TestCompareExchange<Unaligned>() == 0);
    TEST_ENSURE(TestCompareExchange<NonAtomic>() == 0);

    TEST_ENSURE(TestIntegralFetchOperators() == 0);
    TEST_ENSURE(TestFloatingFetchOperators() == 0);
    TEST_ENSURE(TestPointerFetchOperators() == 0);

    TEST_ENSURE(TestProducerConsumer() == 0);

    TEST_ENSURE(TestSpinLock<mini::int32>() == 0);
    TEST_ENSURE(TestSpinLock<Unaligned>() == 0);
    TEST_ENSURE(TestSpinLock<NonAtomic>() == 0);

    return 0;
}