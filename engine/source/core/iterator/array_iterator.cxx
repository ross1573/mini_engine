export module mini.core:array_iterator;

import :type;
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
    constexpr ArrayIterator(ArrayIterator<U, ArrayU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

    constexpr Pointer Address() const noexcept;
    constexpr bool Valid() const noexcept;
    constexpr bool ValidWith(ArrayIterator const&) const noexcept;

    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    constexpr Reference operator[](OffsetT) const noexcept;

    constexpr ArrayIterator& operator++() noexcept;
    constexpr ArrayIterator& operator--() noexcept;
    constexpr ArrayIterator& operator+=(OffsetT) noexcept;
    constexpr ArrayIterator& operator-=(OffsetT) noexcept;
    constexpr ArrayIterator operator++(int32) noexcept;
    constexpr ArrayIterator operator--(int32) noexcept;
    constexpr ArrayIterator operator+(OffsetT) const noexcept;
    constexpr ArrayIterator operator-(OffsetT) const noexcept;

    template <typename U, typename ArrayU>
    constexpr ArrayIterator& operator=(ArrayIterator<U, ArrayU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

protected:
    constexpr ArrayIterator(Pointer, ArrayT*) noexcept;
    constexpr bool CheckIterator(ArrayIterator const&) const noexcept;
};

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::ArrayIterator() noexcept
    : m_ptr(nullptr)
    , m_array(nullptr)
{
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::ArrayIterator(Pointer ptr, ArrayT* base) noexcept
    : m_ptr(ptr)
    , m_array(base)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
inline constexpr ArrayIterator<T, ArrayT>::ArrayIterator(ArrayIterator<U, ArrayU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
    : m_ptr(static_cast<T*>(o.m_ptr))
    , m_array(o.m_array)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
inline constexpr ArrayIterator<T, ArrayT>&
ArrayIterator<T, ArrayT>::operator=(ArrayIterator<U, ArrayU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    m_ptr = static_cast<T*>(o.m_ptr);
    m_array = o.m_array;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr bool
ArrayIterator<T, ArrayT>::CheckIterator(ArrayIterator const& iter) const noexcept
{
    return iter.m_array && iter.m_array->ValidIterator(iter);
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::Pointer
ArrayIterator<T, ArrayT>::Address() const noexcept
{
    return m_ptr;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Valid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::ValidWith(ArrayIterator const& o) const noexcept
{
    return m_array && m_array->ValidRange(*this, o);
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Reset() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    auto begin = m_array->Begin();
    m_ptr = begin.m_ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Finish() noexcept
{
    if (m_array == nullptr) [[unlikely]] {
        return false;
    }

    auto end = m_array->End();
    m_ptr = end.m_ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]] {
        return false;
    }

    ++m_ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Advance(OffsetT d) noexcept
{
    if (!CheckIterator(*this + d)) [[unlikely]] {
        return false;
    }

    m_ptr += d;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr T* ArrayIterator<T, ArrayT>::operator->() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return m_ptr;
}

template <typename T, typename ArrayT>
inline constexpr T& ArrayIterator<T, ArrayT>::operator*() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return *m_ptr;
}

template <typename T, typename ArrayT>
inline constexpr T& ArrayIterator<T, ArrayT>::operator[](OffsetT const o) const noexcept
{
    ASSERT(CheckIterator(*this + o), "invalid access");
    return *(m_ptr + o);
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator++() noexcept
{
    ++m_ptr;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator--() noexcept
{
    --m_ptr;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator+=(OffsetT d) noexcept
{
    m_ptr += d;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator-=(OffsetT d) noexcept
{
    m_ptr -= d;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator++(int32) noexcept
{
    ArrayIterator t(*this);
    ++(*this);
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator--(int32) noexcept
{
    ArrayIterator t(*this);
    --(*this);
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>
ArrayIterator<T, ArrayT>::operator+(OffsetT d) const noexcept
{
    ArrayIterator t(*this);
    t.m_ptr += d;
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>
ArrayIterator<T, ArrayT>::operator-(OffsetT d) const noexcept
{
    ArrayIterator t(*this);
    t.m_ptr -= d;
    return t;
}

export template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> operator+(OffsetT n,
                                                    ArrayIterator<T, ArrayT> const& iter) noexcept
{
    return iter + n;
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr OffsetT operator-(ArrayIterator<T, ArrayT> const& l,
                                   ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    return static_cast<OffsetT>(l.Address() - r.Address());
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr bool operator==(ArrayIterator<T, ArrayT> const& l,
                                 ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && EqualityComparableWithT<T*, U*>
{
    return l.Address() == r.Address();
}

export template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr auto operator<=>(ArrayIterator<T, ArrayT> const& l,
                                  ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>> && ThreeWayComparableWithT<T*, U*>
{
    return l.Address() <=> r.Address();
}

} // namespace mini