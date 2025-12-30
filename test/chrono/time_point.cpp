#include <chrono>

#include "test_macro.h"

#define TEST_TIMEPOINT(x, y) TEST_ENSURE(TimePointEqual(x, y) == true);

import mini.test;

using namespace mini;

using StdClock = std::chrono::high_resolution_clock;
template <typename T>
using StdTp = std::chrono::time_point<StdClock, T>;

template <TimePointT T, typename U>
constexpr bool TimePointEqual(T const& x, U const& y)
{
    if constexpr (TimePointT<U>) {
        return x.SinceEpoch() == y.SinceEpoch();
    }
    else {
        return x.SinceEpoch().Count() == y.time_since_epoch().count();
    }
}

template <TimePointT T, typename U>
constexpr int TestTimePoint()
{
    constexpr int64 num = T::Period::num * 2;
    using TestDurationT = Duration<int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<int64, std::ratio<num, 1>>;

    TEST_TIMEPOINT(T::Min(), T(T::Duration::Min()));
    TEST_TIMEPOINT(T::Max(), T(T::Duration::Max()));

    TEST_TIMEPOINT(T(), U());
    TEST_TIMEPOINT(T(typename T::Duration(1)), U(typename U::duration(1)));
    TEST_TIMEPOINT(T(TestDurationT(1)), U(TestDurationU(1)));

    auto t = T(typename T::Duration(1));
    auto u = U(typename U::duration(1));

    TEST_TIMEPOINT(t += typename T::Duration(2), u += typename U::duration(2));
    TEST_TIMEPOINT(t -= typename T::Duration(2), u -= typename U::duration(2));
    TEST_TIMEPOINT(t += TestDurationT(2), u += TestDurationU(2));
    TEST_TIMEPOINT(t -= TestDurationT(2), u -= TestDurationU(2));

    using FloatDurationT = Duration<float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTp<FloatDurationU>;

    auto dt = DurationCast<FloatDurationT>(t.SinceEpoch());
    auto du = std::chrono::duration_cast<FloatDurationU>(u.time_since_epoch());
    auto ttp = FloatTimePointT(dt);
    auto utp = FloatTimePointU(du);

    TEST_TIMEPOINT(ttp, utp);

    ttp += FloatDurationT(1.6);
    utp += FloatDurationU(1.6);

    TEST_TIMEPOINT(ttp, utp);
    TEST_ENSURE(ttp > t);
    TEST_TIMEPOINT(Floor<typename T::Duration>(ttp), t + typename T::Duration(1));
    TEST_TIMEPOINT(Floor<typename T::Duration>(ttp), std::chrono::floor<typename U::duration>(utp));
    TEST_TIMEPOINT(Ceil<typename T::Duration>(ttp), t + typename T::Duration(2));
    TEST_TIMEPOINT(Ceil<typename T::Duration>(ttp), std::chrono::ceil<typename U::duration>(utp));
    TEST_TIMEPOINT(Round<typename T::Duration>(ttp), t + typename T::Duration(2));
    TEST_TIMEPOINT(Round<typename T::Duration>(ttp), std::chrono::round<typename U::duration>(utp));

    return 0;
}

int main()
{
    static_assert(TestTimePoint<TimePoint<NanoSeconds>, StdTp<std::chrono::nanoseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<MicroSeconds>, StdTp<std::chrono::microseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<MilliSeconds>, StdTp<std::chrono::milliseconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Seconds>, StdTp<std::chrono::seconds>>() == 0);
    static_assert(TestTimePoint<TimePoint<Minutes>, StdTp<std::chrono::minutes>>() == 0);
    static_assert(TestTimePoint<TimePoint<Hours>, StdTp<std::chrono::hours>>() == 0);
    static_assert(TestTimePoint<TimePoint<Days>, StdTp<std::chrono::days>>() == 0);

    return 0;
}