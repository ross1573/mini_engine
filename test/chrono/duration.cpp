#include <chrono>

#include "test_macro.h"

#define TEST_DURATION(x, y) TEST_ENSURE(DurationEqual(x, y) == true);

import mini.test;

using mini::Days;
using mini::Hours;
using mini::Microseconds;
using mini::Milliseconds;
using mini::Minutes;
using mini::Nanoseconds;
using mini::Seconds;

using mini::Duration;
using mini::NumericLimit;
using mini::Ratio;

template <mini::DurationT T, typename U>
constexpr bool DurationEqual(T const& x, U const& y)
{
    if constexpr (mini::DurationT<U>) {
        return x.count() == y.count();
    } else {
        return x.count() == y.count();
    }
}

template <mini::DurationT T, typename U>
constexpr int TestDuration()
{
    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    TEST_DURATION(T::zero(), T(0));
    TEST_DURATION(T::max(), T(NumericLimit<typename T::Value>::max));
    TEST_DURATION(T::min(), T(NumericLimit<typename T::Value>::min));

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
    TEST_DURATION(t += TestDurationT(2), u += TestDurationU(2));
    TEST_DURATION(t -= TestDurationT(2), u -= TestDurationU(2));

    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    auto dt = durationCast<FloatDurationT>(t);
    auto du = std::chrono::duration_cast<FloatDurationU>(u);

    TEST_ENSURE(dt.count() == 1.0);
    TEST_ENSURE(du.count() == 1.0);
    TEST_ENSURE(durationCast<T>(dt).count() == 1);

    dt = FloatDurationT(1.5);
    du = FloatDurationU(1.5);

    TEST_ENSURE(durationCast<T>(dt).count() == 1);
    if constexpr (!MSVC) {
        // this seems to be another consteval bug on msvc
        // msvc really sucks at constant evaluations..
        TEST_ENSURE(dt > t);
    }
    TEST_DURATION(floor<T>(dt), std::chrono::floor<U>(du));
    TEST_DURATION(ceil<T>(dt), std::chrono::ceil<U>(du));
    TEST_DURATION(round<T>(dt), std::chrono::round<U>(du));
    TEST_DURATION(abs(-dt), std::chrono::abs(-du));

    return 0;
}

int main()
{
    static_assert(TestDuration<Nanoseconds, std::chrono::nanoseconds>() == 0);
    static_assert(TestDuration<Microseconds, std::chrono::microseconds>() == 0);
    static_assert(TestDuration<Milliseconds, std::chrono::milliseconds>() == 0);
    static_assert(TestDuration<Seconds, std::chrono::seconds>() == 0);
    static_assert(TestDuration<Minutes, std::chrono::minutes>() == 0);
    static_assert(TestDuration<Hours, std::chrono::hours>() == 0);
    static_assert(TestDuration<Days, std::chrono::days>() == 0);

    return 0;
}