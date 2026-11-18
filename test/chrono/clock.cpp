#include <chrono>

#include "test_macro.h"

#if PLATFORM_MACOS
#  include <time.h>
#  define SLEEP(x)                                         \
      {                                                    \
          struct timespec ts;                              \
          ts.tv_sec = 0;                                   \
          ts.tv_nsec = static_cast<long>(x) * 1000 * 1000; \
          nanosleep(&ts, nullptr);                         \
      }
#elif PLATFORM_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRA_LEAN
#  include <windows.h>
#  define SLEEP(x) Sleep(x)
#endif

import mini.test;

using mini::Clock;
using mini::Milliseconds;

int testMonotonic()
{
    Clock::TimePoint first = Clock::now();
    TEST_ENSURE(first > Clock::TimePoint());

    Clock::TimePoint previous = first;
    for (mini::int32 i = 0; i < 10000; ++i) {
        Clock::TimePoint current = Clock::now();
        TEST_ENSURE(current >= previous);
        previous = current;
    }

    return 0;
}

int testRate()
{
    auto outerStart = std::chrono::steady_clock::now();
    Clock::TimePoint start = Clock::now();
    SLEEP(20);
    Clock::TimePoint end = Clock::now();
    auto outerEnd = std::chrono::steady_clock::now();

    Clock::Duration inner = end - start;
    mini::int64 outer = std::chrono::duration_cast<std::chrono::nanoseconds>(outerEnd - outerStart).count();

    TEST_ENSURE(inner >= Milliseconds(5));
    TEST_ENSURE(inner.count() <= outer + 2000000);

    return 0;
}

int main()
{
    TEST_FUNCTION(testMonotonic);
    TEST_FUNCTION(testRate);

    return 0;
}
