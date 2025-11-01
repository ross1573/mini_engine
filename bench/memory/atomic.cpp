#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

import mini.benchmark;

using namespace mini;
using namespace mini::memory;

#if PLATFORM_MACOS || PLATFORM_WINDWOS
using ContentionT = int64;
#else
using ContentionT = int32;
#endif

struct NonAtomic {
public:
    int32 v[5];

    NonAtomic() noexcept = default;
    NonAtomic(int32 n) noexcept { FillRange(&v[0], &v[4], n); }
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
        sum += a.Load(MemoryOrder::acquire);
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
        sum += a.load(std::memory_order::acquire);
    }

    t.join();
    benchmark::DoNotOptimize(sum);
}

BENCHMARK_TEMPLATE(AtomicLoad, ContentionT);
BENCHMARK_TEMPLATE(AtomicLoad_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicLoad, NonAtomic);
BENCHMARK_TEMPLATE(AtomicLoad_std, NonAtomic);

template <typename T>
    requires ConstructibleFromT<T, int32> && ConvertibleToT<T, int32>
static void AtomicStore(benchmark::State& state)
{
    Atomic<T> a(1);
    std::thread t([&a]() {
        while (a.Exchange(-1, MemoryOrder::relaxed) < 0) {}
    });

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
    std::thread t([&a]() {
        while (a.exchange(-1, std::memory_order::relaxed) < 0) {}
    });

    for (auto _ : state) {
        a.store(1, std::memory_order::release);
    }

    t.join();
    benchmark::DoNotOptimize(a.load(std::memory_order::seq_cst));
}

BENCHMARK_TEMPLATE(AtomicStore, ContentionT);
BENCHMARK_TEMPLATE(AtomicStore_std, ContentionT);
BENCHMARK_TEMPLATE(AtomicStore, NonAtomic);
BENCHMARK_TEMPLATE(AtomicStore_std, NonAtomic);

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
BENCHMARK_TEMPLATE(AtomicWaitNotify, NonAtomic);
BENCHMARK_TEMPLATE(AtomicWaitNotify_std, NonAtomic);

BENCHMARK_MAIN();