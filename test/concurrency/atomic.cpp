#include <chrono>
#include <thread>

#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

template <SizeT CapacityN>
struct S {
    alignas(1) byte buffer[CapacityN];
};

struct Unaligned {
public:
    int a;
    char b;

    Unaligned() noexcept = default;
    Unaligned(int32 n) noexcept { a = n; }
    operator int32() const noexcept { return a; }
};

struct NonAtomic {
public:
    int32 v[5];

    NonAtomic() noexcept = default;
    NonAtomic(int32 n) noexcept { memory::FillRange(&v[0], &v[5], n); }
    operator int32() const noexcept { return v[0]; }
};

static int32 TestAtomicLockFree()
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

    static_assert(Atomic<S<1>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<2>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<3>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<4>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<5>>::IsAlwaysLockFree() == Atomic<S<8>>::IsAlwaysLockFree());
    static_assert(Atomic<S<9>>::IsAlwaysLockFree() == Atomic<S<16>>::IsAlwaysLockFree());
    static_assert(Atomic<S<8>>::IsAlwaysLockFree() != Atomic<S<17>>::IsAlwaysLockFree());
    static_assert(Atomic<Unaligned>::IsAlwaysLockFree() == true);
    static_assert(Atomic<NonAtomic>::IsAlwaysLockFree() == false);

    TEST_ENSURE(Atomic<S<1>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<2>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<3>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<4>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<5>>{}.IsLockFree() == Atomic<S<8>>{}.IsLockFree());
    TEST_ENSURE(Atomic<S<9>>{}.IsLockFree() == Atomic<S<16>>{}.IsLockFree());
    TEST_ENSURE(Atomic<S<8>>{}.IsLockFree() != Atomic<S<17>>{}.IsLockFree());
    TEST_ENSURE(Atomic<Unaligned>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<NonAtomic>{}.IsLockFree() == false);

    TEST_ENSURE(static_cast<Atomic<S<1>> volatile>(Atomic<S<1>>{}).IsLockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<2>> volatile>(Atomic<S<2>>{}).IsLockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<3>> volatile>(Atomic<S<3>>{}).IsLockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<4>> volatile>(Atomic<S<4>>{}).IsLockFree() == true);
    TEST_ENSURE(static_cast<Atomic<S<5>> volatile>(Atomic<S<5>>{}).IsLockFree() ==
                static_cast<Atomic<S<8>> volatile>(Atomic<S<8>>{}).IsLockFree());
    TEST_ENSURE(static_cast<Atomic<S<9>> volatile>(Atomic<S<9>>{}).IsLockFree() ==
                static_cast<Atomic<S<16>> volatile>(Atomic<S<16>>{}).IsLockFree());
    TEST_ENSURE(static_cast<Atomic<S<8>> volatile>(Atomic<S<8>>{}).IsLockFree() !=
                static_cast<Atomic<S<17>> volatile>(Atomic<S<17>>{}).IsLockFree());
    TEST_ENSURE(static_cast<Atomic<Unaligned> volatile>(Atomic<Unaligned>{}).IsLockFree() == true);
    TEST_ENSURE(static_cast<Atomic<NonAtomic> volatile>(Atomic<NonAtomic>{}).IsLockFree() == false);

    return 0;
}

template <typename T>
int32 TestLoad()
{
    Atomic<T> a(5);

    TEST_ENSURE(a.Load(MemoryOrder::relaxed) == 5);
    TEST_ENSURE(a.Load(MemoryOrder::consume) == 5);
    TEST_ENSURE(a.Load(MemoryOrder::acquire) == 5);
    TEST_ENSURE(a.Load(MemoryOrder::sequential) == 5);

    return 0;
}

template <typename T>
int32 TestStore()
{
    Atomic<T> a(5);

    a.Store(42, MemoryOrder::relaxed);
    TEST_ENSURE(a.Load(MemoryOrder::acquire) == 42);
    a.Store(42, MemoryOrder::release);
    TEST_ENSURE(a.Load(MemoryOrder::acquire) == 42);
    a.Store(42, MemoryOrder::sequential);
    TEST_ENSURE(a.Load(MemoryOrder::acquire) == 42);

    return 0;
}

template <typename T>
int32 TestExchange()
{
    Atomic<T> a(0);

    TEST_ENSURE(a.Exchange(1, MemoryOrder::relaxed) == 0);
    TEST_ENSURE(a.Exchange(2, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.Exchange(3, MemoryOrder::consume) == 2);
    TEST_ENSURE(a.Exchange(4, MemoryOrder::release) == 3);
    TEST_ENSURE(a.Exchange(5, MemoryOrder::acquireRelease) == 4);
    TEST_ENSURE(a.Exchange(6, MemoryOrder::sequential) == 5);
    TEST_ENSURE(a.Load(MemoryOrder::acquire) == 6);

    return 0;
}

template <typename T>
int32 TestCompareExchange()
{
    Atomic<T> a(0);
    T value(0);

    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::relaxed) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::relaxed) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::consume) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::consume) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::acquire) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::acquire) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::release) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::release) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::acquireRelease) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::sequential) == false)
    TEST_ENSURE(a.CompareExchangeStrong(value, 2, MemoryOrder::sequential) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::relaxed) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::relaxed) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::consume) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::consume) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::acquire) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::acquire) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::release) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::release) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::acquireRelease) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::acquireRelease) == true)
    value = 1;
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::sequential) == false)
    TEST_ENSURE(a.CompareExchangeWeak(value, 2, MemoryOrder::sequential) == true)

    return 0;
}

int32 TestIntegralFetchOperators()
{
    Atomic<int32> a(1);

    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::acquire) == 2);
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::release) == 4);
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::acquireRelease) == 5);
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::sequential) == 6);

    TEST_ENSURE(a.FetchSub(1, MemoryOrder::relaxed) == 7);
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::acquire) == 6);
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::consume) == 5);
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::release) == 4);
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::sequential) == 2);

    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::consume) == 1);
    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::release) == 1);
    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::acquireRelease) == 1);
    TEST_ENSURE(a.FetchAnd(3, MemoryOrder::sequential) == 1);

    TEST_ENSURE(a.FetchOr(2, MemoryOrder::relaxed) == 1);
    TEST_ENSURE(a.FetchOr(2, MemoryOrder::acquire) == 3);
    TEST_ENSURE(a.FetchOr(2, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.FetchOr(2, MemoryOrder::release) == 3);
    TEST_ENSURE(a.FetchOr(2, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.FetchOr(2, MemoryOrder::sequential) == 3);

    TEST_ENSURE(a.FetchXor(2, MemoryOrder::relaxed) == 3);
    TEST_ENSURE(a.FetchXor(2, MemoryOrder::acquire) == 1);
    TEST_ENSURE(a.FetchXor(2, MemoryOrder::consume) == 3);
    TEST_ENSURE(a.FetchXor(2, MemoryOrder::release) == 1);
    TEST_ENSURE(a.FetchXor(2, MemoryOrder::acquireRelease) == 3);
    TEST_ENSURE(a.FetchXor(2, MemoryOrder::sequential) == 1);

    return 0;
}

int32 TestFloatingFetchOperators()
{
    float32 result = 1.23f;
    Atomic<float32> a = result;

    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::relaxed) == result);
    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::acquire) == (result += 1.0f));
    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::consume) == (result += 1.0f));
    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::release) == (result += 1.0f));
    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::acquireRelease) == (result += 1.0f));
    TEST_ENSURE(a.FetchAdd(1.0f, MemoryOrder::sequential) == (result += 1.0f));

    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::relaxed) == (result += 1.0f));
    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::acquire) == (result -= 1.0f));
    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::consume) == (result -= 1.0f));
    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::release) == (result -= 1.0f));
    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::acquireRelease) == (result -= 1.0f));
    TEST_ENSURE(a.FetchSub(1.0f, MemoryOrder::sequential) == (result -= 1.0f));

    return 0;
}

int32 TestPointerFetchOperators()
{
    StaticArray<int32, 7> sarr;
    auto iter = sarr.Begin();
    Atomic<int32*> a = iter.Address();

    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::relaxed) == (iter++).Address());
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::acquire) == (iter++).Address());
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::consume) == (iter++).Address());
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::release) == (iter++).Address());
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::acquireRelease) == (iter++).Address());
    TEST_ENSURE(a.FetchAdd(1, MemoryOrder::sequential) == (iter++).Address());

    TEST_ENSURE(a.FetchSub(1, MemoryOrder::relaxed) == (iter--).Address());
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::acquire) == (iter--).Address());
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::consume) == (iter--).Address());
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::release) == (iter--).Address());
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::acquireRelease) == (iter--).Address());
    TEST_ENSURE(a.FetchSub(1, MemoryOrder::sequential) == (iter--).Address());

    return 0;
}

int32 TestProducerConsumer()
{
    Atomic<uint32> atomic = 0;
    uint32 consumerCount = 0;
    uint32 producerCount = 0;

    auto consumer = [&atomic, &consumerCount](uint32 end) {
        for (uint32 value = 0; value < end; ++consumerCount, ++value) {
            atomic.Wait(value, MemoryOrder::acquire);
        }
    };

    auto producer = [&atomic, &producerCount](uint32 end) {
        for (uint32 value = 0; value < end; ++producerCount, ++value) {
            atomic.FetchAdd(1, MemoryOrder::release);
            atomic.Notify();
        }
    };

    constexpr uint32 count = NumericLimit<int32>::max >> 8;
    auto t1 = std::thread(producer, count);
    auto t2 = std::thread(consumer, count);
    t1.join();
    t2.join();

    TEST_ENSURE(atomic.Load(MemoryOrder::acquire) == count);
    TEST_ENSURE(consumerCount == count);
    TEST_ENSURE(producerCount == count);

    return 0;
}

template <typename T>
int32 TestSpinLock()
{
    Atomic<int32> atomic(1);
    Atomic<int32> lock(0);

    auto spinLock = [&atomic, &lock]() -> int32 {
        while (lock.Exchange(1, MemoryOrder::acquire) == 1) {
            lock.Wait(1, MemoryOrder::relaxed);
        }

        int32 value = atomic.Load(MemoryOrder::relaxed);
        lock.Store(0, MemoryOrder::release);
        lock.Notify();

        return value;
    };

    std::thread thread([&spinLock]() {
        while (spinLock() > 0) {}
    });

    // test if hangs
    for (uint32 i = 0; i < NumericLimit<uint16>::max; ++i) {
        spinLock();
    }

    atomic.Store(-2, MemoryOrder::sequential);
    thread.join();
    return 0;
}

int main()
{
    TEST_ENSURE(TestAtomicLockFree() == 0);

    TEST_ENSURE(TestLoad<int32>() == 0);
    TEST_ENSURE(TestLoad<Unaligned>() == 0);
    TEST_ENSURE(TestLoad<NonAtomic>() == 0);

    TEST_ENSURE(TestStore<int32>() == 0);
    TEST_ENSURE(TestStore<Unaligned>() == 0);
    TEST_ENSURE(TestStore<NonAtomic>() == 0);

    TEST_ENSURE(TestExchange<int32>() == 0);
    TEST_ENSURE(TestExchange<Unaligned>() == 0);
    TEST_ENSURE(TestExchange<NonAtomic>() == 0);

    TEST_ENSURE(TestCompareExchange<int32>() == 0);
    TEST_ENSURE(TestCompareExchange<Unaligned>() == 0);
    TEST_ENSURE(TestCompareExchange<NonAtomic>() == 0);

    TEST_ENSURE(TestIntegralFetchOperators() == 0);
    TEST_ENSURE(TestFloatingFetchOperators() == 0);
    TEST_ENSURE(TestPointerFetchOperators() == 0);

    TEST_ENSURE(TestProducerConsumer() == 0);

    TEST_ENSURE(TestSpinLock<int32>() == 0);
    TEST_ENSURE(TestSpinLock<Unaligned>() == 0);
    TEST_ENSURE(TestSpinLock<NonAtomic>() == 0);

    return 0;
}