#include <chrono>
#include <unistd.h>

#include "test_macro.h"

import mini.test;

template <mini::SizeT MicroSecN>
void MiniClock()
{
    mini::Clock::TimePoint start = mini::Clock::Now();

    if constexpr (MicroSecN != 0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = MicroSecN * 100;
        nanosleep(&ts, nullptr);
    }

    mini::Clock::TimePoint end = mini::Clock::Now();
    mini::Clock::Duration diff = end - start;

    mini::test::logger.Log("[{}us] Elapsed: {}ns", MicroSecN, diff.Count());
    static_assert(mini::SameAsT<decltype(diff), mini::NanoSeconds>);
}

template <mini::SizeT MicroSecN>
void StdClock()
{
    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (MicroSecN != 0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = MicroSecN * 100;
        nanosleep(&ts, nullptr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;

    mini::test::logger.Log("[{}us] Standard Elapsed: {}ns", MicroSecN, diff.count());
    static_assert(mini::SameAsT<decltype(diff), std::chrono::nanoseconds>);
}

int main()
{
    MiniClock<0>();
    StdClock<0>();

    MiniClock<1000>();
    StdClock<1000>();

    return 0;
}