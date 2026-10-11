#include <chrono>

#include "test_macro.h"

#define TEST_TIMEPOINT(x, y) TEST_ENSURE(TimePointEqual(x, y) == true);

import mini.test;

using mini::Days;
using mini::Hours;
using mini::Microseconds;
using mini::Milliseconds;
using mini::Minutes;
using mini::Nanoseconds;
using mini::Seconds;

using mini::Duration;
using mini::Ratio;
using mini::TimePoint;

using StdClock = std::chrono::high_resolution_clock;
template <typename T>
using StdTp = std::chrono::time_point<StdClock, T>;

template <mini::TimePointT T, typename U>
constexpr bool TimePointEqual(T const& x, U const& y)
{
    if constexpr (mini::TimePointT<U>) {
        return x.sinceEpoch() == y.sinceEpoch();
    } else {
        return x.sinceEpoch().count() == y.time_since_epoch().count();
    }
}

template <mini::TimePointT T, typename U>
constexpr int TestTimePoint()
{
    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    TEST_TIMEPOINT(T::min(), T(T::Duration::min()));
    TEST_TIMEPOINT(T::max(), T(T::Duration::max()));

    TEST_TIMEPOINT(T(), U());
    TEST_TIMEPOINT(T(typename T::Duration(1)), U(typename U::duration(1)));
    TEST_TIMEPOINT(T(TestDurationT(1)), U(TestDurationU(1)));

    auto t = T(typename T::Duration(1));
    auto u = U(typename U::duration(1));

    TEST_TIMEPOINT(t += typename T::Duration(2), u += typename U::duration(2));
    TEST_TIMEPOINT(t -= typename T::Duration(2), u -= typename U::duration(2));
    TEST_TIMEPOINT(t += TestDurationT(2), u += TestDurationU(2));
    TEST_TIMEPOINT(t -= TestDurationT(2), u -= TestDurationU(2));

    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTp<FloatDurationU>;

    auto dt = durationCast<FloatDurationT>(t.sinceEpoch());
    auto du = std::chrono::duration_cast<FloatDurationU>(u.time_since_epoch());
    auto ttp = FloatTimePointT(dt);
    auto utp = FloatTimePointU(du);

    TEST_TIMEPOINT(ttp, utp);

    ttp += FloatDurationT(1.6);
    utp += FloatDurationU(1.6);

    TEST_TIMEPOINT(ttp, utp);
    if constexpr (!MSVC) {
        // this seems to be another consteval bug on msvc
        // msvc really sucks at constant evaluations..
        TEST_ENSURE((ttp > t));
    }
    TEST_TIMEPOINT(floor<typename T::Duration>(ttp), t + typename T::Duration(1));
    TEST_TIMEPOINT(floor<typename T::Duration>(ttp), std::chrono::floor<typename U::duration>(utp));
    TEST_TIMEPOINT(ceil<typename T::Duration>(ttp), t + typename T::Duration(2));
    TEST_TIMEPOINT(ceil<typename T::Duration>(ttp), std::chrono::ceil<typename U::duration>(utp));
    TEST_TIMEPOINT(round<typename T::Duration>(ttp), t + typename T::Duration(2));
    TEST_TIMEPOINT(round<typename T::Duration>(ttp), std::chrono::round<typename U::duration>(utp));

    return 0;
}

int main()
{
    static_assert(TestTimePoint<TimePoint<Nanoseconds>, StdTp<std::chrono::nanoseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Microseconds>, StdTp<std::chrono::microseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Milliseconds>, StdTp<std::chrono::milliseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Seconds>, StdTp<std::chrono::seconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Minutes>, StdTp<std::chrono::minutes>>() == 0);
    static_assert(TestTimePoint<TimePoint<Hours>, StdTp<std::chrono::hours>>() == 0);
    static_assert(TestTimePoint<TimePoint<Days>, StdTp<std::chrono::days>>() == 0);

    return 0;
}