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

    [[nodiscard]] constexpr T const& Base() const& noexcept;
    [[nodiscard]] constexpr T Base() && noexcept;
    [[nodiscard]] constexpr Pointer Address() const noexcept;
    [[nodiscard]] constexpr bool Valid() const;
    [[nodiscard]] constexpr bool ValidWith(MoveIterator const& other) const;

    constexpr bool Increment();
    constexpr bool Decrement()
        requires BidrectionalIteratorT<T>;
    constexpr bool Advance(offset_t offset)
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
constexpr T const& MoveIterator<T>::Base() const& noexcept
{
    return m_iter;
}

template <ForwardIteratorT T>
constexpr T MoveIterator<T>::Base() && noexcept
{
    return MoveArg(m_iter);
}

template <ForwardIteratorT T>
constexpr MoveIterator<T>::Pointer MoveIterator<T>::Address() const noexcept
{
    return m_iter.Address();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::Valid() const
{
    return m_iter.Valid();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::ValidWith(MoveIterator const& other) const
{
    return m_iter.ValidWith(other.Base());
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::Increment()
{
    return m_iter.Increment();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::Decrement()
    requires BidrectionalIteratorT<T>
{
    return m_iter.Decrement();
}

template <ForwardIteratorT T>
constexpr bool MoveIterator<T>::Advance(offset_t offset)
    requires RandomAccessIteratorT<T>
{
    return m_iter.Advance(offset);
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
    return lhs.Base() - rhs.Base();
}

export template <ForwardIteratorT T, ForwardIteratorT U>
constexpr bool operator==(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.Base() == rhs.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator<(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.Base() < rhs.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator>(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.Base() > rhs.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator<=(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.Base() <= rhs.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool operator>=(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
{
    return lhs.Base() >= rhs.Base();
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr auto operator<=>(MoveIterator<T> const& lhs, MoveIterator<U> const& rhs)
    requires ThreeWayComparableWithT<T, U>
{
    return lhs.Base() <=> rhs.Base();
}

} // namespace mini