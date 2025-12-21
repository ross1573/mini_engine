#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

import mini.memory;
import mini.array;
import mini.thread;

using namespace mini;

#if PLATFORM_MACOS || PLATFORM_WINDOWS
using ContentionT = int64;
#else
using ContentionT = int32;
#endif

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

static void NoOp(benchmark::State& state)
{
    for (; state.KeepRunning(););
}

BENCHMARK(NoOp);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicLoad(benchmark::State& state)
{
    Atomic<T> a(1);
    int32 sum = 0;
    std::thread t([&a]() {
        while (a.Load(MemoryOrder::relaxed) < 0) {}
    });

    for (auto _ : state) {
        sum += static_cast<int32>(a.Load(MemoryOrder::acquire));
    }

    t.join();
    benchmark::DoNotOptimize(sum);
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicLoad_std(benchmark::State& state)
{
    std::atomic<T> a(1);
    int32 sum = 0;
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) {}
    });

    for (auto _ : state) {
        sum += static_cast<int32>(a.load(std::memory_order::acquire));
    }

    t.join();
    benchmark::DoNotOptimize(sum);
}

BENCHMARK_TEMPLATE(AtomicLoad, ContentionT);
BENCHMARK_TEMPLATE(AtomicLoad_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicLoad, Unaligned);
BENCHMARK_TEMPLATE(AtomicLoad_std, Unaligned);
BENCHMARK_TEMPLATE(AtomicLoad, NonAtomic);
BENCHMARK_TEMPLATE(AtomicLoad_std, NonAtomic);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicStore(benchmark::State& state)
{
    Atomic<T> a(1);
    std::thread t([&a]() { a.Exchange(-1, MemoryOrder::relaxed); });

    for (auto _ : state) {
        a.Store(1, MemoryOrder::release);
    }

    t.join();
    benchmark::DoNotOptimize(a.Load(MemoryOrder::sequential));
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicStore_std(benchmark::State& state)
{
    std::atomic<T> a(1);
    std::thread t([&a]() { a.exchange(-1, std::memory_order::relaxed); });

    for (auto _ : state) {
        a.store(1, std::memory_order::release);
    }

    t.join();
    benchmark::DoNotOptimize(a.load(std::memory_order::seq_cst));
}

BENCHMARK_TEMPLATE(AtomicStore, ContentionT);
BENCHMARK_TEMPLATE(AtomicStore_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicStore, Unaligned);
BENCHMARK_TEMPLATE(AtomicStore_std, Unaligned);
BENCHMARK_TEMPLATE(AtomicStore, NonAtomic);
BENCHMARK_TEMPLATE(AtomicStore_std, NonAtomic);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicCompareExchange(benchmark::State& state)
{
    Atomic<T> a(1);
    std::thread t([&a]() {
        while (a.Load(MemoryOrder::relaxed) < 0) {}
    });

    for (auto _ : state) {
        T value = 1;
        a.CompareExchangeStrong(value, 1, MemoryOrder::acquireRelease);
    }

    t.join();
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicCompareExchange_std(benchmark::State& state)
{
    std::atomic<T> a(1);
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) {}
    });

    for (auto _ : state) {
        T value = 1;
        a.compare_exchange_strong(value, 1, std::memory_order::acq_rel);
    }

    t.join();
}

BENCHMARK_TEMPLATE(AtomicCompareExchange, ContentionT);
BENCHMARK_TEMPLATE(AtomicCompareExchange_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicCompareExchange, Unaligned);
BENCHMARK_TEMPLATE(AtomicCompareExchange_std, Unaligned);
BENCHMARK_TEMPLATE(AtomicCompareExchange, NonAtomic);
BENCHMARK_TEMPLATE(AtomicCompareExchange_std, NonAtomic);

static void AtomicFalseWait(benchmark::State& state)
{
    Atomic<int32> a(1);
    std::thread t([&a]() {
        while (a.Load(MemoryOrder::relaxed) < 0) {}
    });

    for (auto _ : state) {
        a.Wait(0, MemoryOrder::sequential);
        benchmark::DoNotOptimize(a);
    }

    t.join();
}

static void AtomicFalseWait_std(benchmark::State& state)
{
    std::atomic<int32> a(1);
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) {}
    });

    for (auto _ : state) {
        a.wait(0);
        benchmark::DoNotOptimize(a);
    }

    t.join();
}

BENCHMARK(AtomicFalseWait);
BENCHMARK(AtomicFalseWait_std);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicWait(benchmark::State& state)
{
    Atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<int32>(value) >= 0) {
            if (a.CompareExchangeWeak(value, 1, MemoryOrder::acquire)) {
                a.Notify();
            }

            value = a.Load(MemoryOrder::acquire);
        }
    });

    for (auto _ : state) {
        a.Wait(0, MemoryOrder::acquire);
        a.Store(0, MemoryOrder::release);
    }

    a.Store(-2, MemoryOrder::sequential);
    t.join();
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicWait_std(benchmark::State& state)
{
    std::atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<int32>(value) >= 0) {
            if (a.compare_exchange_weak(value, 1, std::memory_order::acquire)) {
                a.notify_one();
            }

            value = a.load(std::memory_order::acquire);
        }
    });

    for (auto _ : state) {
        a.wait(0, std::memory_order::acquire);
        a.store(0, std::memory_order::release);
    }

    a.store(-2, std::memory_order::seq_cst);
    t.join();
}

BENCHMARK_TEMPLATE(AtomicWait, ContentionT);
BENCHMARK_TEMPLATE(AtomicWait_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicWait, Unaligned);
BENCHMARK_TEMPLATE(AtomicWait_std, Unaligned);
BENCHMARK_TEMPLATE(AtomicWait, NonAtomic);
BENCHMARK_TEMPLATE(AtomicWait_std, NonAtomic);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicWaitNotify(benchmark::State& state)
{
    Atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<int32>(value) >= 0) {
            if (a.CompareExchangeWeak(value, 1, MemoryOrder::acquire)) {
                a.Notify();
            }

            a.Wait(1, MemoryOrder::acquire);
            value = a.Load(MemoryOrder::acquire);
        }
    });

    for (auto _ : state) {
        a.Wait(0, MemoryOrder::acquire);
        a.Store(0, MemoryOrder::release);
        a.Notify();
    }

    a.Store(-2, MemoryOrder::sequential);
    a.Notify();
    t.join();
}

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicWaitNotify_std(benchmark::State& state)
{
    std::atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<int32>(value) >= 0) {
            if (a.compare_exchange_weak(value, 1, std::memory_order::acquire)) {
                a.notify_one();
            }

            a.wait(1, std::memory_order::acquire);
            value = a.load(std::memory_order::acquire);
        }
    });

    for (auto _ : state) {
        a.wait(0, std::memory_order::acquire);
        a.store(0, std::memory_order::release);
        a.notify_one();
    }

    a.store(-2, std::memory_order::seq_cst);
    a.notify_one();
    t.join();
}

BENCHMARK_TEMPLATE(AtomicWaitNotify, ContentionT);
BENCHMARK_TEMPLATE(AtomicWaitNotify_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicWaitNotify, Unaligned);
BENCHMARK_TEMPLATE(AtomicWaitNotify_std, Unaligned);
BENCHMARK_TEMPLATE(AtomicWaitNotify, NonAtomic);
BENCHMARK_TEMPLATE(AtomicWaitNotify_std, NonAtomic);

template <SizeT ThreadN>
static void AtomicSpinLock(benchmark::State& state)
{
    Atomic<int32> atomic(1);
    Atomic<int32> lock(0);
    Array<std::thread> threads(ThreadN);

    auto spinLock = [&atomic, &lock]() -> int32 {
        while (lock.Exchange(1, MemoryOrder::acquire) == 1) {
            lock.Wait(1, MemoryOrder::relaxed);
        }

        int32 value = atomic.Load(MemoryOrder::relaxed);
        lock.Store(0, MemoryOrder::release);
        lock.Notify();

        return value;
    };

    for (SizeT i = 0; i < ThreadN; ++i) {
        threads.Push(std::thread([&spinLock]() {
            while (spinLock() > 0) {}
        }));
    }

    for (auto _ : state) {
        spinLock();
    }

    atomic.Store(-1, MemoryOrder::sequential);

    for (auto& t : threads) {
        t.join();
    }
}

template <SizeT ThreadN>
static void AtomicSpinLock_std(benchmark::State& state)
{
    std::atomic<int32> atomic(1);
    std::atomic<int32> lock(0);
    Array<std::thread> threads(ThreadN);

    auto spinLock = [&atomic, &lock]() -> int32 {
        while (lock.exchange(1, std::memory_order::acquire) == 1) {
            lock.wait(1, std::memory_order::relaxed);
        }

        int32 value = atomic.load(std::memory_order::relaxed);
        lock.store(0, std::memory_order::release);
        lock.notify_one();

        return value;
    };

    for (SizeT i = 0; i < ThreadN; ++i) {
        threads.Push(std::thread([&spinLock]() {
            while (spinLock() > 0) {}
        }));
    }

    for (auto _ : state) {
        spinLock();
    }

    atomic.store(-1, std::memory_order::seq_cst);

    for (auto& t : threads) {
        t.join();
    }
}

BENCHMARK_TEMPLATE(AtomicSpinLock, 4);
BENCHMARK_TEMPLATE(AtomicSpinLock, 8);

BENCHMARK_TEMPLATE(AtomicSpinLock_std, 4);
BENCHMARK_TEMPLATE(AtomicSpinLock_std, 8);

// seems like macos kernel panics when trying to join the waiting thread
// same thing also accurs when using the libc++ implementation
#if !PLATFORM_MACOS
BENCHMARK_TEMPLATE(AtomicSpinLock, 16);
BENCHMARK_TEMPLATE(AtomicSpinLock, 32);

BENCHMARK_TEMPLATE(AtomicSpinLock_std, 16);
BENCHMARK_TEMPLATE(AtomicSpinLock_std, 32);
#endif

BENCHMARK_MAIN();