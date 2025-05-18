#include <benchmark/benchmark.h>
#include <string>

import mini.benchmark;

using namespace mini;

static void NoOp(benchmark::State& state)
{
    for (; state.KeepRunning(););
}

BENCHMARK(NoOp);

static void CtorEmpty(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        benchmark::DoNotOptimize(str);
    }
}

static void CtorEmpty_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        benchmark::DoNotOptimize(str);
    }
}

static void CtorCh(benchmark::State& state)
{
    for (auto _ : state) {
        String str('c', 22);
        benchmark::DoNotOptimize(str);
    }
}

static void CtorCh_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str(22, 'c');
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

static void CtorShort_std(benchmark::State& state)
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

static void CtorLong_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void CtorCopy(benchmark::State& state)
{
    String str("Hello from string!");
    for (auto _ : state) {
        String str2(str);
        benchmark::DoNotOptimize(str2);
    }
}

static void CtorCopy_std(benchmark::State& state)
{
    std::string str("Hello from string!");
    for (auto _ : state) {
        std::string str2(str);
        benchmark::DoNotOptimize(str2);
    }
}

static void CtorMove(benchmark::State& state)
{
    String str("Hello world from long string!");
    for (auto _ : state) {
        String str2(MoveArg(str));
        memory::DestructAt(&str);
        memory::ConstructAt(&str, MoveArg(str2));
        benchmark::DoNotOptimize(str);
        benchmark::DoNotOptimize(str2);
    }
}

static void CtorMove_std(benchmark::State& state)
{
    std::string str("Hello world from long string!");
    for (auto _ : state) {
        std::string str2(MoveArg(str));
        memory::DestructAt(&str);
        memory::ConstructAt(&str, MoveArg(str2));
        benchmark::DoNotOptimize(str);
        benchmark::DoNotOptimize(str2);
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

static void CtorIter_std(benchmark::State& state)
{
    std::string str("Hello from string!");
    for (auto _ : state) {
        std::string str2(str.begin(), str.end());
        benchmark::DoNotOptimize(str2);
    }
}

BENCHMARK(CtorEmpty);
BENCHMARK(CtorEmpty_std);
BENCHMARK(CtorCh);
BENCHMARK(CtorCh_std);
BENCHMARK(CtorShort);
BENCHMARK(CtorShort_std);
BENCHMARK(CtorLong);
BENCHMARK(CtorLong_std);
BENCHMARK(CtorCopy);
BENCHMARK(CtorCopy_std);
BENCHMARK(CtorMove);
BENCHMARK(CtorMove_std);
BENCHMARK(CtorIter);
BENCHMARK(CtorIter_std);

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

static void AssignOther_std(benchmark::State& state)
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

static void AssignCopy(benchmark::State& state)
{
    String str("Hello from string!");
    for (auto _ : state) {
        String str2(str);
        benchmark::DoNotOptimize(str2);
    }
}

static void AssignCopy_std(benchmark::State& state)
{
    std::string str("Hello from string!");
    for (auto _ : state) {
        std::string str2(str);
        benchmark::DoNotOptimize(str2);
    }
}

static void AssignMove(benchmark::State& state)
{
    String str("Hello world from long string!");
    for (auto _ : state) {
        String str2;
        str2 = MoveArg(str);
        str = MoveArg(str2);
        benchmark::DoNotOptimize(str2);
    }
}

static void AssignMove_std(benchmark::State& state)
{
    std::string str("Hello world from long string!");
    for (auto _ : state) {
        std::string str2;
        str2 = MoveArg(str);
        str = MoveArg(str2);
        benchmark::DoNotOptimize(str2);
    }
}

static void AssignShort(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str = "Hello from string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignShort_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str = "Hello from string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str = "Hello world from long string!";
        benchmark::DoNotOptimize(str);
    }
}

static void AssignLong_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str = "Hello world from long string!";
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(AssignOther);
BENCHMARK(AssignOther_std);
BENCHMARK(AssignCopy);
BENCHMARK(AssignCopy_std);
BENCHMARK(AssignMove);
BENCHMARK(AssignMove_std);
BENCHMARK(AssignShort);
BENCHMARK(AssignShort_std);
BENCHMARK(AssignLong);
BENCHMARK(AssignLong_std);

static void AppendShort(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Append("Hello from string!");
        benchmark::DoNotOptimize(str);
    }
}

static void AppendShort_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.append("Hello from string!");
        benchmark::DoNotOptimize(str);
    }
}

static void AppendLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Append("Hello from string!");
        str.Append("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void AppendLong_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.append("Hello from string!");
        str.append("Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(AppendShort);
BENCHMARK(AppendShort_std);
BENCHMARK(AppendLong);
BENCHMARK(AppendLong_std);

static void InsertShort(benchmark::State& state)
{
    for (auto _ : state) {
        String str('c', 3);
        str.Insert(0, "Hello from string!");
        benchmark::DoNotOptimize(str);
    }
}

static void InsertShort_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str(3, 'c');
        str.insert(0, "Hello from string!");
        benchmark::DoNotOptimize(str);
    }
}

static void InsertLong(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        str.Insert(0, "Hello from string!");
        str.Insert(5, "Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

static void InsertLong_std(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        str.insert(0, "Hello from string!");
        str.insert(5, "Hello world from long string!");
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(InsertShort);
BENCHMARK(InsertShort_std);
BENCHMARK(InsertLong);
BENCHMARK(InsertLong_std);

BENCHMARK_MAIN();