#include <chrono>

#include "test_macro.h"

#define TEST_TIMEPOINT(x, y) TEST_ENSURE(timePointEqual(x, y) == true);

import mini.test;

using mini::Days;
using mini::Duration;
using mini::durationCast;
using mini::FloatMinutes;
using mini::FloatSeconds;
using mini::Hours;
using mini::Microseconds;
using mini::Milliseconds;
using mini::Minutes;
using mini::Nanoseconds;
using mini::Ratio;
using mini::Seconds;
using mini::TimePoint;
using mini::timePointCast;
using mini::TimePointT;

using StdClock = std::chrono::high_resolution_clock;

template <typename T>
using StdTimePoint = std::chrono::time_point<StdClock, T>;

template <TimePointT T, typename U>
constexpr bool timePointEqual(T const& x, U const& y)
{
    if constexpr (TimePointT<U>) {
        return x.sinceEpoch() == y.sinceEpoch();
    } else {
        return x.sinceEpoch().count() == y.time_since_epoch().count();
    }
}

template <TimePointT T, typename U>
constexpr int testCtor()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;

    TEST_TIMEPOINT(T(), U());
    TEST_TIMEPOINT(T(DurationT(1)), U(DurationU(1)));
    TEST_TIMEPOINT(T(DurationT(-1)), U(DurationU(-1)));

    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    TEST_TIMEPOINT(T(TestDurationT(1)), U(TestDurationU(1)));
    TEST_TIMEPOINT(T(TestDurationT(-3)), U(TestDurationU(-3)));

    return 0;
}

template <TimePointT T, typename U>
constexpr int testLimits()
{
    TEST_TIMEPOINT(T::min(), U::min());
    TEST_TIMEPOINT(T::max(), U::max());

    return 0;
}

template <TimePointT T, typename U>
constexpr int testAdd()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;

    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    auto t = T(DurationT(1));
    auto u = U(DurationU(1));

    TEST_TIMEPOINT(t + DurationT(3), u + DurationU(3));
    TEST_TIMEPOINT(DurationT(3) + t, DurationU(3) + u);
    TEST_TIMEPOINT(t + TestDurationT(1), u + TestDurationU(1));

    TEST_TIMEPOINT(t += DurationT(2), u += DurationU(2));
    TEST_TIMEPOINT(t, u);
    TEST_TIMEPOINT(t += TestDurationT(2), u += TestDurationU(2));
    TEST_TIMEPOINT(t, u);

    return 0;
}

template <TimePointT T, typename U>
constexpr int testSubtract()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;

    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    auto t = T(DurationT(9));
    auto u = U(DurationU(9));

    TEST_TIMEPOINT(t - DurationT(3), u - DurationU(3));
    TEST_TIMEPOINT(t - TestDurationT(1), u - TestDurationU(1));

    TEST_TIMEPOINT(t -= DurationT(2), u -= DurationU(2));
    TEST_TIMEPOINT(t, u);
    TEST_TIMEPOINT(t -= TestDurationT(2), u -= TestDurationU(2));
    TEST_TIMEPOINT(t, u);

    return 0;
}

template <TimePointT T, typename>
constexpr int testDifference()
{
    using DurationT = T::Duration;

    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;

    auto t = T(DurationT(1));

    TEST_ENSURE(((t + DurationT(5)) - t).count() == 5);
    TEST_ENSURE((t - (t + DurationT(5))).count() == -5);
    TEST_ENSURE((t - t) == DurationT::zero());
    TEST_ENSURE(((t + TestDurationT(1)) - t) == TestDurationT(1));

    return 0;
}

template <TimePointT T, typename>
constexpr int testEqual()
{
    using DurationT = T::Duration;

    TEST_ENSURE(T(DurationT(1)) == T(DurationT(1)));
    TEST_ENSURE(T() == T(DurationT::zero()));
    TEST_ENSURE(!(T(DurationT(1)) == T(DurationT(2))));

    return 0;
}

template <TimePointT T, typename>
constexpr int testLess()
{
    using DurationT = T::Duration;

    TEST_ENSURE(T(DurationT(1)) < T(DurationT(2)));
    TEST_ENSURE(T::min() < T());
    TEST_ENSURE(T() < T::max());
    TEST_ENSURE(!(T(DurationT(1)) < T(DurationT(1))));

    return 0;
}

template <TimePointT T, typename U>
constexpr int testFloatConvert()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTimePoint<FloatDurationU>;

    auto t = T(DurationT(1));
    auto u = U(DurationU(1));

    auto ttp = FloatTimePointT(durationCast<FloatDurationT>(t.sinceEpoch()));
    auto utp = FloatTimePointU(std::chrono::duration_cast<FloatDurationU>(u.time_since_epoch()));

    TEST_TIMEPOINT(ttp, utp);
    TEST_ENSURE(ttp == t);

    ttp += FloatDurationT(1.6);
    utp += FloatDurationU(1.6);

    TEST_TIMEPOINT(ttp, utp);

    if constexpr (!MSVC) {
        TEST_ENSURE((ttp > t));
        TEST_ENSURE((t < ttp));
    }

    return 0;
}

template <TimePointT T, typename U>
constexpr int testTimePointCast()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTimePoint<FloatDurationU>;

    auto ttp = FloatTimePointT(FloatDurationT(2.6));
    auto utp = FloatTimePointU(FloatDurationU(2.6));

    TEST_TIMEPOINT(timePointCast<DurationT>(ttp), std::chrono::time_point_cast<DurationU>(utp));

    ttp = FloatTimePointT(FloatDurationT(-1.4));
    utp = FloatTimePointU(FloatDurationU(-1.4));

    TEST_TIMEPOINT(timePointCast<DurationT>(ttp), std::chrono::time_point_cast<DurationU>(utp));

    return 0;
}

template <TimePointT T, typename U>
constexpr int testFloor()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTimePoint<FloatDurationU>;

    auto ttp = FloatTimePointT(FloatDurationT(2.6));
    auto utp = FloatTimePointU(FloatDurationU(2.6));

    TEST_TIMEPOINT(floor<DurationT>(ttp), std::chrono::floor<DurationU>(utp));

    ttp = FloatTimePointT(FloatDurationT(-1.4));
    utp = FloatTimePointU(FloatDurationU(-1.4));

    TEST_TIMEPOINT(floor<DurationT>(ttp), std::chrono::floor<DurationU>(utp));

    return 0;
}

template <TimePointT T, typename U>
constexpr int testCeil()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTimePoint<FloatDurationU>;

    auto ttp = FloatTimePointT(FloatDurationT(2.6));
    auto utp = FloatTimePointU(FloatDurationU(2.6));

    TEST_TIMEPOINT(ceil<DurationT>(ttp), std::chrono::ceil<DurationU>(utp));

    ttp = FloatTimePointT(FloatDurationT(-1.4));
    utp = FloatTimePointU(FloatDurationU(-1.4));

    TEST_TIMEPOINT(ceil<DurationT>(ttp), std::chrono::ceil<DurationU>(utp));

    return 0;
}

template <TimePointT T, typename U>
constexpr int testRound()
{
    using DurationT = T::Duration;
    using DurationU = U::duration;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;
    using FloatTimePointT = TimePoint<FloatDurationT>;
    using FloatTimePointU = StdTimePoint<FloatDurationU>;

    auto ttp = FloatTimePointT(FloatDurationT(2.6));
    auto utp = FloatTimePointU(FloatDurationU(2.6));

    TEST_TIMEPOINT(round<DurationT>(ttp), std::chrono::round<DurationU>(utp));

    ttp = FloatTimePointT(FloatDurationT(-1.4));
    utp = FloatTimePointU(FloatDurationU(-1.4));

    TEST_TIMEPOINT(round<DurationT>(ttp), std::chrono::round<DurationU>(utp));

    return 0;
}

constexpr int testConvert()
{
    TimePoint<Minutes> minutes(Minutes(3));
    TimePoint<Seconds> seconds(minutes);
    TEST_ENSURE(seconds.sinceEpoch().count() == 180);

    TimePoint<FloatSeconds> floatSeconds(TimePoint<Milliseconds>(Milliseconds(1500)));
    TEST_ENSURE(floatSeconds.sinceEpoch().count() == 1.5);

    TEST_ENSURE(TimePoint<Seconds>(TimePoint<Days>(Days(-1))).sinceEpoch().count() == -86400);

    return 0;
}

constexpr int testLargeConvert()
{
    TimePoint<Nanoseconds> nanoseconds(TimePoint<Days>(Days(100000)));
    TEST_ENSURE(nanoseconds.sinceEpoch().count() == 8640000000000000000LL);
    TEST_ENSURE(nanoseconds == TimePoint<Days>(Days(100000)));
    TEST_ENSURE(timePointCast<Days>(nanoseconds).sinceEpoch().count() == 100000);

    TEST_ENSURE((TimePoint<Minutes>(Minutes(40000000)) + Seconds(0)).sinceEpoch().count() == 2400000000LL);
    TEST_ENSURE((TimePoint<Minutes>(Minutes(40000000)) - TimePoint<Seconds>()).count() == 2400000000LL);

    return 0;
}

constexpr int testTimePointCastUnit()
{
    TimePoint<Seconds> seconds(Seconds(90));

    TEST_ENSURE(timePointCast<Milliseconds>(seconds).sinceEpoch().count() == 90000);
    TEST_ENSURE(timePointCast<Minutes>(seconds).sinceEpoch().count() == 1);

    return 0;
}

constexpr int testRoundingUnit()
{
    TEST_ENSURE(floor<Minutes>(TimePoint<Seconds>(Seconds(90))).sinceEpoch().count() == 1);
    TEST_ENSURE(floor<Minutes>(TimePoint<Seconds>(Seconds(-30))).sinceEpoch().count() == -1);

    TEST_ENSURE(ceil<Minutes>(TimePoint<Seconds>(Seconds(90))).sinceEpoch().count() == 2);
    TEST_ENSURE(ceil<Minutes>(TimePoint<Seconds>(Seconds(-30))).sinceEpoch().count() == 0);

    TEST_ENSURE(round<Minutes>(TimePoint<Seconds>(Seconds(150))).sinceEpoch().count() == 2);
    TEST_ENSURE(round<Minutes>(TimePoint<Seconds>(Seconds(-90))).sinceEpoch().count() == -2);

    return 0;
}

constexpr int testArithmetic()
{
    TimePoint<Seconds> seconds(Seconds(90));
    TimePoint<Milliseconds> later(Milliseconds(90500));

    TEST_ENSURE((seconds + Milliseconds(500)).sinceEpoch().count() == 90500);
    TEST_ENSURE((Minutes(1) + seconds).sinceEpoch().count() == 150);
    TEST_ENSURE((seconds - Hours(1)).sinceEpoch().count() == -3510);

    TEST_ENSURE((later - seconds).count() == 500);
    TEST_ENSURE((seconds - later).count() == -500);

    return 0;
}

constexpr int testCompoundChain()
{
    TimePoint<Seconds> seconds(Seconds(10));

    TEST_ENSURE(((seconds += Seconds(5)) += Minutes(1)).sinceEpoch().count() == 75);
    TEST_ENSURE(seconds.sinceEpoch().count() == 75);
    TEST_ENSURE(((seconds -= Seconds(70)) -= Seconds(10)).sinceEpoch().count() == -5);
    TEST_ENSURE(seconds.sinceEpoch().count() == -5);

    return 0;
}

constexpr int testFloatMixed()
{
    TimePoint<FloatSeconds> floatSeconds(FloatSeconds(1.5));
    TimePoint<Milliseconds> milliseconds(Milliseconds(500));

    TEST_ENSURE((floatSeconds - milliseconds).count() == 1.0);
    TEST_ENSURE((milliseconds - floatSeconds).count() == -1.0);
    TEST_ENSURE((floatSeconds + Milliseconds(250)).sinceEpoch().count() == 1.75);
    TEST_ENSURE((Milliseconds(250) + floatSeconds).sinceEpoch().count() == 1.75);
    TEST_ENSURE((floatSeconds - Milliseconds(250)).sinceEpoch().count() == 1.25);

    TEST_ENSURE(milliseconds < floatSeconds);
    TEST_ENSURE(TimePoint<FloatSeconds>(FloatSeconds(0.5)) == milliseconds);
    TEST_ENSURE((floatSeconds <=> TimePoint<Milliseconds>(Milliseconds(1500))) == 0);

    TEST_ENSURE(timePointCast<FloatMinutes>(TimePoint<Seconds>(Seconds(90))).sinceEpoch().count() == 1.5);

    return 0;
}

constexpr int testComparison()
{
    TEST_ENSURE(TimePoint<Seconds>(Seconds(90)) == TimePoint<Milliseconds>(Milliseconds(90000)));
    TEST_ENSURE(TimePoint<Hours>(Hours(24)) == TimePoint<Days>(Days(1)));
    TEST_ENSURE(TimePoint<Seconds>(Seconds(90)) != TimePoint<Milliseconds>(Milliseconds(90001)));
    TEST_ENSURE(TimePoint<Seconds>(Seconds(90)) < TimePoint<Milliseconds>(Milliseconds(90001)));
    TEST_ENSURE(TimePoint<Seconds>(Seconds(90)) > TimePoint<Minutes>(Minutes(1)));

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testLimits, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testAdd, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testDifference, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testEqual, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testLess, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testFloatConvert, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testTimePointCast, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testFloor, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testCeil, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Nanoseconds>, StdTimePoint<std::chrono::nanoseconds>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Microseconds>, StdTimePoint<std::chrono::microseconds>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Milliseconds>, StdTimePoint<std::chrono::milliseconds>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Seconds>, StdTimePoint<std::chrono::seconds>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Minutes>, StdTimePoint<std::chrono::minutes>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Hours>, StdTimePoint<std::chrono::hours>);
    TEST_CONSTEXPR_TEMPLATE(testRound, TimePoint<Days>, StdTimePoint<std::chrono::days>);

    TEST_CONSTEXPR(testConvert);
    TEST_CONSTEXPR(testLargeConvert);

    TEST_CONSTEXPR(testTimePointCastUnit);
    TEST_CONSTEXPR(testRoundingUnit);

    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testCompoundChain);
    TEST_CONSTEXPR(testFloatMixed);

    TEST_CONSTEXPR(testComparison);

    return 0;
}
