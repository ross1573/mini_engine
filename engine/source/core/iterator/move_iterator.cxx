export module mini.core:move_iterator;

import :type;
import :iterator;

namespace mini {

export template <ForwardIteratorT T>
class MoveIterator {
private:
    template <ForwardIteratorT U>
    friend class MoveIterator;

public:
    typedef typename T::Value Value;
    typedef typename T::Pointer Pointer;
    typedef RemoveRefT<typename T::Reference>&& Reference;

protected:
    T m_iter;

public:
    constexpr MoveIterator() noexcept = default;
    explicit constexpr MoveIterator(T);

    constexpr T const& Base() const& noexcept;
    constexpr T Base() && noexcept;
    constexpr Pointer Address() const noexcept;

    constexpr bool Valid() const;
    constexpr bool ValidWith(MoveIterator const&) const;
    constexpr bool Increment();
    constexpr bool Decrement()
        requires BidrectionalIteratorT<T>;
    constexpr bool Advance(OffsetT)
        requires RandomAccessIteratorT<T>;

    constexpr Pointer operator->() const;
    constexpr Reference operator*() const;
    constexpr Reference operator[](OffsetT) const
        requires RandomAccessIteratorT<T>;

    constexpr MoveIterator& operator++();
    constexpr MoveIterator operator++(int32);
    constexpr MoveIterator& operator--()
        requires BidrectionalIteratorT<T>;
    constexpr MoveIterator operator--(int32)
        requires BidrectionalIteratorT<T>;
    constexpr MoveIterator& operator+=(OffsetT)
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator& operator-=(OffsetT)
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator+(OffsetT) const
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator-(OffsetT) const
        requires RandomAccessIteratorT<T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator(MoveIterator<U> const&)
        requires ConvertibleToT<U const&, T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator& operator=(MoveIterator<U> const&)
        requires ConvertibleToT<U const&, T>;
};

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::MoveIterator(T iter)
    : m_iter(iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
inline constexpr MoveIterator<T>::MoveIterator(MoveIterator<U> const& o)
    requires ConvertibleToT<U const&, T>
    : m_iter(o.m_iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator=(MoveIterator<U> const& o)
    requires ConvertibleToT<U const&, T>
{
    m_iter = o.m_iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr T const& MoveIterator<T>::Base() const& noexcept
{
    return m_iter;
}

template <ForwardIteratorT T>
inline constexpr T MoveIterator<T>::Base() && noexcept
{
    return MoveArg(m_iter);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Pointer MoveIterator<T>::Address() const noexcept
{
    return m_iter.Address();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Valid() const
{
    return m_iter.Valid();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::ValidWith(MoveIterator const& o) const
{
    return m_iter.ValidWith(o.Base());
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Increment()
{
    return m_iter.Increment();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Decrement()
    requires BidrectionalIteratorT<T>
{
    return m_iter.Decrement();
}

template <ForwardIteratorT T>
inline constexpr bool MoveIterator<T>::Advance(OffsetT s)
    requires RandomAccessIteratorT<T>
{
    return m_iter.Advance(s);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Pointer MoveIterator<T>::operator->() const
{
    return m_iter.operator->();
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Reference MoveIterator<T>::operator*() const
{
    return static_cast<Reference>(*m_iter);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>::Reference MoveIterator<T>::operator[](OffsetT n) const
    requires RandomAccessIteratorT<T>
{
    return static_cast<Reference>(m_iter[n]);
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator++()
{
    ++m_iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator++(int32)
{
    MoveIterator<T> t(*this);
    ++(*this);
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator--()
    requires BidrectionalIteratorT<T>
{
    --m_iter;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator--(int32)
    requires BidrectionalIteratorT<T>
{
    MoveIterator<T> t(*this);
    --(*this);
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator+=(OffsetT n)
    requires RandomAccessIteratorT<T>
{
    m_iter += n;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T>& MoveIterator<T>::operator-=(OffsetT n)
    requires RandomAccessIteratorT<T>
{
    m_iter -= n;
    return *this;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator+(OffsetT n) const
    requires RandomAccessIteratorT<T>
{
    MoveIterator t(*this);
    t += n;
    return t;
}

template <ForwardIteratorT T>
inline constexpr MoveIterator<T> MoveIterator<T>::operator-(OffsetT n) const
    requires RandomAccessIteratorT<T>
{
    MoveIterator t(*this);
    t -= n;
    return t;
}

export template <RandomAccessIteratorT T>
inline constexpr MoveIterator<T> operator+(OffsetT n, MoveIterator<T> i)
{
    return i + n;
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr OffsetT operator-(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() - r.Base();
}

export template <ForwardIteratorT T, ForwardIteratorT U>
inline constexpr bool operator==(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() == r.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator<(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() < r.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator>(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() > r.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator<=(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() <= r.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool operator>=(MoveIterator<T> const& l, MoveIterator<U> const& r)
{
    return l.Base() >= r.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr auto operator<=>(MoveIterator<T> const& l, MoveIterator<U> const& r)
    requires ThreeWayComparableWithT<T, U>
{
    return l.Base() <=> r.Base();
}

} // namespace mini