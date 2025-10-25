#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

import mini.benchmark;

using namespace mini;
using namespace mini::memory;

static void NoOp(benchmark::State& state)
{
    for (; state.KeepRunning(););
}

BENCHMARK(NoOp);

static void AtomicStoreLoad(benchmark::State& state)
{
    Atomic<int32> a;
    for (auto _ : state) {
        a.Store(10, MemoryOrder::release);
        auto tmp = a.Load(MemoryOrder::acquire);
        benchmark::DoNotOptimize(tmp);
    }
}

static void AtomicStoreLoad_std(benchmark::State& state)
{
    std::atomic<int32> a;
    for (auto _ : state) {
        a.store(10, std::memory_order::release);
        auto tmp = a.load(std::memory_order::acquire);
        benchmark::DoNotOptimize(tmp);
    }
}

BENCHMARK(AtomicStoreLoad);
BENCHMARK(AtomicStoreLoad_std);

static void AtomicFalseWait(benchmark::State& state)
{
    Atomic<int32> a(1);
    for (auto _ : state) {
        a.Wait(0, MemoryOrder::sequential);
        benchmark::DoNotOptimize(a);
    }
}

static void AtomicFalseWait_std(benchmark::State& state)
{
    std::atomic<int32> a(1);
    for (auto _ : state) {
        a.wait(0);
        benchmark::DoNotOptimize(a);
    }
}

BENCHMARK(AtomicFalseWait);
BENCHMARK(AtomicFalseWait_std);

static void AtomicWait(benchmark::State& state)
{
    Atomic<int32> a(0);
    std::thread t([&a]() {
        int start = 0;
        while (start >= 0) {
            a.Wait(start, MemoryOrder::acquire);
            start = a.Load(MemoryOrder::acquire);
            a.Store(++start, MemoryOrder::release);
            a.Notify();
        }
    });

    auto start = 0;
    for (auto _ : state) {
        start = a.Load(MemoryOrder::acquire);
        a.Store(++start, MemoryOrder::release);
        a.Notify();
        a.Wait(start, MemoryOrder::acquire);
    }

    a.Store(-2, MemoryOrder::sequential);
    a.Notify();
    t.join();
}

static void AtomicWait_std(benchmark::State& state)
{
    std::atomic<int32> a(0);
    std::thread t([&a]() {
        int start = 0;
        while (start >= 0) {
            a.wait(start, std::memory_order::acquire);
            start = a.load(std::memory_order::acquire);
            a.store(++start, std::memory_order::release);
            a.notify_one();
        }
    });

    auto start = 0;
    for (auto _ : state) {
        start = a.load(std::memory_order::acquire);
        a.store(++start, std::memory_order::release);
        a.notify_one();
        a.wait(start, std::memory_order::acquire);
    }

    a.store(-2, std::memory_order::seq_cst);
    a.notify_one();
    t.join();
}

BENCHMARK(AtomicWait);
BENCHMARK(AtomicWait_std);

BENCHMARK_MAIN();