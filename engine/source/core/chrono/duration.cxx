export module mini.core:duration;

import :type;
import :ratio;

namespace mini {

export template <ArithmeticT T, RatioT PeriodT>
class Duration;

export using NanoSeconds = Duration<int64, Ratio<1, 1000000000>>;
export using MicroSeconds = Duration<int64, Ratio<1, 1000000>>;
export using MilliSeconds = Duration<int64, Ratio<1, 1000>>;
export using Seconds = Duration<int64, Ratio<1, 1>>;
export using Minutes = Duration<int32, Ratio<60, 1>>;
export using Hours = Duration<int32, Ratio<3600, 1>>;
export using Days = Duration<int32, Ratio<86400, 1>>;

template <typename T>
constexpr bool IsDurationT = false;

template <ArithmeticT T, RatioT PeriodT>
constexpr bool IsDurationT<Duration<T, PeriodT>> = true;

export template <typename T>
concept DurationT = IsDurationT<T>;

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
    explicit constexpr Duration(U) noexcept
        requires(FloatingT<T> || !FloatingT<U>);

    template <ArithmeticT U, RatioT PeriodU>
    constexpr Duration(Duration<U, PeriodU> const&) noexcept
        requires(FloatingT<T> || (!FloatingT<U> && RatioDivideT<PeriodU, PeriodT>::denom == 1));

    constexpr Value Count() const noexcept;

    constexpr CommonT<Duration> operator+() const noexcept;
    constexpr CommonT<Duration> operator-() const noexcept;
    constexpr Duration operator++(int) noexcept;
    constexpr Duration operator--(int) noexcept;
    constexpr Duration& operator++() noexcept;
    constexpr Duration& operator--() noexcept;
    constexpr Duration& operator+=(Duration const&) noexcept;
    constexpr Duration& operator-=(Duration const&) noexcept;
    constexpr Duration& operator%=(Duration const&) noexcept;
    constexpr Duration& operator*=(Value const&) noexcept;
    constexpr Duration& operator/=(Value const&) noexcept;
    constexpr Duration& operator%=(Value const&) noexcept;

    static constexpr Duration Zero() noexcept;
    static constexpr Duration Min() noexcept;
    static constexpr Duration Max() noexcept;
};

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U DurationCast(Duration<T, PeriodT> const& from) noexcept
{
    using FromDuration = Duration<T, PeriodT>;
    using ToDuration = U;
    using ResultT = CommonT<typename FromDuration::Value, typename ToDuration::Value, int64>;

    constexpr auto period = RatioDivideT<PeriodT, typename U::Period>{};
    if constexpr (period.num == 1 && period.denom == 1) {
        return ToDuration(static_cast<typename ToDuration::Value>(from.Count()));
    } else if constexpr (period.num == 1) {
        ResultT ticks = static_cast<ResultT>(from.Count()) / static_cast<ResultT>(period.denom);
        return ToDuration(static_cast<typename ToDuration::Value>(ticks));
    } else if constexpr (period.denom == 1) {
        ResultT ticks = static_cast<ResultT>(from.Count()) * static_cast<ResultT>(period.num);
        return ToDuration(static_cast<typename ToDuration::Value>(ticks));
    } else {
        ResultT ticks = static_cast<ResultT>(from.Count()) * static_cast<ResultT>(period.num) /
                        static_cast<ResultT>(period.denom);
        return ToDuration(static_cast<typename ToDuration::Value>(ticks));
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
    : m_ticks(DurationCast<Duration<T, PeriodT>>(other).Count())
{
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT>::Value Duration<T, PeriodT>::Count() const noexcept
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
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::Zero() noexcept
{
    return Duration(0);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::Min() noexcept
{
    return Duration(NumericLimit<T>::min);
}

template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Duration<T, PeriodT>::Max() noexcept
{
    return Duration(NumericLimit<T>::max);
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>
operator+(Duration<T, PeriodT> const& l, Duration<U, PeriodU> const& r) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(DurationCast<ResultT>(l).Count() + DurationCast<ResultT>(r).Count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>
operator-(Duration<T, PeriodT> const& l, Duration<U, PeriodU> const& r) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(DurationCast<ResultT>(l).Count() - DurationCast<ResultT>(r).Count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator*(Duration<T, PeriodT> const& duration,
                                                     U value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(DurationCast<ResultT>(duration).Count() * static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator*(U value,
                                                     Duration<T, PeriodT> const& duration) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(DurationCast<ResultT>(duration).Count() * static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator/(Duration<T, PeriodT> const& duration,
                                                     U value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(DurationCast<ResultT>(duration).Count() / static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<T, U> operator/(Duration<T, PeriodT> const& l,
                                  Duration<U, PeriodU> const& r) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return DurationCast<CommonDurationT>(l).Count() / DurationCast<CommonDurationT>(r).Count();
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>
operator%(Duration<T, PeriodT> const& l, Duration<U, PeriodU> const& r) noexcept
{
    using ResultT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return ResultT(DurationCast<ResultT>(l).Count() % DurationCast<ResultT>(r).Count());
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U>
constexpr Duration<CommonT<T, U>, PeriodT> operator%(Duration<T, PeriodT> const& duration,
                                                     U const& value) noexcept
{
    using ValueT = CommonT<T, U>;
    using ResultT = Duration<ValueT, PeriodT>;
    return ResultT(DurationCast<ResultT>(duration).Count() % static_cast<ValueT>(value));
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr bool operator==(Duration<T, PeriodT> const& l, Duration<U, PeriodU> const& r) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return DurationCast<CommonDurationT>(l).Count() == DurationCast<CommonDurationT>(r).Count();
}

export template <ArithmeticT T, RatioT PeriodT, ArithmeticT U, RatioT PeriodU>
constexpr auto operator<=>(Duration<T, PeriodT> const& l, Duration<U, PeriodU> const& r) noexcept
{
    using CommonDurationT = CommonT<Duration<T, PeriodT>, Duration<U, PeriodU>>;
    return DurationCast<CommonDurationT>(l).Count() <=> DurationCast<CommonDurationT>(r).Count();
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U Floor(Duration<T, PeriodT> const& duration) noexcept
{
    U result = DurationCast<U>(duration);
    if (result > duration) {
        result = result - U(1);
    }

    return result;
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U Ceil(Duration<T, PeriodT> const& duration) noexcept
{
    U result = DurationCast<U>(duration);
    if (result < duration) {
        result = result + U(1);
    }

    return result;
}

export template <DurationT U, ArithmeticT T, RatioT PeriodT>
constexpr U Round(Duration<T, PeriodT> const& duration) noexcept
{
    U lower = DurationCast<U>(duration);
    if (lower > duration) {
        lower = lower - U(1);
    }

    U upper = lower + U(1);
    auto lowerDiff = duration - lower;
    auto upperDiff = upper - duration;

    if (lowerDiff < upperDiff) {
        return lower;
    } else if (lowerDiff > upperDiff) {
        return upper;
    }

    return (lower.Count() & 1) ? upper : lower;
}

export template <ArithmeticT T, RatioT PeriodT>
constexpr Duration<T, PeriodT> Abs(Duration<T, PeriodT> const& duration) noexcept
{
    return duration >= duration.Zero() ? +duration : -duration;
}

} // namespace mini