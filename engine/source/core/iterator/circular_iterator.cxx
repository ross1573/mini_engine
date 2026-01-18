export module mini.core:circular_iterator;

import :type;
import :iterator;

namespace mini {

export template <typename T, typename CircularT>
class CircularIterator {
private:
    template <typename U, typename C>
    friend class CircularIterator;
    friend CircularT;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

protected:
    SizeT m_offset;
    SizeT m_capacity;
    Pointer m_begin;
    CircularT* m_circular;

public:
    constexpr CircularIterator() noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator(CircularIterator<U, CircularU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

    constexpr Pointer Address() const noexcept;
    constexpr bool Valid() const noexcept;
    constexpr bool ValidWith(CircularIterator const&) const noexcept;

    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    constexpr Reference operator[](OffsetT) const noexcept;

    constexpr CircularIterator& operator++() noexcept;
    constexpr CircularIterator& operator--() noexcept;
    constexpr CircularIterator& operator+=(OffsetT) noexcept;
    constexpr CircularIterator& operator-=(OffsetT) noexcept;
    constexpr CircularIterator operator++(int32) noexcept;
    constexpr CircularIterator operator--(int32) noexcept;
    constexpr CircularIterator operator+(OffsetT) const noexcept;
    constexpr CircularIterator operator-(OffsetT) const noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator& operator=(CircularIterator<U, CircularU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

protected:
    constexpr CircularIterator(SizeT, SizeT, Pointer, CircularT*) noexcept;
    constexpr bool CheckIterator(CircularIterator const&) const noexcept;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr OffsetT operator-(CircularIterator<U, CircularU> const& l,
                                       CircularIterator<Y, CircularY> const& r) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr bool operator==(CircularIterator<U, CircularU> const&,
                                     CircularIterator<Y, CircularY> const&) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>> && EqualityComparableWithT<U*, Y*>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr auto operator<=>(CircularIterator<U, CircularU> const&,
                                      CircularIterator<Y, CircularY> const&) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>> && ThreeWayComparableWithT<U*, Y*>;
};

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>::CircularIterator() noexcept
    : m_offset(0)
    , m_capacity(0)
    , m_begin(nullptr)
    , m_circular(nullptr)
{
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>::CircularIterator(SizeT idx, SizeT cap,
                                                                  Pointer begin,
                                                                  CircularT* base) noexcept
    : m_offset(idx)
    , m_capacity(cap)
    , m_begin(begin)
    , m_circular(base)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
inline constexpr CircularIterator<T, CircularT>::
    CircularIterator(CircularIterator<U, CircularU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>
    : m_offset(o.m_offset)
    , m_capacity(o.m_capacity)
    , m_begin(o.m_begin)
    , m_circular(o.m_circular)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator=(CircularIterator<U, CircularU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    m_offset = o.m_offset;
    m_capacity = o.m_capacity;
    m_begin = o.m_begin;
    m_circular = o.m_circular;
    return *this;
}

template <typename T, typename CircularT>
inline constexpr bool
CircularIterator<T, CircularT>::CheckIterator(CircularIterator const& iter) const noexcept
{
    return iter.m_circular && iter.m_circular->ValidIterator(iter);
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>::Pointer
CircularIterator<T, CircularT>::Address() const noexcept
{
    return m_begin + (m_offset % m_capacity);
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Valid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename CircularT>
inline constexpr bool
CircularIterator<T, CircularT>::ValidWith(CircularIterator const& o) const noexcept
{
    return m_circular && m_circular->ValidRange(*this, o);
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Reset() noexcept
{
    if (m_circular == nullptr) [[unlikely]] {
        return false;
    }

    CircularIterator begin = m_circular->Begin();
    m_begin = begin.m_begin;
    m_offset = begin.m_offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Finish() noexcept
{
    if (m_circular == nullptr) [[unlikely]] {
        return false;
    }

    CircularIterator end = m_circular->End();
    m_begin = end.m_begin;
    m_offset = end.m_offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]] {
        return false;
        ;
    }

    ++m_offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_offset;
    return true;
}

template <typename T, typename CircularT>
inline constexpr bool CircularIterator<T, CircularT>::Advance(OffsetT d) noexcept
{
    if (d >= (OffsetT)m_capacity || !CheckIterator(*this + d)) [[unlikely]] {
        return false;
    }

    m_offset = (SizeT)((OffsetT)m_offset + d);
    return true;
}

template <typename T, typename CircularT>
inline constexpr T* CircularIterator<T, CircularT>::operator->() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return m_begin + (m_offset % m_capacity);
}

template <typename T, typename CircularT>
inline constexpr T& CircularIterator<T, CircularT>::operator*() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return *(m_begin + (m_offset % m_capacity));
}

template <typename T, typename CircularT>
inline constexpr T& CircularIterator<T, CircularT>::operator[](OffsetT const o) const noexcept
{
    ASSERT(CheckIterator(*this + o), "invalid access");
    return *(m_begin + ((m_offset + o) % m_capacity));
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator++() noexcept
{
    ++m_offset;
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator--() noexcept
{
    --m_offset;
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator+=(OffsetT d) noexcept
{
    m_offset = (SizeT)((OffsetT)m_offset + d);
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>&
CircularIterator<T, CircularT>::operator-=(OffsetT d) noexcept
{
    m_offset = (SizeT)((OffsetT)m_offset - d);
    return *this;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator++(int32) noexcept
{
    CircularIterator t(*this);
    ++(*this);
    return t;
}

template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
CircularIterator<T, CircularT>::operator--(int32) noexcept
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

export template <typename T, typename CircularT>
inline constexpr CircularIterator<T, CircularT>
operator+(OffsetT n, CircularIterator<T, CircularT> const& iter) noexcept
{
    return iter + n;
}

export template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr OffsetT operator-(CircularIterator<T, CircularT> const& l,
                                   CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    ASSERT(l.m_begin == r.m_begin);
    return (OffsetT)l.m_offset - (OffsetT)r.m_offset;
}

export template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr bool operator==(CircularIterator<T, CircularT> const& l,
                                 CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>> && EqualityComparableWithT<T*, U*>
{
    return (l.m_offset == r.m_offset) && (l.m_begin == r.m_begin);
}

export template <typename T, typename CircularT, typename U, typename CircularU>
inline constexpr auto operator<=>(CircularIterator<T, CircularT> const& l,
                                  CircularIterator<U, CircularU> const& r) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>> && ThreeWayComparableWithT<T*, U*>
{
    ASSERT(l.m_begin == r.m_begin);
    return l.m_offset <=> r.m_offset;
}

} // namespace mini