export module mini.core:time_point;

import :type;
import :duration;

namespace mini {

export template <DurationT T = NanoSeconds>
class TimePoint;

template <typename T>
constexpr bool IsTimePointT = false;

template <DurationT T>
constexpr bool IsTimePointT<TimePoint<T>> = true;

export template <typename T>
concept TimePointT = IsTimePointT<T>;

template <DurationT T>
class TimePoint {
public:
    typedef T Duration;
    typedef typename Duration::Value Value;
    typedef typename Duration::Period Period;

private:
    Duration m_Duration;

public:
    constexpr TimePoint() noexcept;
    explicit constexpr TimePoint(Duration const&) noexcept;

    template <DurationT U>
    constexpr TimePoint(TimePoint<U> const&) noexcept;

    constexpr Duration SinceEpoch() const noexcept;

    constexpr TimePoint& operator+=(Duration const&) noexcept;
    constexpr TimePoint& operator-=(Duration const&) noexcept;

    static constexpr TimePoint Min() noexcept;
    static constexpr TimePoint Max() noexcept;
};

template <DurationT T>
constexpr TimePoint<T>::TimePoint() noexcept
    : m_Duration(0)
{
}

template <DurationT T>
constexpr TimePoint<T>::TimePoint(Duration const& duration) noexcept
    : m_Duration(duration)
{
}

template <DurationT T>
template <DurationT U>
constexpr TimePoint<T>::TimePoint(TimePoint<U> const& other) noexcept
    : m_Duration(DurationCast<T>(other.m_Duration))
{
}

template <DurationT T>
constexpr TimePoint<T>::Duration TimePoint<T>::SinceEpoch() const noexcept
{
    return m_Duration;
}

template <DurationT T>
constexpr TimePoint<T>& TimePoint<T>::operator+=(Duration const& duration) noexcept
{
    m_Duration += duration;
    return *this;
}

template <DurationT T>
constexpr TimePoint<T>& TimePoint<T>::operator-=(Duration const& duration) noexcept
{
    m_Duration -= duration;
    return *this;
}

template <DurationT T>
constexpr TimePoint<T> TimePoint<T>::Min() noexcept
{
    return TimePoint<T>(Duration::Min());
}

template <DurationT T>
constexpr TimePoint<T> TimePoint<T>::Max() noexcept
{
    return TimePoint<T>(Duration::Max());
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> TimePointCast(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(DurationCast<To>(timepoint.SinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> Floor(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(Floor<To>(timepoint.SinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> Ceil(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(Ceil<To>(timepoint.SinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> Round(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(Round<To>(timepoint.SinceEpoch()));
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator+(TimePoint<T> const& timepoint,
                                             U const& duration) noexcept
{
    return TimePoint<CommonT<T, U>>(timepoint.SinceEpoch() + duration);
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator+(T const& duration,
                                             TimePoint<U> const& timepoint) noexcept
{
    return TimePoint<CommonT<T, U>>(duration + timepoint.SinceEpoch());
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator-(TimePoint<T> const& timepoint,
                                             U const& duration) noexcept
{
    return TimePoint<CommonT<T, U>>(timepoint.SinceEpoch() - duration);
}

export template <DurationT T, DurationT U>
constexpr CommonT<T, U> operator-(TimePoint<T> const& l, TimePoint<U> const& r) noexcept
{
    return l.SinceEpoch() - r.SinceEpoch();
}

export template <DurationT T, DurationT U>
constexpr bool operator==(TimePoint<T> const& l, TimePoint<U> const& r) noexcept
{
    return l.SinceEpoch() == r.SinceEpoch();
}

export template <DurationT T, DurationT U>
constexpr auto operator<=>(TimePoint<T> const& l, TimePoint<U> const& r) noexcept
{
    return l.SinceEpoch() <=> r.SinceEpoch();
}

} // namespace mini