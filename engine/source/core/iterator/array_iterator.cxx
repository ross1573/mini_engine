module;

#include "core/assert.h"

export module mini.core:array_iterator;

import :type;
import :iterator;
import :iterator_version;

export namespace mini
{

template <
    typename T,
    typename ArrayT
>
class ArrayIterator
{
    template <typename U, typename A>
    friend class ArrayIterator;
    friend ArrayT;

    using VersionT = IteratorVersion<ArrayT>;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

protected:
    Ptr m_Ptr;
    ArrayT* m_Array;
    [[no_unique_address]] VersionT m_Version;

public:
    constexpr ArrayIterator() noexcept;

    template <typename U, typename ArrayU>
    constexpr ArrayIterator(ArrayIterator<U, ArrayU> const&) noexcept
        requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

    constexpr Ptr Address() const noexcept;

    constexpr bool IsValid() const noexcept;
    constexpr bool IsValidWith(ArrayIterator const&) const noexcept;
    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    constexpr Ref operator[](OffsetT) const noexcept;

    constexpr ArrayIterator& operator++() noexcept;
    constexpr ArrayIterator& operator--() noexcept;
    constexpr ArrayIterator& operator+=(OffsetT) noexcept;
    constexpr ArrayIterator& operator-=(OffsetT) noexcept;
    constexpr ArrayIterator operator++(int) noexcept;
    constexpr ArrayIterator operator--(int) noexcept;
    constexpr ArrayIterator operator+(OffsetT) const noexcept;
    constexpr ArrayIterator operator-(OffsetT) const noexcept;

    template <typename U, typename ArrayU>
    constexpr ArrayIterator& operator=(ArrayIterator<U, ArrayU> const&) noexcept
        requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>;

protected:
    constexpr ArrayIterator(Ptr, SizeT, ArrayT*) noexcept;

    constexpr bool CheckSource(ArrayIterator const&) const noexcept;
    constexpr bool CheckIterator(ArrayIterator const&) const noexcept;
};

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::ArrayIterator() noexcept
    : m_Ptr(nullptr)
    , m_Array(nullptr)
    , m_Version(0)
{
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::ArrayIterator(Ptr ptr, SizeT ver,
                                                                   ArrayT* base) noexcept
    : m_Ptr(ptr)
    , m_Array(base)
    , m_Version(ver)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
inline constexpr
ArrayIterator<T, ArrayT>::ArrayIterator(ArrayIterator<U, ArrayU> const& o)
noexcept requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
    : m_Ptr(static_cast<T*>(o.m_Ptr))
    , m_Array(o.m_Array)
    , m_Version(o.m_Version)
{
}

template <typename T, typename ArrayT>
template <typename U, typename ArrayU>
inline constexpr ArrayIterator<T, ArrayT>&
ArrayIterator<T, ArrayT>::operator=(ArrayIterator<U, ArrayU> const& o) noexcept
    requires PtrConvertibleToT<U, T>&& SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    m_Ptr = static_cast<T*>(o.m_Ptr);
    m_Array = o.m_Array;
    m_Version = o.m_Version;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::CheckSource(ArrayIterator const& iter) const noexcept
{
    return m_Array && m_Array == iter.m_Array && m_Version == iter.m_Version;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::CheckIterator(ArrayIterator const& iter) const noexcept
{
    return iter.m_Array && iter.m_Array->IsValidIterator(iter);
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>::Ptr ArrayIterator<T, ArrayT>::Address() const noexcept
{
    return m_Ptr;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::IsValid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::IsValidWith(ArrayIterator const& o) const noexcept
{
    return CheckSource(o);
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Reset() noexcept
{
    if (m_Array == nullptr) [[unlikely]]
    {
        return false;
    }

    auto begin = m_Array->Begin();
    m_Ptr = begin.m_Ptr;
    m_Version = begin.m_Version;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Finish() noexcept
{
    if (m_Array == nullptr) [[unlikely]]
    {
        return false;
    }

    auto end = m_Array->End();
    m_Ptr = end.m_Ptr;
    m_Version = end.m_Version;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]]
    {
        return false;
    }

    ++m_Ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]]
    {
        return false;
    }

    --m_Ptr;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr bool ArrayIterator<T, ArrayT>::Advance(OffsetT d) noexcept
{
    if (!CheckIterator(*this + d)) [[unlikely]]
    {
        return false;
    }

    m_Ptr += d;
    return true;
}

template <typename T, typename ArrayT>
inline constexpr T* ArrayIterator<T, ArrayT>::operator->() const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this), "invalid access");
    return m_Ptr;
}

template <typename T, typename ArrayT>
inline constexpr T& ArrayIterator<T, ArrayT>::operator*() const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this), "invalid access");
    return *m_Ptr;
}

template <typename T, typename ArrayT>
inline constexpr T& ArrayIterator<T, ArrayT>::operator[](OffsetT const o) const noexcept
{
    CONSTEXPR_ASSERT(CheckIterator(*this + o), "invalid access");
    return *(m_Ptr + o);
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator++() noexcept
{
    ++m_Ptr;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator--() noexcept
{
    --m_Ptr;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator+=(OffsetT d) noexcept
{
    m_Ptr += d;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>& ArrayIterator<T, ArrayT>::operator-=(OffsetT d) noexcept
{
    m_Ptr -= d;
    return *this;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator++(int) noexcept
{
    ArrayIterator t(*this);
    ++t.m_Ptr;
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator--(int) noexcept
{
    ArrayIterator t(*this);
    --t.m_Ptr;
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator+(OffsetT d) const noexcept
{
    ArrayIterator t(*this);
    t.m_Ptr += d;
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT> ArrayIterator<T, ArrayT>::operator-(OffsetT d) const noexcept
{
    ArrayIterator t(*this);
    t.m_Ptr -= d;
    return t;
}

template <typename T, typename ArrayT>
inline constexpr ArrayIterator<T, ArrayT>
operator+(OffsetT n, ArrayIterator<T, ArrayT> const& iter) noexcept
{
    return iter + n;
}

template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr OffsetT operator-(ArrayIterator<T, ArrayT> const& l,
                                             ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>
{
    return static_cast<OffsetT>(l.Address() - r.Address());
}

template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr bool operator==(ArrayIterator<T, ArrayT> const& l,
                                           ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>&& EqualityComparableWithT<T*, U*>
{
    return l.Address() == r.Address();
}

template <typename T, typename U, typename ArrayT, typename ArrayU>
inline constexpr auto operator<=>(ArrayIterator<T, ArrayT> const& l,
                                            ArrayIterator<U, ArrayU> const& r) noexcept
    requires SameAsT<DecayT<ArrayT>, DecayT<ArrayU>>&& ThreeWayComparableWithT<T*, U*>
{
    return l.Address() <=> r.Address();
}

} // namespace mini