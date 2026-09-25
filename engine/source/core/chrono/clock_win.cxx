module;

#include "win_include.h"

export module mini.core:clock_platform;

import :type;
import :ignore;
import :duration;
import :time_point;

namespace mini {

inline LARGE_INTEGER GetFrequency() noexcept
{
    LARGE_INTEGER freq;
    ignore = QueryPerformanceFrequency(&freq);
    return freq;
}

template <DurationT T>
inline TimePoint<T> ClockNow() noexcept
{
    constexpr int64 _10MHz = 10'000'000; // common on X86/X64
    constexpr int64 _24MHz = 24'000'000; // common on ARM64

    static const LARGE_INTEGER freq = GetFrequency();

    LARGE_INTEGER count;
    ignore = QueryPerformanceCounter(&count);

    if (freq.QuadPart == _10MHz) {
        constexpr int64 multiplier = Nanoseconds::Period::denom / _10MHz;
        return TimePoint<T>(Nanoseconds(count.QuadPart * multiplier));
    } else if (freq.QuadPart == _24MHz) {
        const int64 main = (count.QuadPart / _24MHz) * Nanoseconds::Period::denom;
        const int64 sub = (count.QuadPart % _24MHz) * Nanoseconds::Period::denom / _24MHz;
        return TimePoint<T>(Nanoseconds(main + sub));
    }

    const int64 second = count.QuadPart / freq.QuadPart;
    const int64 fraction = count.QuadPart % freq.QuadPart;
    const int64 duration = (second * Nanoseconds::Period::denom) +
                           (fraction * Nanoseconds::Period::denom / freq.QuadPart);

    return TimePoint<T>(Nanoseconds(duration));
}

} // namespace mini