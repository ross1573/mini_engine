module;

#include <sys/time.h>

export module mini.core:clock_platform;

import :type;
import :duration;
import :time_point;

namespace mini {

template <DurationT T>
inline TimePoint<T> ClockNow() noexcept
{
    struct timespec ts;
    VERIFY(clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == 0, "clock_gettime of CLOCK_MONOTONIC_RAW");
    return TimePoint<T>(Seconds(ts.tv_sec) + NanoSeconds(ts.tv_nsec));
}

} // namespace mini