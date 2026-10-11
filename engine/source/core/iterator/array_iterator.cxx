export module mini.core:array_iterator;

import :type;
import :assert;
import :iterator;

namespace mini {

export template <typename T, typename ArrayT>
class ArrayIterator {
private:
    template <typename U, typename A>
    friend class ArrayIterator;
    friend ArrayT;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

protected:
    Pointer m_ptr;
    ArrayT* m_array;

public:
    constexpr ArrayIterator() noexcept;

    template <typename U, typename ArrayU>
    constexpr ArrayIterator(ArrayIterator<U, ArrayU> const& other) noexcept
        requires ConvertibleToT<U*, T*> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

    [[nodiscard]] constexpr Pointer address() const noexcept;
    [[nodiscard]] constexpr bool valid() const noexcept;
    [[nodiscard]] constexpr bool validWith(ArrayIterator const& other) const noexcept;

    constexpr bool reset() noexcept;
    constexpr bool finish() noexcept;
    constexpr bool increment() noexcept;
    constexpr bool decrement() noexcept;
    constexpr bool advance(offset_t offset) noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;
    [[nodiscard]] constexpr Reference operator[](offset_t offset) const noexcept;

    constexpr ArrayIterator& operator++() noexcept;
    constexpr ArrayIterator& operator--() noexcept;
    constexpr ArrayIterator& operator+=(offset_t offset) noexcept;
    constexpr ArrayIterator& operator-=(offset_t offset) noexcept;
    constexpr ArrayIterator operator++(int32) noexcept;
    constexpr ArrayIterator operator--(int32) noexcept;
    constexpr ArrayIterator operator+(offset_t offset) const noexcept;
    constexpr ArrayIterator operator-(offset_t offset) const noexcept;

    template <typename U, typename ArrayU>
    constexpr ArrayIterator& operator=(ArrayIterator<U, ArrayU> const& other) noexcept
        requires ConvertibleToT<U*, T*> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

protected:
    constexpr ArrayIterator(Pointer ptr, ArrayT* base) noexcept;
    constexpr bool check(ArrayIterator const& iter) const noexcept;
};

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>::ArrayIterator() noexcept
    : m_ptr(nullptr)
    , m_array(nullptr)
{
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>::ArrayIterator(Pointer ptr, ArrayT* base) noexcept
    : m_ptr(ptr)
    , m_array(base)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
constexpr ArrayIterator<T, ArrayT>::ArrayIterator(ArrayIterator<U, ArrayU> const& other) noexcept
    requires ConvertibleToT<U*, T*> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_array(other.m_array)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator=(ArrayIterator<U, ArrayU> const& other) noexcept
    requires ConvertibleToT<U*, T*> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    m_ptr = static_cast<T*>(other.m_ptr);
    m_array = other.m_array;
    return *this;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::check(ArrayIterator const& iter) const noexcept
{
    return iter.m_array && iter.m_array->validIterator(iter);
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>::Pointer ArrayIterator<T, ArrayT>::address() const noexcept
{
    return m_ptr;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::valid() const noexcept
{
    return check(*this);
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::validWith(ArrayIterator const& other) const noexcept
{
    return m_array && m_array->validRange(*this, other);
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::reset() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    m_ptr = m_array->begin().m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::finish() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    auto end = m_array->end();
    m_ptr = end.m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::increment() noexcept
{
    if (!check(*this + 1)) [[unlikely]] {
        return false;
    }

    ++m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::decrement() noexcept
{
    if (!check(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::advance(offset_t offset) noexcept
{
    if (!check(*this + offset)) [[unlikely]] {
        return false;
    }

    m_ptr += offset;
    return true;
}

template <typename T, typename ArrayT>
constexpr T* ArrayIterator<T, ArrayT>::operator->() const noexcept
{
    ASSERT(check(*this), "invalid access");
    return m_ptr;
}

template <typename T, typename ArrayT>
constexpr T& ArrayIterator<T, ArrayT>::operator*() const noexcept
{
    ASSERT(check(*this), "invalid access");
    return *m_ptr;
}

template <typename T, typename ArrayT>
constexpr T& ArrayIterator<T, ArrayT>::operator[](offset_t const offset) const noexcept
{
    ASSERT(check(*this + offset), "invalid access");
    return *(m_ptr + offset);
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator++() noexcept
{
    ++m_ptr;
    return *this;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator--() noexcept
{
    --m_ptr;
    return *this;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator+=(offset_t offset) noexcept
{
    m_ptr += offset;
    return *this;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator-=(offset_t offset) noexcept
{
    m_ptr -= offset;
    return *this;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator++(int32) noexcept
{
    ArrayIterator t(*this);
    ++(*this);
    return t;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator--(int32) noexcept
{
    ArrayIterator t(*this);
    --(*this);
    return t;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator+(offset_t offset) const noexcept
{
    ArrayIterator t(*this);
    t.m_ptr += offset;
    return t;
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator-(offset_t offset) const noexcept
{
    ArrayIterator t(*this);
    t.m_ptr -= offset;
    return t;
}

export template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT> operator+(offset_t offset, ArrayIterator<T, ArrayT> const& other) noexcept
{
    return other + offset;
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
constexpr offset_t operator-(ArrayIterator<T, ArrayT> const& lhs, ArrayIterator<U, ArrayU> const& rhs) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    return static_cast<offset_t>(lhs.address() - rhs.address());
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
constexpr bool operator==(ArrayIterator<T, ArrayT> const& lhs, ArrayIterator<U, ArrayU> const& rhs) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && EqualityComparableWithT<T*, U*>
{
    return lhs.address() == rhs.address();
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
constexpr auto operator<=>(ArrayIterator<T, ArrayT> const& lhs, ArrayIterator<U, ArrayU> const& rhs) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && ThreeWayComparableWithT<T*, U*>
{
    return lhs.address() <=> rhs.address();
}

} // namespace mini