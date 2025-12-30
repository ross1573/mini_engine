export module mini.core:clock;

import :type;
import :duration;
import :time_point;
import :clock_platform;

namespace mini {

export class CORE_API Clock {
public:
    typedef NanoSeconds Duration;
    typedef TimePoint<Duration> TimePoint;

public:
    static TimePoint Now() noexcept { return ClockNow<Duration>(); }
};

} // namespace mini