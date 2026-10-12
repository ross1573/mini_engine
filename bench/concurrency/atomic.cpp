#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

import mini.core;

using mini::Atomic;
using mini::MemoryOrder;

#if PLATFORM_MACOS || PLATFORM_WINDOWS
using ContentionT = mini::int64;
#else
using ContentionT = mini::int32;
#endif

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

void NoOp(benchmark::State& state)
{
    for (; state.KeepRunning();) { }
}

BENCHMARK(NoOp);

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicLoad(benchmark::State& state)
{
    Atomic<T> a(1);
    mini::int32 sum = 0;
    std::thread t([&a]() {
        while (a.load(MemoryOrder::Relaxed) < 0) { }
    });

    for (auto _ : state) {
        sum += static_cast<mini::int32>(a.load(MemoryOrder::Acquire));
    }

    t.join();
    benchmark::DoNotOptimize(sum);
}

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicLoad_std(benchmark::State& state)
{
    std::atomic<T> a(1);
    mini::int32 sum = 0;
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) { }
    });

    for (auto _ : state) {
        sum += static_cast<mini::int32>(a.load(std::memory_order::acquire));
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
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicStore(benchmark::State& state)
{
    Atomic<T> a(1);
    std::thread t([&a]() { a.exchange(-1, MemoryOrder::Relaxed); });

    for (auto _ : state) {
        a.store(1, MemoryOrder::Release);
    }

    t.join();
    benchmark::DoNotOptimize(a.load(MemoryOrder::Sequential));
}

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicStore_std(benchmark::State& state)
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
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicCompareExchange(benchmark::State& state)
{
    Atomic<T> a(1);
    std::thread t([&a]() {
        while (a.load(MemoryOrder::Relaxed) < 0) { }
    });

    for (auto _ : state) {
        T value = 1;
        a.compareExchangeStrong(value, 1, MemoryOrder::AcquireRelease);
    }

    t.join();
}

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicCompareExchange_std(benchmark::State& state)
{
    std::atomic<T> a(1);
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) { }
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

void AtomicFalseWait(benchmark::State& state)
{
    Atomic<mini::int32> a(1);
    std::thread t([&a]() {
        while (a.load(MemoryOrder::Relaxed) < 0) { }
    });

    for (auto _ : state) {
        a.wait(0, MemoryOrder::Sequential);
        benchmark::DoNotOptimize(a);
    }

    t.join();
}

void AtomicFalseWait_std(benchmark::State& state)
{
    std::atomic<mini::int32> a(1);
    std::thread t([&a]() {
        while (a.load(std::memory_order::relaxed) < 0) { }
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
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicWait(benchmark::State& state)
{
    Atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<mini::int32>(value) >= 0) {
            if (a.compareExchangeWeak(value, 1, MemoryOrder::Acquire)) {
                a.notify();
            }

            value = a.load(MemoryOrder::Acquire);
        }
    });

    for (auto _ : state) {
        a.wait(0, MemoryOrder::Acquire);
        a.store(0, MemoryOrder::Release);
    }

    a.store(-2, MemoryOrder::Sequential);
    t.join();
}

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicWait_std(benchmark::State& state)
{
    std::atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<mini::int32>(value) >= 0) {
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
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicWaitNotify(benchmark::State& state)
{
    Atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<mini::int32>(value) >= 0) {
            if (a.compareExchangeWeak(value, 1, MemoryOrder::Acquire)) {
                a.notify();
            }

            a.wait(1, MemoryOrder::Acquire);
            value = a.load(MemoryOrder::Acquire);
        }
    });

    for (auto _ : state) {
        a.wait(0, MemoryOrder::Acquire);
        a.store(0, MemoryOrder::Release);
        a.notify();
    }

    a.store(-2, MemoryOrder::Sequential);
    a.notify();
    t.join();
}

template <typename T>
    requires mini::ConstructibleFromT<T, mini::int32> && mini::ConvertibleToT<T, mini::int32>
void AtomicWaitNotify_std(benchmark::State& state)
{
    std::atomic<T> a(0);
    std::thread t([&a]() {
        T value = 1;
        while (static_cast<mini::int32>(value) >= 0) {
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

template <size_t ThreadN>
void AtomicSpinLock(benchmark::State& state)
{
    Atomic<mini::int32> atomic(1);
    Atomic<mini::int32> lock(0);
    mini::Array<std::thread> threads(ThreadN);

    auto spinLock = [&atomic, &lock]() -> mini::int32 {
        while (lock.exchange(1, MemoryOrder::Acquire) == 1) {
            lock.wait(1, MemoryOrder::Relaxed);
        }

        mini::int32 value = atomic.load(MemoryOrder::Relaxed);
        lock.store(0, MemoryOrder::Release);
        lock.notify();

        return value;
    };

    for (size_t i = 0; i < ThreadN; ++i) {
        threads.pushBack(std::thread([&spinLock]() {
            while (spinLock() > 0) { }
        }));
    }

    for (auto _ : state) {
        spinLock();
    }

    atomic.store(-1, MemoryOrder::Sequential);

    for (auto& t : threads) {
        t.join();
    }
}

template <size_t ThreadN>
void AtomicSpinLock_std(benchmark::State& state)
{
    std::atomic<mini::int32> atomic(1);
    std::atomic<mini::int32> lock(0);
    mini::Array<std::thread> threads(ThreadN);

    auto spinLock = [&atomic, &lock]() -> mini::int32 {
        while (lock.exchange(1, std::memory_order::acquire) == 1) {
            lock.wait(1, std::memory_order::relaxed);
        }

        mini::int32 value = atomic.load(std::memory_order::relaxed);
        lock.store(0, std::memory_order::release);
        lock.notify_one();

        return value;
    };

    for (size_t i = 0; i < ThreadN; ++i) {
        threads.pushBack(std::thread([&spinLock]() {
            while (spinLock() > 0) { }
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