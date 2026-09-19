export module mini.core:circular_iterator;

import :type;
import :assert;
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
    size_t m_offset;
    size_t m_capacity;
    Pointer m_begin;
    CircularT* m_circular;

public:
    constexpr CircularIterator() noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator(CircularIterator<U, CircularU> const& other) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

    [[nodiscard]] constexpr Pointer Address() const noexcept;
    [[nodiscard]] constexpr bool Valid() const noexcept;
    [[nodiscard]] constexpr bool ValidWith(CircularIterator const& other) const noexcept;

    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(offset_t offset) noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;
    [[nodiscard]] constexpr Reference operator[](offset_t offset) const noexcept;

    constexpr CircularIterator& operator++() noexcept;
    constexpr CircularIterator& operator--() noexcept;
    constexpr CircularIterator& operator+=(offset_t offset) noexcept;
    constexpr CircularIterator& operator-=(offset_t offset) noexcept;
    constexpr CircularIterator operator++(int32) noexcept;
    constexpr CircularIterator operator--(int32) noexcept;
    constexpr CircularIterator operator+(offset_t offset) const noexcept;
    constexpr CircularIterator operator-(offset_t offset) const noexcept;

    template <typename U, typename CircularU>
    constexpr CircularIterator& operator=(CircularIterator<U, CircularU> const& other) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>;

protected:
    constexpr CircularIterator(size_t idx, size_t cap, Pointer begin, CircularT* base) noexcept;
    constexpr bool CheckIterator(CircularIterator const& iter) const noexcept;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr offset_t operator-(CircularIterator<U, CircularU> const& lhs,
                                        CircularIterator<Y, CircularY> const& rhs) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr bool operator==(CircularIterator<U, CircularU> const& lhs,
                                     CircularIterator<Y, CircularY> const& rhs) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>> && EqualityComparableWithT<U*, Y*>;

    template <typename U, typename CircularU, typename Y, typename CircularY>
    friend constexpr auto operator<=>(CircularIterator<U, CircularU> const& lhs,
                                      CircularIterator<Y, CircularY> const& rhs) noexcept
        requires SameAsT<DecayT<CircularU>, DecayT<CircularY>> && ThreeWayComparableWithT<U*, Y*>;
};

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>::CircularIterator() noexcept
    : m_offset(0)
    , m_capacity(0)
    , m_begin(nullptr)
    , m_circular(nullptr)
{
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>::
    CircularIterator(size_t idx, size_t cap, Pointer begin, CircularT* base) noexcept
    : m_offset(idx)
    , m_capacity(cap)
    , m_begin(begin)
    , m_circular(base)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
constexpr CircularIterator<T, CircularT>::CircularIterator(CircularIterator<U, CircularU> const& other) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>
    : m_offset(other.m_offset)
    , m_capacity(other.m_capacity)
    , m_begin(other.m_begin)
    , m_circular(other.m_circular)
{
}

template <typename T, typename CircularT>
template <typename U, typename CircularU>
constexpr CircularIterator<T, CircularT>& CircularIterator<T, CircularT>::
operator=(CircularIterator<U, CircularU> const& other) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    m_offset = other.m_offset;
    m_capacity = other.m_capacity;
    m_begin = other.m_begin;
    m_circular = other.m_circular;
    return *this;
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::CheckIterator(CircularIterator const& iter) const noexcept
{
    return iter.m_circular && iter.m_circular->ValidIterator(iter);
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>::Pointer CircularIterator<T, CircularT>::Address() const noexcept
{
    return m_begin + (m_offset % m_capacity);
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::Valid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::ValidWith(CircularIterator const& other) const noexcept
{
    return m_circular && m_circular->ValidRange(*this, other);
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::Reset() noexcept
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
constexpr bool CircularIterator<T, CircularT>::Finish() noexcept
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
constexpr bool CircularIterator<T, CircularT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]] {
        return false;
        ;
    }

    ++m_offset;
    return true;
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_offset;
    return true;
}

template <typename T, typename CircularT>
constexpr bool CircularIterator<T, CircularT>::Advance(offset_t offset) noexcept
{
    if (offset >= static_cast<offset_t>(m_capacity) || !CheckIterator(*this + offset)) [[unlikely]] {
        return false;
    }

    m_offset = static_cast<size_t>(static_cast<offset_t>(m_offset) + offset);
    return true;
}

template <typename T, typename CircularT>
constexpr T* CircularIterator<T, CircularT>::operator->() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return m_begin + (m_offset % m_capacity);
}

template <typename T, typename CircularT>
constexpr T& CircularIterator<T, CircularT>::operator*() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return *(m_begin + (m_offset % m_capacity));
}

template <typename T, typename CircularT>
constexpr T& CircularIterator<T, CircularT>::operator[](offset_t const offset) const noexcept
{
    ASSERT(CheckIterator(*this + offset), "invalid access");
    return *(m_begin + ((m_offset + offset) % m_capacity));
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>& CircularIterator<T, CircularT>::operator++() noexcept
{
    ++m_offset;
    return *this;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>& CircularIterator<T, CircularT>::operator--() noexcept
{
    --m_offset;
    return *this;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>& CircularIterator<T, CircularT>::operator+=(offset_t offset) noexcept
{
    m_offset = static_cast<size_t>(static_cast<offset_t>(m_offset) + offset);
    return *this;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT>& CircularIterator<T, CircularT>::operator-=(offset_t offset) noexcept
{
    m_offset = static_cast<size_t>(static_cast<offset_t>(m_offset) - offset);
    return *this;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT> CircularIterator<T, CircularT>::operator++(int32) noexcept
{
    CircularIterator tmp(*this);
    ++(*this);
    return tmp;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT> CircularIterator<T, CircularT>::operator--(int32) noexcept
{
    CircularIterator tmp(*this);
    --(*this);
    return tmp;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT> CircularIterator<T, CircularT>::operator+(offset_t offset) const noexcept
{
    CircularIterator tmp(*this);
    tmp += offset;
    return tmp;
}

template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT> CircularIterator<T, CircularT>::operator-(offset_t offset) const noexcept
{
    CircularIterator tmp(*this);
    tmp -= offset;
    return tmp;
}

export template <typename T, typename CircularT>
constexpr CircularIterator<T, CircularT> operator+(offset_t offset, CircularIterator<T, CircularT> const& iter) noexcept
{
    return iter + offset;
}

export template <typename T, typename CircularT, typename U, typename CircularU>
constexpr offset_t operator-(CircularIterator<T, CircularT> const& lhs,
                             CircularIterator<U, CircularU> const& rhs) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>>
{
    ASSERT(lhs.m_begin == rhs.m_begin);
    return static_cast<offset_t>(lhs.m_offset) - static_cast<offset_t>(rhs.m_offset);
}

export template <typename T, typename CircularT, typename U, typename CircularU>
constexpr bool operator==(CircularIterator<T, CircularT> const& lhs, CircularIterator<U, CircularU> const& rhs) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>> && EqualityComparableWithT<T*, U*>
{
    return (lhs.m_offset == rhs.m_offset) && (lhs.m_begin == rhs.m_begin);
}

export template <typename T, typename CircularT, typename U, typename CircularU>
constexpr auto operator<=>(CircularIterator<T, CircularT> const& lhs,
                           CircularIterator<U, CircularU> const& rhs) noexcept
    requires SameAsT<DecayT<CircularT>, DecayT<CircularU>> && ThreeWayComparableWithT<T*, U*>
{
    ASSERT(lhs.m_begin == rhs.m_begin);
    return lhs.m_offset <=> rhs.m_offset;
}

} // namespace mini