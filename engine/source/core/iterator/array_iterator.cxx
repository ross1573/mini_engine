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
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

    [[nodiscard]] constexpr Pointer Address() const noexcept;
    [[nodiscard]] constexpr bool Valid() const noexcept;
    [[nodiscard]] constexpr bool ValidWith(ArrayIterator const& other) const noexcept;

    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(offset_t offset) noexcept;

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
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

protected:
    constexpr ArrayIterator(Pointer ptr, ArrayT* base) noexcept;
    constexpr bool CheckIterator(ArrayIterator const& iter) const noexcept;
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
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_array(other.m_array)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator=(ArrayIterator<U, ArrayU> const& other) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    m_ptr = static_cast<T*>(other.m_ptr);
    m_array = other.m_array;
    return *this;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::CheckIterator(ArrayIterator const& iter) const noexcept
{
    return iter.m_array && iter.m_array->ValidIterator(iter);
}

template <typename T, typename ArrayT>
constexpr ArrayIterator<T, ArrayT>::Pointer ArrayIterator<T, ArrayT>::Address() const noexcept
{
    return m_ptr;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Valid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::ValidWith(ArrayIterator const& other) const noexcept
{
    return m_array && m_array->ValidRange(*this, other);
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Reset() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    auto begin = m_array->Begin();
    m_ptr = begin.m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Finish() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    auto end = m_array->End();
    m_ptr = end.m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]] {
        return false;
    }

    ++m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_ptr;
    return true;
}

template <typename T, typename ArrayT>
constexpr bool ArrayIterator<T, ArrayT>::Advance(offset_t offset) noexcept
{
    if (!CheckIterator(*this + offset)) [[unlikely]] {
        return false;
    }

    m_ptr += offset;
    return true;
}

template <typename T, typename ArrayT>
constexpr T* ArrayIterator<T, ArrayT>::operator->() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return m_ptr;
}

template <typename T, typename ArrayT>
constexpr T& ArrayIterator<T, ArrayT>::operator*() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return *m_ptr;
}

template <typename T, typename ArrayT>
constexpr T& ArrayIterator<T, ArrayT>::operator[](offset_t const offset) const noexcept
{
    ASSERT(CheckIterator(*this + offset), "invalid access");
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
    return static_cast<offset_t>(lhs.Address() - rhs.Address());
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
constexpr bool operator==(ArrayIterator<T, ArrayT> const& lhs, ArrayIterator<U, ArrayU> const& rhs) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && EqualityComparableWithT<T*, U*>
{
    return lhs.Address() == rhs.Address();
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
constexpr auto operator<=>(ArrayIterator<T, ArrayT> const& lhs, ArrayIterator<U, ArrayU> const& rhs) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && ThreeWayComparableWithT<T*, U*>
{
    return lhs.Address() <=> rhs.Address();
}

} // namespace mini