#include <chrono>
#include <thread>

#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::memory;
using namespace mini::test;

template <SizeT CapacityN>
struct S {
    alignas(1) byte buffer[CapacityN];
};

struct NonAtomic {
public:
    int32 v[5];

    NonAtomic() noexcept = default;
    NonAtomic(int32 n) noexcept { FillRange(&v[0], &v[4], n); }
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

    static_assert(Atomic<S<1>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<2>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<3>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<4>>::IsAlwaysLockFree() == true);
    static_assert(Atomic<S<5>>::IsAlwaysLockFree() == Atomic<S<8>>::IsAlwaysLockFree());
    static_assert(Atomic<S<9>>::IsAlwaysLockFree() == Atomic<S<16>>::IsAlwaysLockFree());
    static_assert(Atomic<S<8>>::IsAlwaysLockFree() != Atomic<S<17>>::IsAlwaysLockFree());

    TEST_ENSURE(Atomic<S<1>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<2>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<3>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<4>>{}.IsLockFree() == true);
    TEST_ENSURE(Atomic<S<5>>{}.IsLockFree() == Atomic<S<8>>{}.IsLockFree());
    TEST_ENSURE(Atomic<S<9>>{}.IsLockFree() == Atomic<S<16>>{}.IsLockFree());
    TEST_ENSURE(Atomic<S<8>>{}.IsLockFree() != Atomic<S<17>>{}.IsLockFree());

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

    return 0;
}

int32 TestWaitNotifyIncrement()
{
    Atomic<uint32> atomic = 0;
    Array<uint32> result;

    auto waitNotify = [&atomic, &result](uint32 start, uint32 end) {
        for (; start < end;) {
            atomic.Wait(start, MemoryOrder::acquire);
            start = atomic.Load(MemoryOrder::acquire);
            result.Push(start);
            atomic.Store(++start, MemoryOrder::release);
            atomic.Notify();
        }
    };

    for (uint32 n = 0; n < 10; ++n) {
        result.Clear();
        atomic.Store(0, MemoryOrder::sequential);

        auto t1 = std::thread(waitNotify, 0, 100);
        auto t2 = std::thread(waitNotify, 1, 100);
        t1.join();
        t2.join();

        TEST_ENSURE(atomic.Load(MemoryOrder::acquire) == 101);
        TEST_ENSURE(result.Size() == 101);
        for (SizeT i = 0; i < result.Size(); ++i) {
            TEST_ENSURE(i == result[i]);
        }
    }

    return 0;
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
int32 TestWaitNotify()
{
    Atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<int32>(value) >= 0) {
            a.CompareExchangeWeak(value, 1, MemoryOrder::acquire);
            a.Notify();
            a.Wait(1, MemoryOrder::acquire);
            value = a.Load(MemoryOrder::acquire);
        }
    });

    // test if hangs
    for (uint32 i = 0; i < NumericLimit<uint32>::max >> 12; ++i) {
        a.Wait(0, MemoryOrder::acquire);
        a.Store(0, MemoryOrder::release);
        a.Notify();
    }

    a.Store(-2, MemoryOrder::sequential);
    a.Notify();
    t.join();
    return 0;
}

int main()
{
    TEST_ENSURE(TestAtomicLockFree() == 0);

    TEST_ENSURE(TestWaitNotifyIncrement() == 0);
    TEST_ENSURE(TestWaitNotify<int32>() == 0);
    TEST_ENSURE(TestWaitNotify<NonAtomic>() == 0);

    return 0;
}