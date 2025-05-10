#include <benchmark/benchmark.h>
#include <string>

import mini.benchmark;

using namespace mini;

static void CtorEmpty(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        benchmark::DoNotOptimize(str);
    }
}

static void CtorEmpty_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        benchmark::DoNotOptimize(str);
    }
}

static void CtorShort(benchmark::State& state)
{
    for (auto _ : state) {
        String str("Hello world!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorShort_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("Hello world!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorLong_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorIter(benchmark::State& state)
{
    String str("Hello world!");
    for (auto _ : state) {
        String str2(str.Begin(), str.End());
        benchmark::DoNotOptimize(str2);
    }
}

static void CtorIter_stdlib(benchmark::State& state)
{
    std::string str("Hello world!");
    for (auto _ : state) {
        std::string str2(str.begin(), str.end());
        benchmark::DoNotOptimize(str2);
    }
}

BENCHMARK(CtorEmpty);
BENCHMARK(CtorEmpty_stdlib);
BENCHMARK(CtorShort);
BENCHMARK(CtorShort_stdlib);
BENCHMARK(CtorLong);
BENCHMARK(CtorLong_stdlib);
BENCHMARK(CtorIter);
BENCHMARK(CtorIter_stdlib);

static void AssignOther(benchmark::State& state)
{
    String str("Hello world!");
    for (auto _ : state) {
        String str2;
        str2 = str;
    }
}

static void AssignOther_stdlib(benchmark::State& state)
{
    std::string str("Hello world!");
    for (auto _ : state) {
        std::string str2;
        str2 = str;
    }
}

static void AssignLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str = "Hello world from long string!";
    }
}

static void AssignLong_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str = "Hello world from long string!";
    }
}

BENCHMARK(AssignOther);
BENCHMARK(AssignOther_stdlib);
BENCHMARK(AssignLong);
BENCHMARK(AssignLong_stdlib);

static void Append(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Append("Hello world!");
        str.Append("Hello world from long string!");
    }
}

static void Append_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.append("Hello world!");
        str.append("Hello world from long string!");
    }
}

BENCHMARK(Append);
BENCHMARK(Append_stdlib);

static void Insert(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Insert(0, "Hello world!");
        str.Insert(5, "Hello world from long string!");
    }
}

static void Insert_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.insert(0, "Hello world!");
        str.insert(5, "Hello world from long string!");
    }
}

BENCHMARK(Insert);
BENCHMARK(Insert_stdlib);

BENCHMARK_MAIN();