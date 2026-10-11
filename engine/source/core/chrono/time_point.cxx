export module mini.core:time_point;

import :type;
import :duration;

namespace mini {

export template <DurationT T = Nanoseconds>
class TimePoint;

template <typename T>
constexpr bool timePointType = false;

template <DurationT T>
constexpr bool timePointType<TimePoint<T>> = true;

export template <typename T>
concept TimePointT = timePointType<T>;

template <DurationT T>
class TimePoint {
public:
    typedef T Duration;
    typedef Duration::Value Value;
    typedef Duration::Period Period;

private:
    Duration m_duration;

public:
    constexpr TimePoint() noexcept;
    explicit constexpr TimePoint(Duration const& duration) noexcept;

    template <DurationT U>
    constexpr TimePoint(TimePoint<U> const& other) noexcept;

    constexpr Duration sinceEpoch() const noexcept;

    constexpr TimePoint& operator+=(Duration const& duration) noexcept;
    constexpr TimePoint& operator-=(Duration const& duration) noexcept;

    static constexpr TimePoint min() noexcept;
    static constexpr TimePoint max() noexcept;
};

template <DurationT T>
constexpr TimePoint<T>::TimePoint() noexcept
    : m_duration(0)
{
}

template <DurationT T>
constexpr TimePoint<T>::TimePoint(Duration const& duration) noexcept
    : m_duration(duration)
{
}

template <DurationT T>
template <DurationT U>
constexpr TimePoint<T>::TimePoint(TimePoint<U> const& other) noexcept
    : m_duration(durationCast<T>(other.m_duration))
{
}

template <DurationT T>
constexpr TimePoint<T>::Duration TimePoint<T>::sinceEpoch() const noexcept
{
    return m_duration;
}

template <DurationT T>
constexpr TimePoint<T>& TimePoint<T>::operator+=(Duration const& duration) noexcept
{
    m_duration += duration;
    return *this;
}

template <DurationT T>
constexpr TimePoint<T>& TimePoint<T>::operator-=(Duration const& duration) noexcept
{
    m_duration -= duration;
    return *this;
}

template <DurationT T>
constexpr TimePoint<T> TimePoint<T>::min() noexcept
{
    return TimePoint<T>(Duration::min());
}

template <DurationT T>
constexpr TimePoint<T> TimePoint<T>::max() noexcept
{
    return TimePoint<T>(Duration::max());
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> timePointCast(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(durationCast<To>(timepoint.sinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> floor(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(floor<To>(timepoint.sinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> ceil(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(ceil<To>(timepoint.sinceEpoch()));
}

export template <DurationT To, DurationT From>
constexpr TimePoint<To> round(TimePoint<From> const& timepoint) noexcept
{
    return TimePoint<To>(round<To>(timepoint.sinceEpoch()));
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator+(TimePoint<T> const& timepoint, U const& duration) noexcept
{
    return TimePoint<CommonT<T, U>>(timepoint.sinceEpoch() + duration);
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator+(T const& duration, TimePoint<U> const& timepoint) noexcept
{
    return TimePoint<CommonT<T, U>>(duration + timepoint.sinceEpoch());
}

export template <DurationT T, DurationT U>
constexpr TimePoint<CommonT<T, U>> operator-(TimePoint<T> const& timepoint, U const& duration) noexcept
{
    return TimePoint<CommonT<T, U>>(timepoint.sinceEpoch() - duration);
}

export template <DurationT T, DurationT U>
constexpr CommonT<T, U> operator-(TimePoint<T> const& lhs, TimePoint<U> const& rhs) noexcept
{
    return lhs.sinceEpoch() - rhs.sinceEpoch();
}

export template <DurationT T, DurationT U>
constexpr bool operator==(TimePoint<T> const& lhs, TimePoint<U> const& rhs) noexcept
{
    return lhs.sinceEpoch() == rhs.sinceEpoch();
}

export template <DurationT T, DurationT U>
constexpr auto operator<=>(TimePoint<T> const& lhs, TimePoint<U> const& rhs) noexcept
{
    return lhs.sinceEpoch() <=> rhs.sinceEpoch();
}

} // namespace mini