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
    typedef T::Value Value;
    typedef T::Pointer Pointer;
    typedef RemoveRefT<typename T::Reference>&& Reference;

protected:
    T m_iter;

public:
    constexpr MoveIterator() noexcept = default;
    explicit constexpr MoveIterator(T iter);

    [[nodiscard]] constexpr T const& base() const& noexcept;
    [[nodiscard]] constexpr T base() && noexcept;
    [[nodiscard]] constexpr Pointer address() const noexcept;
    [[nodiscard]] constexpr bool valid() const;
    [[nodiscard]] constexpr bool validWith(MoveIterator const& other) const;

    constexpr bool increment();
    constexpr bool decrement()
        requires BidrectionalIteratorT<T>;
    constexpr bool advance(offset_t offset)
        requires RandomAccessIteratorT<T>;

    [[nodiscard]] constexpr Pointer operator->() const;
    [[nodiscard]] constexpr Reference operator*() const;
    [[nodiscard]] constexpr Reference operator[](offset_t offset) const
        requires RandomAccessIteratorT<T>;

    constexpr MoveIterator& operator++();
    constexpr MoveIterator operator++(int32);
    constexpr MoveIterator& operator--()
        requires BidrectionalIteratorT<T>;
    constexpr MoveIterator operator--(int32)
        requires BidrectionalIteratorT<T>;
    constexpr MoveIterator& operator+=(offset_t offset)
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator& operator-=(offset_t offset)
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator+(offset_t offset) const
        requires RandomAccessIteratorT<T>;
    constexpr MoveIterator operator-(offset_t offset) const
        requires RandomAccessIteratorT<T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator(MoveIterator<U> const& other)
        requires ConvertibleToT<U const&, T>;

    template <ForwardIteratorT U>
    constexpr MoveIterator& operator=(MoveIterator<U> const& other)
        requires ConvertibleToT<U const&, T>;
};

template <ForwardIteratorT T>
constexpr MoveIterator<T>::MoveIterator(T iter)
    : m_iter(iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
constexpr MoveIterator<T>::MoveIterator(MoveIterator<U> const& other)
    requires ConvertibleToT<U const&, T>
    : m_iter(other.m_iter)
{
}

template <ForwardIteratorT T>
template <ForwardIteratorT U>
constexpr MoveIterator<T>& MoveIterator<T>::operator=(MoveIterator<U> const& other)
    requires ConvertibleToT<U const&, T>
{
    m_iter = other.m_iter;
    return *this;
}

template <ForwardIteratorT T>
constexpr T const& MoveIterator<T>::base() const& noexcept
{
    return m_iter;
}

template <ForwardIteratorT T>
constexpr T MoveIterator<T>::base() && noexcept
{
    return moveCast(m_iter);
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>::Pointer MoveIterator<T>::address() const noexcept
{
    return m_iter.address();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::valid() const
{
    return m_iter.valid();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::validWith(MoveIterator const& other) const
{
    return m_iter.validWith(other.base());
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::increment()
{
    return m_iter.increment();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::decrement()
    requires BidrectionalIteratorT<T>
{
    return m_iter.decrement();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::advance(offset_t offset)
    requires RandomAccessIteratorT<T>
{
    return m_iter.advance(offset);
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>::Pointer MoveIterator<T>::operator->() const
{
    return m_iter.operator->();
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>::Reference MoveIterator<T>::operator*() const
{
    return static_cast<Reference>(*m_iter);
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>::Reference MoveIterator<T>::operator[](offset_t offset) const
    requires RandomAccessIteratorT<T>
{
    return static_cast<Reference>(m_iter[offset]);
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>& MoveIterator<T>::operator++()
{
    ++m_iter;
    return *this;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T> MoveIterator<T>::operator++(int32)
{
    MoveIterator<T> tmp(*this);
    ++(*this);
    return tmp;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>& MoveIterator<T>::operator--()
    requires BidrectionalIteratorT<T>
{
    --m_iter;
    return *this;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T> MoveIterator<T>::operator--(int32)
    requires BidrectionalIteratorT<T>
{
    MoveIterator<T> tmp(*this);
    --(*this);
    return tmp;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>& MoveIterator<T>::operator+=(offset_t offset)
    requires RandomAccessIteratorT<T>
{
    m_iter += offset;
    return *this;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>& MoveIterator<T>::operator-=(offset_t offset)
    requires RandomAccessIteratorT<T>
{
    m_iter -= offset;
    return *this;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T> MoveIterator<T>::operator+(offset_t offset) const
    requires RandomAccessIteratorT<T>
{
    MoveIterator tmp(*this);
    tmp += offset;
    return tmp;
}

template <ForwardIteratorT T>
constexpr MoveIterator<T> MoveIterator<T>::operator-(offset_t offset) const
    requires RandomAccessIteratorT<T>
{
    MoveIterator tmp(*this);
    tmp -= offset;
    return tmp;
}

export template <RandomAccessIteratorT T>
constexpr MoveIterator<T> operator+(offset_t offset, MoveIterator<T> iter)
{
    return iter + offset;
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr offset_t operator-(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() - rhs.base();
}

export template <ForwardIteratorT T, ForwardIteratorT U>
constexpr bool operator==(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() == rhs.base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator<(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() < rhs.base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator>(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() > rhs.base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator<=(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() <= rhs.base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator>=(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.base() >= rhs.base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr auto operator<=>(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
    requires ThreeWayComparableWithT<T, U>
{
    return lhs.base() <=> rhs.base();
}

} // namespace mini