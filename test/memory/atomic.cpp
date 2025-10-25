#include <chrono>
#include <thread>

import mini.test;

using namespace mini;
using namespace mini::memory;
using namespace mini::test;

template <SizeT CapacityN>
struct S {
    alignas(1) byte buffer[CapacityN];
};

static bool TestAtomicLockFree()
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

    // TODO: is this supported by all platforms that we're targeting..?
    TEST_ENSURE(Atomic<S<1>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<2>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<3>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<4>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<5>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<8>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<9>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<16>>::IsLockFree() == true);
    TEST_ENSURE(Atomic<S<17>>::IsLockFree() == false);

    TEST_ENSURE(Atomic<S<1>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<2>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<3>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<4>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<5>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<8>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<9>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<16>>::IsAlwaysLockFree() == true);
    TEST_ENSURE(Atomic<S<17>>::IsAlwaysLockFree() == false);

    return 0;
}

Atomic<uint32> atomic = 0;
Array<uint32> result;

void TestWait(uint32 start, uint32 end)
{
    for (; start < end;) {
        atomic.Wait(start, MemoryOrder::acquire);
        start = atomic.Load(MemoryOrder::acquire);
        result.Push(start);
        atomic.Store(++start, MemoryOrder::release);
        atomic.Notify();
    }
}

int main()
{
    TEST_ENSURE(TestAtomicLockFree() == 0);

    for (uint32 n = 0; n < 10; ++n) {
        result.Clear();
        atomic.Store(0, MemoryOrder::sequential);

        auto t1 = std::thread(&TestWait, 0, 100);
        auto t2 = std::thread(&TestWait, 1, 100);
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