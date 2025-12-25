#include <chrono>

#include "test_macro.h"

#define TEST_DURATION(x, y) TEST_ENSURE(DurationEqual(x, y) == true);

import mini.test;

using namespace mini;

template <DurationT T, typename U>
constexpr bool DurationEqual(T const& x, U const& y)
{
    if constexpr (DurationT<U>) {
        return x.Count() == y.Count();
    }
    else {
        return x.Count() == y.count();
    }
}

template <DurationT T, typename U>
constexpr int TestDuration()
{
    constexpr int64 num = T::Period::num * 2;
    using TestDurationT = Duration<int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<int64, std::ratio<num, 1>>;

    TEST_DURATION(T::Zero(), T(0));
    TEST_DURATION(T::Max(), T(NumericLimit<typename T::Value>::max));
    TEST_DURATION(T::Min(), T(NumericLimit<typename T::Value>::min));

    auto t = T(1);
    auto u = U(1);

    TEST_DURATION(T(0), U(0));
    TEST_DURATION(T(t), U(u));
    TEST_DURATION(T(TestDurationT(1)), U(TestDurationU(1)));

    TEST_DURATION(+t, +u);
    TEST_DURATION(-t, -u);
    TEST_DURATION(t++, u++);
    TEST_DURATION(t--, u--);
    TEST_DURATION(++t, ++u);
    TEST_DURATION(--t, --u);

    TEST_DURATION(t += T(2), u += U(2));
    TEST_DURATION(t -= T(2), u -= U(2));

    using FloatDurationT = Duration<float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<float64, typename U::period>;

    auto dt = DurationCast<FloatDurationT>(t);
    auto du = std::chrono::duration_cast<FloatDurationU>(u);

    TEST_ENSURE(dt.Count() == 1.0);
    TEST_ENSURE(du.count() == 1.0);
    TEST_ENSURE(DurationCast<T>(dt).Count() == 1);

    dt = FloatDurationT(1.5);
    du = FloatDurationU(1.5);

    TEST_ENSURE(DurationCast<T>(dt).Count() == 1);
    TEST_ENSURE(dt > t);
    TEST_DURATION(Floor<T>(dt), std::chrono::floor<U>(du));
    TEST_DURATION(Ceil<T>(dt), std::chrono::ceil<U>(du));
    TEST_DURATION(Round<T>(dt), std::chrono::round<U>(du));
    TEST_DURATION(Abs(-dt), std::chrono::abs(-du));

    return 0;
}

int main()
{
    static_assert(TestDuration<NanoSeconds, std::chrono::nanoseconds>() == 0);
    static_assert(TestDuration<MicroSeconds, std::chrono::microseconds>() == 0);
    static_assert(TestDuration<MilliSeconds, std::chrono::milliseconds>() == 0);
    static_assert(TestDuration<Seconds, std::chrono::seconds>() == 0);
    static_assert(TestDuration<Minutes, std::chrono::minutes>() == 0);
    static_assert(TestDuration<Hours, std::chrono::hours>() == 0);
    static_assert(TestDuration<Days, std::chrono::days>() == 0);
    static_assert(TestDuration<Weeks, std::chrono::weeks>() == 0);
    static_assert(TestDuration<Months, std::chrono::months>() == 0);
    static_assert(TestDuration<Years, std::chrono::years>() == 0);

    return 0;
}