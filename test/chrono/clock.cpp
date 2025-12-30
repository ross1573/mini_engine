#include <chrono>

#include "test_macro.h"

#if PLATFORM_MACOS
#  include <unistd.h>
#  define SLEEP(x)                 \
      struct timespec ts;          \
      ts.tv_sec = 0;               \
      ts.tv_nsec = x * 1000 * 100; \
      nanosleep(&ts, nullptr)
#elif PLATFORM_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRA_LEAN
#  include <windows.h>
#  define SLEEP(x) Sleep(x)
#endif

import mini.test;

template <mini::SizeT MilliSecT>
void MiniClock()
{
    mini::Clock::TimePoint start = mini::Clock::Now();

    if constexpr (MilliSecT != 0) {
        SLEEP(MilliSecT);
    }

    mini::Clock::TimePoint end = mini::Clock::Now();
    mini::Clock::Duration diff = end - start;

    mini::test::logger.Log("[{}ms] Elapsed: {}ns", MilliSecT, diff.Count());
    static_assert(mini::SameAsT<decltype(diff), mini::NanoSeconds>);
}

template <mini::SizeT MilliSecT>
void StdClock()
{
    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (MilliSecT != 0) {
        SLEEP(MilliSecT);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;

    mini::test::logger.Log("[{}ms] Standard Elapsed: {}ns", MilliSecT, diff.count());
    static_assert(mini::SameAsT<decltype(diff), std::chrono::nanoseconds>);
}

int main()
{
    MiniClock<0>();
    StdClock<0>();

    MiniClock<1>();
    StdClock<1>();

    return 0;
}