#include <chrono>

#include "test_macro.h"

#define TEST_DURATION(x, y) TEST_ENSURE(durationEqual(x, y) == true);

import mini.test;

using mini::Days;
using mini::Duration;
using mini::durationCast;
using mini::DurationT;
using mini::FloatDays;
using mini::FloatHours;
using mini::FloatMicroseconds;
using mini::FloatMilliseconds;
using mini::FloatMinutes;
using mini::FloatNanoseconds;
using mini::FloatSeconds;
using mini::Hours;
using mini::Microseconds;
using mini::Milliseconds;
using mini::Minutes;
using mini::Nanoseconds;
using mini::Ratio;
using mini::Seconds;

using NinetySeconds = Duration<mini::int64, Ratio<90, 1>>;
using Thirds = Duration<mini::int64, Ratio<1, 3>>;
using Halves = Duration<mini::int64, Ratio<1, 2>>;
using Sixths = Duration<mini::int64, Ratio<1, 6>>;
using WideMinutes = Duration<mini::int64, Ratio<60, 1>>;

template <DurationT T, typename U>
constexpr bool durationEqual(T const& x, U const& y)
{
    return x.count() == y.count();
}

template <DurationT T, typename U>
constexpr int testCtor()
{
    TEST_DURATION(T(0), U(0));
    TEST_DURATION(T{}, U{});
    TEST_DURATION(T(static_cast<mini::int8>(5)), U(static_cast<mini::int8>(5)));

    constexpr mini::int64 num = T::Period::num * 2;
    using TestDurationT = Duration<mini::int64, Ratio<num, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<num, 1>>;

    TEST_DURATION(T(TestDurationT(1)), U(TestDurationU(1)));
    TEST_DURATION(T(TestDurationT(-3)), U(TestDurationU(-3)));

    return 0;
}

template <DurationT T, typename U>
constexpr int testLimits()
{
    TEST_DURATION(T::zero(), U::zero());
    TEST_DURATION(T::min(), U::min());
    TEST_DURATION(T::max(), U::max());

    return 0;
}

template <DurationT T, typename U>
constexpr int testUnary()
{
    T t(3);
    U u(3);

    TEST_DURATION(+t, +u);
    TEST_DURATION(-t, -u);
    TEST_DURATION(-T(0), -U(0));

    return 0;
}

template <DurationT T, typename U>
constexpr int testIncrement()
{
    T t(1);
    U u(1);

    TEST_DURATION(t++, u++);
    TEST_DURATION(t, u);
    TEST_DURATION(++t, ++u);
    TEST_DURATION(t, u);
    TEST_DURATION(t--, u--);
    TEST_DURATION(t, u);
    TEST_DURATION(--t, --u);
    TEST_DURATION(t, u);

    return 0;
}

template <DurationT T, typename U>
constexpr int testAdd()
{
    using TestDurationT = Duration<mini::int64, Ratio<T::Period::num * 2, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<T::Period::num * 2, 1>>;

    TEST_DURATION(T(5) + T(3), U(5) + U(3));
    TEST_DURATION(T(1) + TestDurationT(1), U(1) + TestDurationU(1));

    T t(1);
    U u(1);

    TEST_DURATION(t += T(2), u += U(2));
    TEST_DURATION(t += TestDurationT(2), u += TestDurationU(2));

    return 0;
}

template <DurationT T, typename U>
constexpr int testSubtract()
{
    using TestDurationT = Duration<mini::int64, Ratio<T::Period::num * 2, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<T::Period::num * 2, 1>>;

    TEST_DURATION(T(5) - T(8), U(5) - U(8));
    TEST_DURATION(TestDurationT(1) - T(1), TestDurationU(1) - U(1));

    T t(9);
    U u(9);

    TEST_DURATION(t -= T(2), u -= U(2));
    TEST_DURATION(t -= TestDurationT(2), u -= TestDurationU(2));

    return 0;
}

template <DurationT T, typename U>
constexpr int testMultiply()
{
    TEST_DURATION(T(5) * 3, U(5) * 3);
    TEST_DURATION(3 * T(5), 3 * U(5));
    TEST_DURATION(T(-5) * 3, U(-5) * 3);

    T t(2);
    U u(2);

    TEST_DURATION(t *= 6, u *= 6);

    return 0;
}

template <DurationT T, typename U>
constexpr int testDivide()
{
    using TestDurationT = Duration<mini::int64, Ratio<T::Period::num * 2, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<T::Period::num * 2, 1>>;

    TEST_DURATION(T(15) / 4, U(15) / 4);
    TEST_ENSURE(T(15) / T(4) == U(15) / U(4));
    TEST_ENSURE(TestDurationT(3) / T(2) == TestDurationU(3) / U(2));

    T t(12);
    U u(12);

    TEST_DURATION(t /= 2, u /= 2);

    T n(-17);
    U nu(-17);

    TEST_DURATION(n /= 5, nu /= 5);
    TEST_DURATION(n /= -3, nu /= -3);

    return 0;
}

template <DurationT T, typename U>
constexpr int testModulo()
{
    using TestDurationT = Duration<mini::int64, Ratio<T::Period::num * 2, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<T::Period::num * 2, 1>>;

    TEST_DURATION(T(15) % 4, U(15) % 4);
    TEST_DURATION(T(15) % T(4), U(15) % U(4));
    TEST_DURATION(TestDurationT(5) % T(3), TestDurationU(5) % U(3));

    T t(7);
    U u(7);

    TEST_DURATION(t %= 4, u %= 4);
    TEST_DURATION(t = T(7), u = U(7));
    TEST_DURATION(t %= T(4), u %= U(4));

    T n(-17);
    U nu(-17);

    TEST_DURATION(n %= 5, nu %= 5);
    TEST_DURATION(n = T(-17), nu = U(-17));
    TEST_DURATION(n %= T(-5), nu %= U(-5));
    TEST_DURATION(n = T(17), nu = U(17));
    TEST_DURATION(n %= -5, nu %= -5);

    return 0;
}

template <DurationT T, typename U>
constexpr int testAbs()
{
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    for (mini::int32 value : {-7, 0, 7}) {
        TEST_DURATION(abs(T(value)), std::chrono::abs(U(value)));
    }

    if constexpr (!MSVC) {
        for (mini::float64 value : {1.2, 1.5, -1.7, -2.5, 0.0}) {
            TEST_DURATION(abs(FloatDurationT(value)), std::chrono::abs(FloatDurationU(value)));
        }
    }

    return 0;
}

template <DurationT T, typename>
constexpr int testEqual()
{
    using DoubleT = Duration<mini::int64, Ratio<T::Period::num * 2, T::Period::denom>>;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;

    TEST_ENSURE(T(1) == T(1));
    TEST_ENSURE(DoubleT(1) == T(2));
    TEST_ENSURE(!(T(1) == T(2)));

    if constexpr (!MSVC) {
        TEST_ENSURE(FloatDurationT(1.0) == T(1));
    }

    return 0;
}

template <DurationT T, typename>
constexpr int testLess()
{
    using DoubleT = Duration<mini::int64, Ratio<T::Period::num * 2, T::Period::denom>>;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;

    TEST_ENSURE(T(1) < T(2));
    TEST_ENSURE(T(-1) < T::zero());
    TEST_ENSURE(T::min() < T::max());
    TEST_ENSURE(T(1) < DoubleT(1));
    TEST_ENSURE(!(T(2) < DoubleT(1)));

    if constexpr (!MSVC) {
        TEST_ENSURE(T(1) < FloatDurationT(1.5));
    }

    return 0;
}

template <DurationT T, typename U>
constexpr int testFloatCast()
{
    using FloatDurationT = Duration<mini::float64, typename T::Period>;

    T t(1);

    auto dt = durationCast<FloatDurationT>(t);

    TEST_ENSURE(dt.count() == 1.0);
    TEST_ENSURE(durationCast<T>(dt).count() == 1);
    TEST_ENSURE(FloatDurationT(t) == t);

    return 0;
}

template <DurationT T, typename U>
constexpr int testDurationCast()
{
    using TestDurationT = Duration<mini::int64, Ratio<T::Period::num * 2, 1>>;
    using TestDurationU = std::chrono::duration<mini::int64, std::ratio<T::Period::num * 2, 1>>;
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    for (mini::int32 value : {-7, -1, 0, 1, 7}) {
        TEST_DURATION(durationCast<T>(TestDurationT(value)), std::chrono::duration_cast<U>(TestDurationU(value)));
    }

    if constexpr (!MSVC) {
        for (mini::float64 value : {1.2, 1.5, 1.7, 2.5, -1.2, -1.5, -1.7, -2.5, 0.0}) {
            TEST_DURATION(durationCast<T>(FloatDurationT(value)), std::chrono::duration_cast<U>(FloatDurationU(value)));
        }
    }

    return 0;
}

template <DurationT T, typename U>
constexpr int testFloor()
{
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    for (mini::float64 value : {1.2, 1.5, 1.7, 2.5, -1.2, -1.5, -1.7, -2.5, 0.0}) {
        TEST_DURATION(floor<T>(FloatDurationT(value)), std::chrono::floor<U>(FloatDurationU(value)));
    }

    return 0;
}

template <DurationT T, typename U>
constexpr int testCeil()
{
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    for (mini::float64 value : {1.2, 1.5, 1.7, 2.5, -1.2, -1.5, -1.7, -2.5, 0.0}) {
        TEST_DURATION(ceil<T>(FloatDurationT(value)), std::chrono::ceil<U>(FloatDurationU(value)));
    }

    return 0;
}

template <DurationT T, typename U>
constexpr int testRound()
{
    using FloatDurationT = Duration<mini::float64, typename T::Period>;
    using FloatDurationU = std::chrono::duration<mini::float64, typename U::period>;

    for (mini::float64 value : {1.2, 1.5, 1.7, 2.5, -1.2, -1.5, -1.7, -2.5, 0.0}) {
        TEST_DURATION(round<T>(FloatDurationT(value)), std::chrono::round<U>(FloatDurationU(value)));
    }

    return 0;
}

constexpr int testConvertInteger()
{
    TEST_ENSURE(Milliseconds(Seconds(3)).count() == 3000);
    TEST_ENSURE(Nanoseconds(Microseconds(-2)).count() == -2000);
    TEST_ENSURE(Seconds(Days(1)).count() == 86400);
    TEST_ENSURE(Minutes(Hours(2)).count() == 120);
    TEST_ENSURE(Hours(Days(2)).count() == 48);
    TEST_ENSURE(Seconds(NinetySeconds(2)).count() == 180);

    return 0;
}

constexpr int testConvertFloat()
{
    TEST_ENSURE(FloatSeconds(Milliseconds(1500)).count() == 1.5);
    TEST_ENSURE(FloatMilliseconds(FloatSeconds(0.25)).count() == 250.0);
    TEST_ENSURE(FloatHours(Minutes(90)).count() == 1.5);
    TEST_ENSURE(FloatMinutes(Seconds(90)).count() == 1.5);
    TEST_ENSURE(FloatDays(Hours(36)).count() == 1.5);
    TEST_ENSURE(FloatMicroseconds(Nanoseconds(1500)).count() == 1.5);
    TEST_ENSURE(FloatNanoseconds(Microseconds(2)).count() == 2000.0);

    return 0;
}

constexpr int testValueWidening()
{
    using ShortSeconds = Duration<mini::int16, Ratio<1, 1>>;
    using ShortMilliseconds = Duration<mini::int16, Ratio<1, 1000>>;
    using IntMilliseconds = Duration<mini::int32, Ratio<1, 1000>>;

    TEST_ENSURE(durationCast<IntMilliseconds>(ShortSeconds(static_cast<mini::int16>(30000))).count() == 30000000);
    TEST_ENSURE(durationCast<ShortSeconds>(ShortMilliseconds(static_cast<mini::int16>(30000))).count() == 30);
    TEST_ENSURE(durationCast<ShortSeconds>(ShortMilliseconds(static_cast<mini::int16>(-30000))).count() == -30);

    TEST_ENSURE(Seconds(Days(30000)).count() == 2592000000LL);
    TEST_ENSURE(Nanoseconds(Days(100000)).count() == 8640000000000000000LL);
    TEST_ENSURE(durationCast<Days>(Nanoseconds(8640000000000000000LL)).count() == 100000);

    TEST_ENSURE((Minutes(40000000) + Seconds(0)).count() == 2400000000LL);
    TEST_ENSURE((Seconds(0) - Minutes(40000000)).count() == -2400000000LL);
    TEST_ENSURE(Hours(1000000) == Seconds(3600000000LL));
    TEST_ENSURE(Hours(1000000) < Seconds(3600000001LL));
    TEST_ENSURE((Minutes(2) * mini::int64(3000000000LL)).count() == 6000000000LL);
    TEST_ENSURE((mini::int64(3000000000LL) * Minutes(2)).count() == 6000000000LL);
    TEST_ENSURE((Minutes(7) % mini::int64(4)).count() == 3);

    return 0;
}

constexpr int testDurationCastUnit()
{
    TEST_ENSURE(durationCast<Seconds>(Milliseconds(2999)).count() == 2);
    TEST_ENSURE(durationCast<Seconds>(Milliseconds(-2999)).count() == -2);
    TEST_ENSURE(durationCast<Minutes>(Seconds(119)).count() == 1);
    TEST_ENSURE(durationCast<Days>(Hours(47)).count() == 1);
    TEST_ENSURE(durationCast<Minutes>(NinetySeconds(3)).count() == 4);
    TEST_ENSURE(durationCast<NinetySeconds>(Minutes(4)).count() == 2);
    TEST_ENSURE(durationCast<Milliseconds>(FloatSeconds(1.2345)).count() == 1234);
    TEST_ENSURE(durationCast<Seconds>(Seconds(42)).count() == 42);

    TEST_ENSURE(durationCast<FloatMinutes>(NinetySeconds(3)).count() == 4.5);
    TEST_ENSURE(durationCast<NinetySeconds>(FloatMinutes(3.0)).count() == 2);
    TEST_ENSURE(durationCast<Seconds>(FloatMilliseconds(1500.7)).count() == 1);
    TEST_ENSURE(durationCast<Seconds>(FloatMilliseconds(-1500.7)).count() == -1);
    TEST_ENSURE(durationCast<Minutes>(NinetySeconds(-3)).count() == -4);

    return 0;
}

constexpr int testRoundingUnit()
{
    TEST_ENSURE(floor<Seconds>(Milliseconds(2999)).count() == 2);
    TEST_ENSURE(floor<Seconds>(Milliseconds(-2001)).count() == -3);
    TEST_ENSURE(floor<Seconds>(Milliseconds(-2000)).count() == -2);

    TEST_ENSURE(ceil<Seconds>(Milliseconds(2001)).count() == 3);
    TEST_ENSURE(ceil<Seconds>(Milliseconds(-2999)).count() == -2);
    TEST_ENSURE(ceil<Seconds>(Milliseconds(2000)).count() == 2);

    TEST_ENSURE(round<Seconds>(Milliseconds(1499)).count() == 1);
    TEST_ENSURE(round<Seconds>(Milliseconds(1500)).count() == 2);
    TEST_ENSURE(round<Seconds>(Milliseconds(2500)).count() == 2);
    TEST_ENSURE(round<Seconds>(Milliseconds(2501)).count() == 3);
    TEST_ENSURE(round<Seconds>(Milliseconds(-1500)).count() == -2);
    TEST_ENSURE(round<Seconds>(Milliseconds(-2500)).count() == -2);
    TEST_ENSURE(round<Seconds>(Milliseconds(500)).count() == 0);
    TEST_ENSURE(round<Seconds>(Milliseconds(-500)).count() == 0);
    TEST_ENSURE(round<Seconds>(Milliseconds(-501)).count() == -1);
    TEST_ENSURE(round<Seconds>(Milliseconds(3000)).count() == 3);
    TEST_ENSURE(round<Seconds>(Milliseconds(-3000)).count() == -3);

    TEST_ENSURE(round<Seconds>(FloatSeconds(0.5)).count() == 0);
    TEST_ENSURE(round<Seconds>(FloatSeconds(-0.5)).count() == 0);
    TEST_ENSURE(round<Minutes>(Seconds(90)).count() == 2);
    TEST_ENSURE(round<Minutes>(Seconds(150)).count() == 2);
    TEST_ENSURE(round<Minutes>(Seconds(-90)).count() == -2);

    return 0;
}

constexpr int testNonIntegralPeriod()
{
    TEST_ENSURE(durationCast<Halves>(Thirds(3)).count() == 2);
    TEST_ENSURE(durationCast<Halves>(Thirds(4)).count() == 2);
    TEST_ENSURE(durationCast<Halves>(Thirds(-4)).count() == -2);
    TEST_ENSURE(durationCast<Thirds>(Halves(3)).count() == 4);

    TEST_ENSURE(durationCast<Seconds>(Thirds(7)).count() == 2);
    TEST_ENSURE(durationCast<Seconds>(Thirds(-7)).count() == -2);
    TEST_ENSURE(Thirds(Seconds(2)).count() == 6);

    return 0;
}

constexpr int testArithmetic()
{
    TEST_ENSURE((Seconds(1) + Milliseconds(500)).count() == 1500);
    TEST_ENSURE((Hours(1) + Minutes(1)).count() == 61);
    TEST_ENSURE((Seconds(1) + FloatSeconds(0.5)).count() == 1.5);

    TEST_ENSURE((Minutes(1) - Seconds(30)).count() == 30);

    TEST_ENSURE((Seconds(3) * 1.5).count() == 4.5);
    TEST_ENSURE((2 * Minutes(3)).count() == 6);

    TEST_ENSURE((Seconds(3) / 2.0).count() == 1.5);
    TEST_ENSURE(Hours(3) / Minutes(40) == 4);
    TEST_ENSURE(Milliseconds(2500) / Seconds(1) == 2);
    TEST_ENSURE(FloatSeconds(Milliseconds(2500)) / Seconds(1) == 2.5);

    TEST_ENSURE((Seconds(1) % Milliseconds(300)).count() == 100);
    TEST_ENSURE((Minutes(1) % Seconds(7)).count() == 4);
    TEST_ENSURE((Seconds(-1) % Milliseconds(300)).count() == -100);

    return 0;
}

constexpr int testFloatOperator()
{
    FloatSeconds f(1.5);

    TEST_ENSURE((+f).count() == 1.5);
    TEST_ENSURE((-f).count() == -1.5);

    f = FloatSeconds(0.5);
    TEST_ENSURE((f++).count() == 0.5);
    TEST_ENSURE(f.count() == 1.5);
    TEST_ENSURE((++f).count() == 2.5);

    TEST_ENSURE((f--).count() == 2.5);
    TEST_ENSURE(f.count() == 1.5);
    TEST_ENSURE((--f).count() == 0.5);

    TEST_ENSURE((f += FloatSeconds(0.5)).count() == 1.0);
    TEST_ENSURE((f -= FloatSeconds(0.25)).count() == 0.75);
    TEST_ENSURE((f *= 2.0).count() == 1.5);
    TEST_ENSURE((f /= 4.0).count() == 0.375);

    return 0;
}

constexpr int testMixedCompound()
{
    Seconds s(70);
    TEST_ENSURE((s %= Minutes(1)).count() == 10);
    TEST_ENSURE((s += Minutes(2)).count() == 130);
    TEST_ENSURE((s -= Hours(1)).count() == -3470);
    TEST_ENSURE(s.count() == -3470);

    Milliseconds ms(5);
    TEST_ENSURE(((ms += Seconds(1)) += Seconds(1)).count() == 2005);
    TEST_ENSURE(((ms -= Seconds(2)) -= Milliseconds(5)).count() == 0);
    TEST_ENSURE(((ms += Milliseconds(9)) *= 2).count() == 18);
    TEST_ENSURE(((ms /= 4) %= 3).count() == 1);

    FloatSeconds f(1.0);
    TEST_ENSURE((f += Milliseconds(500)).count() == 1.5);
    TEST_ENSURE((f -= Minutes(1)).count() == -58.5);

    return 0;
}

constexpr int testComparison()
{
    TEST_ENSURE(Seconds(1) == Milliseconds(1000));
    TEST_ENSURE(Hours(24) == Days(1));
    TEST_ENSURE(FloatSeconds(0.5) == Milliseconds(500));

    TEST_ENSURE(Hours(23) != Days(1));

    TEST_ENSURE(Seconds(1) < Milliseconds(1001));
    TEST_ENSURE(Seconds(59) < Minutes(1));
    TEST_ENSURE(Minutes(1) > Seconds(59));
    TEST_ENSURE(Minutes(1) <= Seconds(60));
    TEST_ENSURE(Minutes(1) >= Seconds(60));

    TEST_ENSURE((Seconds(1) <=> Milliseconds(999)) > 0);
    TEST_ENSURE((Seconds(1) <=> Milliseconds(1001)) < 0);
    TEST_ENSURE((NinetySeconds(2) <=> Seconds(180)) == 0);

    return 0;
}

constexpr int testMixedValueTypeCommon()
{
    TEST_ENSURE((WideMinutes(1) + Minutes(1)).count() == 2);
    TEST_ENSURE(WideMinutes(3) == Minutes(3));
    TEST_ENSURE((Thirds(1) + Halves(1)) == Sixths(5));
    TEST_ENSURE(Thirds(3) == Halves(2));
    TEST_ENSURE(Thirds(1) < Halves(1));
    TEST_ENSURE((Halves(1) - Thirds(1)).count() == 1);

    TEST_ENSURE(floor<Halves>(Thirds(4)).count() == 2);
    TEST_ENSURE(floor<Halves>(Thirds(-4)).count() == -3);
    TEST_ENSURE(ceil<Halves>(Thirds(4)).count() == 3);
    TEST_ENSURE(ceil<Halves>(Thirds(-4)).count() == -2);
    TEST_ENSURE(round<Halves>(Thirds(4)).count() == 3);
    TEST_ENSURE(round<Halves>(Thirds(-4)).count() == -3);
    TEST_ENSURE(round<Halves>(Thirds(5)).count() == 3);
    TEST_ENSURE(round<Halves>(Thirds(3)).count() == 2);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testCtor, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testLimits, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testLimits, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testUnary, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testUnary, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testIncrement, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testIncrement, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testAdd, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testAdd, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testSubtract, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testSubtract, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testMultiply, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testMultiply, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testDivide, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testDivide, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testModulo, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testModulo, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testAbs, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testAbs, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testEqual, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testEqual, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testLess, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testLess, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testLess, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testLess, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testLess, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testLess, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testLess, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testFloatCast, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testDurationCast, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testFloor, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testFloor, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testCeil, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testCeil, Days, std::chrono::days);

    TEST_CONSTEXPR_TEMPLATE(testRound, Nanoseconds, std::chrono::nanoseconds);
    TEST_CONSTEXPR_TEMPLATE(testRound, Microseconds, std::chrono::microseconds);
    TEST_CONSTEXPR_TEMPLATE(testRound, Milliseconds, std::chrono::milliseconds);
    TEST_CONSTEXPR_TEMPLATE(testRound, Seconds, std::chrono::seconds);
    TEST_CONSTEXPR_TEMPLATE(testRound, Minutes, std::chrono::minutes);
    TEST_CONSTEXPR_TEMPLATE(testRound, Hours, std::chrono::hours);
    TEST_CONSTEXPR_TEMPLATE(testRound, Days, std::chrono::days);

    TEST_CONSTEXPR(testConvertInteger);
    TEST_CONSTEXPR(testConvertFloat);
    TEST_CONSTEXPR(testValueWidening);

    TEST_CONSTEXPR(testDurationCastUnit);
    TEST_CONSTEXPR(testRoundingUnit);
    TEST_CONSTEXPR(testNonIntegralPeriod);

    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testFloatOperator);
    TEST_CONSTEXPR(testMixedCompound);

    TEST_CONSTEXPR(testComparison);
    TEST_CONSTEXPR(testMixedValueTypeCommon);

    return 0;
}
