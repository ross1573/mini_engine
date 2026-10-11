export module mini.core:duration;

import :type;
import :ratio;

namespace mini {

export template <ArithmeticT T, RatioT PeriodT>
class Duration;

using NanosecondPeriod = Ratio<1, 1000000000>;
using MicrosecondPeriod = Ratio<1, 1000000>;
using MillisecondPeriod = Ratio<1, 1000>;
using SecondPeriod = Ratio<1, 1>;
using MinutePeriod = Ratio<60, 1>;
using HourPeriod = Ratio<3600, 1>;
using DayPeriod = Ratio<86400, 1>;

export using Nanoseconds = Duration<int64, NanosecondPeriod>;
export using Microseconds = Duration<int64, MicrosecondPeriod>;
export using Milliseconds = Duration<int64, MillisecondPeriod>;
export using Seconds = Duration<int64, SecondPeriod>;
export using Minutes = Duration<int32, MinutePeriod>;
export using Hours = Duration<int32, HourPeriod>;
export using Days = Duration<int32, DayPeriod>;

export using FloatNanoseconds = Duration<float64, NanosecondPeriod>;
export using FloatMicroseconds = Duration<float64, MicrosecondPeriod>;
export using FloatMilliseconds = Duration<float64, MillisecondPeriod>;
export using FloatSeconds = Duration<float64, SecondPeriod>;
export using FloatMinutes = Duration<float64, MinutePeriod>;
export using FloatHours = Duration<float64, HourPeriod>;
export using FloatDays = Duration<float64, DayPeriod>;

template <typename T>
constexpr bool durationType = false;

template <ArithmeticT T, RatioT PeriodT>
constexpr bool durationType<Duration<T, PeriodT>> = true;

export template <typename T>
concept DurationT = durationType<T>;

template <ArithmeticT T, RatioT PeriodT>
class Duration {
public:
    typedef T Value;
    typedef PeriodT Period;

private:
    T m_ticks;

public:
    constexpr Duration() noexcept = default;

    template <ConvertibleToT<T> U>
    explicit constexpr Duration(U value) noexcept
        requires(FloatingT<T> || !FloatingT<U>);

    template <ArithmeticT U, RatioT PeriodU>
    constexpr Duration(Duration<U, PeriodU> const& other) noexcept
        requires(FloatingT<T> || (!FloatingT<U> && RatioDivideT<PeriodU, PeriodT>::denom == 1));

    constexpr Value count() const noexcept;

    constexpr CommonT<Duration> operator+() const noexcept;
    constexpr CommonT<Duration> operator-() const noexcept;
    constexpr Duration operator++(int) noexcept;
    constexpr Duration operator--(int) noexcept;
    constexpr Duration& operator++() noexcept;
    constexpr Duration& operator--() noexcept;
    constexpr Duration& operator+=(Duration const& other) noexcept;
    constexpr Duration& operator-=(Duration const& other) noexcept;
    constexpr Duration& operator%=(Duration const& other) noexcept;
    constexpr Duration& operator*=(Value const& value) noexcept;
    constexpr Duration& operator/=(Value const& value) noexcept;
    constexpr Duration& operator%=(Value const& value) noexcept;

    static constexpr Duration zero() noexcept;
    static constexpr Duration min() noexcept;
    static constexpr Duration max() noexcept;
};

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U durationCast(Duration<T, PeriodT> const& from) noexcept
{
    using FromDuration = Duration<T, PeriodT>;
    using ToDuration = U;
    using ResultT = CommonT<typename FromDuration::Value, typename ToDuration::Value, int64>;

    constexpr auto period = RatioDivideT<PeriodT, typename U::Period>{};
    if constexpr (period.num == 1 && period.denom == 1) {
        return ToDuration(static_cast<ToDuration::Value>(from.count()));
    } else if constexpr (period.num == 1) {
        ResultT ticks = static_cast<ResultT>(from.count()) / static_cast<ResultT>(period.denom);
        return ToDuration(static_cast<ToDuration::Value>(ticks));
    } else if constexpr (period.denom == 1) {
        ResultT ticks = static_cast<ResultT>(from.count()) * static_cast<ResultT>(period.num);
        return ToDuration(static_cast<ToDuration::Value>(ticks));
    } else {
        ResultT ticks =
            static_cast<ResultT>(from.count()) * static_cast<ResultT>(period.num) / static_cast<ResultT>(period.denom);
        return ToDuration(static_cast<ToDuration::Value>(ticks));
    }
}

template <ArithmeticT T, RatioT PeriodT>
template <ConvertibleToT<T> U>
constexpr Duration<T, PeriodT>::Duration(U value) noexcept
    requires(FloatingT<T> || !FloatingT<U>)
    : m_ticks(value)
{
}

template <ArithmeticT T, RatioT PeriodT>
template <ArithmeticT U, RatioT PeriodU>
constexpr Duration<T, PeriodT>::Duration(Duration<U, PeriodU> const& other) noexcept
    requires(FloatingT<T> || (!FloatingT<U> && RatioDivideT<PeriodU, PeriodT>::denom == 1))
    : m_ticks(durationCast<Duration<T, PeriodT>>(other).count())
{
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>::Value Duration<T, PeriodT>::count() const noexcept
{
    return m_ticks;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr CommonT<Duration<T, PeriodT>> Duration<T, PeriodT>::operator+() const noexcept
{
    return Duration(*this);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr CommonT<Duration<T, PeriodT>> Duration<T, PeriodT>::operator-() const noexcept
{
    return Duration(-m_ticks);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::operator++(int) noexcept
{
    return Duration(m_ticks++);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::operator--(int) noexcept
{
    return Duration(m_ticks--);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator++() noexcept
{
    ++m_ticks;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator--() noexcept
{
    --m_ticks;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator+=(Duration const& other) noexcept
{
    m_ticks += other.m_ticks;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator-=(Duration const& other) noexcept
{
    m_ticks -= other.m_ticks;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator%=(Duration const& other) noexcept
{
    m_ticks %= other.m_ticks;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator*=(Value const& value) noexcept
{
    m_ticks *= value;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator/=(Value const& value) noexcept
{
    m_ticks /= value;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>& Duration<T, PeriodT>::operator%=(Value const& value) noexcept
{
    m_ticks %= value;
    return *this;
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::zero() noexcept
{
    return Duration(0);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::min() noexcept
{
    return Duration(NumericLimit<T>::min);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::max() noexcept
{
    return Duration(NumericLimit<T>::max);
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>> operator+(Duration<T, PeriodT> const& lhs,
                                                                        Duration<U, PeriodU> const& rhs) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(durationCast<ResultT>(lhs).count() + durationCast<ResultT>(rhs).count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>> operator-(Duration<T, PeriodT> const& lhs,
                                                                        Duration<U, PeriodU> const& rhs) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(durationCast<ResultT>(lhs).count() - durationCast<ResultT>(rhs).count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator*(Duration<T, PeriodT> const& duration, U value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(durationCast<ResultT>(duration).count() * static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator*(U value, Duration<T, PeriodT> const& duration) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(durationCast<ResultT>(duration).count() * static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator/(Duration<T, PeriodT> const& duration, U value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(durationCast<ResultT>(duration).count() / static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<T, U> operator/(Duration<T, PeriodT> const& lhs, Duration<U, PeriodU> const& rhs) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return durationCast<CommonDurationT>(lhs).count() / durationCast<CommonDurationT>(rhs).count();
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>> operator%(Duration<T, PeriodT> const& lhs,
                                                                        Duration<U, PeriodU> const& rhs) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(durationCast<ResultT>(lhs).count() % durationCast<ResultT>(rhs).count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator%(Duration<T, PeriodT> const& duration, U const& value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(durationCast<ResultT>(duration).count() % static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr bool operator==(Duration<T, PeriodT> const& lhs, Duration<U, PeriodU> const& rhs) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return durationCast<CommonDurationT>(lhs).count() == durationCast<CommonDurationT>(rhs).count();
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr auto operator<=>(Duration<T, PeriodT> const& lhs, Duration<U, PeriodU> const& rhs) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return durationCast<CommonDurationT>(lhs).count() <=> durationCast<CommonDurationT>(rhs).count();
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U floor(Duration<T, PeriodT> const& duration) noexcept
{
    U result = durationCast<U>(duration);
    if (result > duration) {
        result = result - U(1);
    }

    return result;
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U ceil(Duration<T, PeriodT> const& duration) noexcept
{
    U result = durationCast<U>(duration);
    if (result < duration) {
        result = result + U(1);
    }

    return result;
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U round(Duration<T, PeriodT> const& duration) noexcept
{
    U lower = durationCast<U>(duration);
    if (lower > duration) {
        lower = lower - U(1);
    }

    U upper = lower + U(1);
    auto lowerDiff = duration - lower;
    auto upperDiff = upper - duration;

    if (lowerDiff < upperDiff) {
        return lower;
    }

    if (lowerDiff > upperDiff) {
        return upper;
    }

    return (lower.count() & 1) ? upper : lower;
}

export template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> abs(Duration<T, PeriodT> const& duration) noexcept
{
    return duration >= duration.zero() ? +duration : -duration;
}

} // namespace mini