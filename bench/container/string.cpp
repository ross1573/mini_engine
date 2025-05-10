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
        String str("Hello from string!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorShort_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("Hello from string!");
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
    String str("Hello from string!");
    for (auto _ : state) {
        String str2(str.Begin(), str.End());
        benchmark::DoNotOptimize(str2);
    }
}

static void CtorIter_stdlib(benchmark::State& state)
{
    std::string str("Hello from string!");
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
    String str("Hello from string!");
    String str2("Hello world from long string!");
    for (auto _ : state) {
        String str3;
        str3 = str;
        str3 = str2;
        benchmark::DoNotOptimize(str3);
    }
}

static void AssignOther_stdlib(benchmark::State& state)
{
    std::string str("Hello from string!");
    std::string str2("Hello world from long string!");
    for (auto _ : state) {
        std::string str3;
        str3 = str;
        str3 = str2;
        benchmark::DoNotOptimize(str3);
    }
}

static void AssignShort(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str = "Hello from string!";
        str = "Hello from string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignShort_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str = "Hello from string!";
        str = "Hello from string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str = "Hello world from long string!";
        str = "Hello world from long string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignLong_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str = "Hello world from long string!";
        str = "Hello world from long string!";
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(AssignOther);
BENCHMARK(AssignOther_stdlib);
BENCHMARK(AssignShort);
BENCHMARK(AssignShort_stdlib);
BENCHMARK(AssignLong);
BENCHMARK(AssignLong_stdlib);

static void Append(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Append("Hello from string!");
        str.Append("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void Append_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.append("Hello from string!");
        str.append("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(Append);
BENCHMARK(Append_stdlib);

static void Insert(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Insert(0, "Hello from string!");
        str.Insert(5, "Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void Insert_stdlib(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.insert(0, "Hello from string!");
        str.insert(5, "Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(Insert);
BENCHMARK(Insert_stdlib);

BENCHMARK_MAIN();