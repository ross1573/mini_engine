export module mini.core:move_iterator;

import :type;
import :iterator;

export namespace mini {

template <ForwardIteratorT T>
class MoveIterator {
private:
    template <ForwardIteratorT U>
    friend class MoveIterator;

public:
    typedef typename T::Value Value;
    typedef typename T::Ptr Ptr;
    typedef RemoveRefT<typename T::Ref>&& Ref;

protected:
    T m_Iter;

public:
    constexpr MoveIterator() noexcept = default;
    explicit constexpr MoveIterator(T);

    constexpr T const& Base() const& noexcept;
    constexpr T Base() && noexcept;
    constexpr Ptr Address() const noexcept;

    constexpr bool IsValid() const;
    constexpr bool IsValidWith(MoveIterator const&) const;
    constexpr bool Increment();
    constexpr bool Decrement() requires BidrectionalIteratorT<T>;
    constexpr bool Advance(OffsetT) requires RandomAccessIteratorT<T>;

    constexpr Ptr operator->() const;
    constexpr Ref operator*() const;
    constexpr Ref operator[](OffsetT) const requires RandomAccessIteratorT<T>;

    constexpr MoveIterator& operator++();
    constexpr MoveIterator operator++(int);
    constexpr MoveIterator& operator--() requires BidrectionalIteratorT<T>;
    constexpr MoveIterator operator--(int) requires BidrectionalIteratorT<T>;
    constexpr MoveIterator& operator+=(OffsetT) requires RandomAccessIteratorT<T>;
    constexpr MoveIterator& operator-=(OffsetT) requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator+(OffsetT) const requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator-(OffsetT) const requires RandomAccessIteratorT<T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator(MoveIterator<U> const&) requires ConvertibleToT<U const&, T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator& operator=(MoveIterator<U> const&) requires ConvertibleToT<U const&, T>;
};

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::MoveIterator(T iter)
    : m_Iter(iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
inline constexpr MoveIterator<T>::MoveIterator(MoveIterator<U> const& o) requires ConvertibleToT<U const&, T>
    : m_Iter(o.m_Iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator=(MoveIterator<U> const& o)
    requires ConvertibleToT<U const&, T>
{
    m_Iter = o.m_Iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr T const& MoveIterator<T>::Base() const& noexcept
{
    return m_Iter;
}

template <ForwardIteratorT T>
inline constexpr T MoveIterator<T>::Base() && noexcept
{
    return MoveArg(m_Iter);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Ptr MoveIterator<T>::Address() const noexcept
{
    return m_Iter.Address();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::IsValid() const
{
    return m_Iter.IsValid();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::IsValidWith(MoveIterator const& o) const
{
    return m_Iter.IsValidWith(o.Base());
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Increment()
{
    return m_Iter.Increment();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Decrement() requires BidrectionalIteratorT<T>
{
    return m_Iter.Decrement();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Advance(OffsetT s) requires RandomAccessIteratorT<T>
{
    return m_Iter.Advance(s);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Ptr MoveIterator<T>::operator->() const
{
    return m_Iter.operator->();
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Ref MoveIterator<T>::operator*() const
{
    return static_cast<Ref>(*m_Iter);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Ref MoveIterator<T>::operator[](OffsetT n) const requires RandomAccessIteratorT<T>
{
    return static_cast<Ref>(m_Iter[n]);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator++()
{
    ++m_Iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator++(int)
{
    MoveIterator<T> t(*this);
    ++(*this);
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator--() requires BidrectionalIteratorT<T>
{
    --m_Iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator--(int) requires BidrectionalIteratorT<T>
{
    MoveIterator<T> t(*this);
    --(*this);
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator+=(OffsetT n) requires RandomAccessIteratorT<T>
{
    m_Iter += n;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator-=(OffsetT n) requires RandomAccessIteratorT<T>
{
    m_Iter -= n;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator+(OffsetT n) const requires RandomAccessIteratorT<T>
{
    MoveIterator t(*this);
    t += n;
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator-(OffsetT n) const requires RandomAccessIteratorT<T>
{
    MoveIterator t(*this);
    t -= n;
    return t;
}

template <RandomAccessIteratorT T>
inline constexpr MoveIterator<T> operator+(OffsetT n, MoveIterator<T> i)
{
    return i + n;
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr OffsetT operator-(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() - r.Base();
}

template <ForwardIteratorT T, ForwardIteratorT U>
inline constexpr bool operator==(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() == r.Base();
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator<(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() < r.Base();
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator>(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() > r.Base();
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator<=(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() <= r.Base();
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator>=(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() >= r.Base();
}

template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr auto operator<=>(MoveIterator<T> const& l, MoveIterator<U> const& r)
    requires ThreeWayComparableWithT<T, U>
{
    return l.Base() <=> r.Base();
}

} // namespace mini