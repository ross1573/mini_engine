module;

#include "core/assert.h"

export module mini.core:circular_iterator;

import :type;
import :iterator;
import :iterator_version;

export namespace mini
{

template <
    typename T,
    typename CircularT
>
class CircularIterator
{
    template <typename U, typename C>
    friend class CircularIterator;
    friend CircularT;

    using VersionT = IteratorVersion<CircularT>;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

protected:
    SizeT m_Offset;
    SizeT m_Capacity;
    Ptr m_Begin;
    CircularT* m_Circular;
    [[no_unique_address]] VersionT m_Version;

public:
    constexpr CircularIterator() noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator(CircularIterator<U, CircularU> const&) noexcept
        requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

    constexpr Ptr Address() const noexcept;

    constexpr bool IsValid() const noexcept;
    constexpr bool IsValidWith(CircularIterator const&) const noexcept;
    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    constexpr Ref operator[](OffsetT) const noexcept;

    constexpr CircularIterator& operator++() noexcept;
    constexpr CircularIterator& operator--() noexcept;
    constexpr CircularIterator& operator+=(OffsetT) noexcept;
    constexpr CircularIterator& operator-=(OffsetT) noexcept;
    constexpr CircularIterator operator++(int) noexcept;
    constexpr CircularIterator operator--(int) noexcept;
    constexpr CircularIterator operator+(OffsetT) const noexcept;
    constexpr CircularIterator operator-(OffsetT) const noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator& operator=(CircularIterator<U, CircularU> const&) noexcept
        requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

protected:
    constexpr CircularIterator(SizeT, SizeT, Ptr, SizeT, CircularT*) noexcept;

    template <typename U, typename CircularU>
    constexpr bool CheckSource(CircularIterator<U, CircularU> const&) const noexcept;
    constexpr bool CheckIterator(CircularIterator const&) const noexcept;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr OffsetT operator-(CircularIterator<U, CircularU> const& l,
                                       CircularIterator<Y, CircularY> const& r) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr bool operator==(CircularIterator<U, CircularU> const&,
                                     CircularIterator<Y, CircularY> const&) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>>&& EqualityComparableWithT<U*, Y*>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr auto operator<=>(CircularIterator<U, CircularU> const&,
                                      CircularIterator<Y, CircularY> const&) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>>&& ThreeWayComparableWithT<U*, Y*>;
};

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>::CircularIterator() noexcept
    : m_Offset(0)
    , m_Capacity(0)
    , m_Begin(nullptr)
    , m_Circular(nullptr)
    , m_Version(0)
{
}

template <typename T, typename CircularT>
inline constexpr
CircularIterator<T, CircularT>::CircularIterator(SizeT idx, SizeT cap,
                                                 Ptr begin, SizeT ver,
                                                 CircularT* base) noexcept
    : m_Offset(idx)
    , m_Capacity(cap)
    , m_Begin(begin)
    , m_Circular(base)
    , m_Version(ver)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
inline constexpr
CircularIterator<T, CircularT>::CircularIterator(CircularIterator<U, CircularU> const& o)
noexcept requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<CircularT>, DecayT<CircularU>>
    : m_Offset(o.m_Offset)
    , m_Capacity(o.m_Capacity)
    , m_Begin(o.m_Begin)
    , m_Circular(o.m_Circular)
    , m_Version(o.m_Version)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator=(CircularIterator<U, CircularU> const& o) noexcept
    requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    m_Offset = o.m_Offset;
    m_Capacity = o.m_Capacity;
    m_Begin = o.m_Begin;
    m_Circular = o.m_Circular;
    m_Version = o.m_Version;
    return *this;
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
inline constexpr bool
CircularIterator<T, CircularT>::CheckSource(CircularIterator<U, CircularU> const& iter) const noexcept
{
    return m_Circular && m_Circular == iter.m_Circular && m_Version == iter.m_Version;
}

template <typename T, typename CircularT>
inline constexpr bool
CircularIterator<T, CircularT>::CheckIterator(CircularIterator const& iter) const noexcept
{
    return iter.m_Circular && iter.m_Circular->IsValidIterator(iter);
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>::Ptr
CircularIterator<T, CircularT>::Address() const noexcept
{
    return m_Begin + (m_Offset % m_Capacity);
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::IsValid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename CircularT>
inline constexpr bool
CircularIterator<T, CircularT>::IsValidWith(CircularIterator const& o) const noexcept
{
    return CheckSource(o);
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Reset() noexcept
{
    if (m_Circular == nullptr) [[unlikely]]
    {
        return false;
    }

    CircularIterator begin = m_Circular->Begin();
    m_Begin = begin.m_Begin;
    m_Offset = begin.m_Offset;
    m_Version = begin.m_Version;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Finish() noexcept
{
    if (m_Circular == nullptr) [[unlikely]]
    {
        return false;
    }

    CircularIterator end = m_Circular->End();
    m_Begin = end.m_Begin;
    m_Offset = end.m_Offset;
    m_Version = end.m_Version;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]]
    {
        return false;;
    }

    ++m_Offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]]
    {
        return false;
    }

    --m_Offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Advance(OffsetT d) noexcept
{
    if (d >= m_Capacity || !CheckIterator(*this + d)) [[unlikely]]
    {
        return false;
    }

    m_Offset = (SizeT)((OffsetT)m_Offset + d);
    return true;
}

template <typename T, typename CircularT>
inline constexpr T* CircularIterator<T, CircularT>::operator->() const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this), "invalid access");
    return m_Begin + (m_Offset % m_Capacity);
}

template <typename T, typename CircularT>
inline constexpr T& CircularIterator<T, CircularT>::operator*() const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this), "invalid access");
    return *(m_Begin + (m_Offset % m_Capacity));
}

template <typename T, typename CircularT>
inline constexpr T&
CircularIterator<T, CircularT>::operator[](OffsetT const o) const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this + o), "invalid access");
    return *(m_Begin + ((m_Offset + o) % m_Capacity));
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator++() noexcept
{
    ++m_Offset;
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator--() noexcept
{
    --m_Offset;
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator+=(OffsetT d) noexcept
{
    m_Offset = (SizeT)((OffsetT)m_Offset + d);
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator-=(OffsetT d) noexcept
{
    m_Offset = (SizeT)((OffsetT)m_Offset - d);
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator++(int) noexcept
{
    CircularIterator t(*this);
    ++(*this);
    return t;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator--(int) noexcept
{
    CircularIterator t(*this);
    --(*this);
    return t;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator+(OffsetT d) const noexcept
{
    CircularIterator t(*this);
    t += d;
    return t;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator-(OffsetT d) const noexcept
{
    CircularIterator t(*this);
    t -= d;
    return t;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
operator+(OffsetT n, CircularIterator<T, CircularT> const& iter) noexcept
{
    return iter + n;
}

template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr OffsetT operator-(CircularIterator<T, CircularT> const& l,
                                             CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    ASSERT(l.m_Begin == r.m_Begin);
    return (OffsetT)l.m_Offset - (OffsetT)r.m_Offset;
}

template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr bool operator==(CircularIterator<T, CircularT> const& l,
                                           CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>>&& EqualityComparableWithT<T*, U*>
{
    return (l.m_Offset == r.m_Offset) && (l.m_Begin == r.m_Begin);
}

template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr auto operator<=>(CircularIterator<T, CircularT> const& l,
                                            CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>>&& ThreeWayComparableWithT<T*, U*>
{
    ASSERT(l.m_Begin == r.m_Begin);
    return l.m_Offset <=> r.m_Offset;
}

} // namespace mini