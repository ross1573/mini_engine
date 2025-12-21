export module mini.string:iterator;

import mini.type;
import mini.iterator;

namespace mini {

export template <typename T, typename StringT>
class StringIterator {
private:
    template <typename U, typename A>
    friend class StringIterator;
    friend StringT;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

protected:
    Ptr m_Ptr;
    StringT* m_String;

public:
    constexpr StringIterator() noexcept;

    template <typename U, typename StringU>
    constexpr StringIterator(StringIterator<U, StringU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<StringT>, DecayT<StringU>>;

    constexpr Ptr Address() const noexcept;
    constexpr bool IsValid() const noexcept;
    constexpr bool IsValidWith(StringIterator const&) const noexcept;

    constexpr bool Reset() noexcept;
    constexpr bool Finish() noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    constexpr Ref operator[](OffsetT) const noexcept;

    constexpr StringIterator& operator++() noexcept;
    constexpr StringIterator& operator--() noexcept;
    constexpr StringIterator& operator+=(OffsetT) noexcept;
    constexpr StringIterator& operator-=(OffsetT) noexcept;
    constexpr StringIterator operator++(int32) noexcept;
    constexpr StringIterator operator--(int32) noexcept;
    constexpr StringIterator operator+(OffsetT) const noexcept;
    constexpr StringIterator operator-(OffsetT) const noexcept;

    template <typename U, typename StringU>
    constexpr StringIterator& operator=(StringIterator<U, StringU> const&) noexcept
        requires PtrConvertibleToT<U, T> && SameAsT<DecayT<StringT>, DecayT<StringU>>;

protected:
    constexpr StringIterator(Ptr, StringT*) noexcept;
    constexpr bool CheckIterator(StringIterator const&) const noexcept;
};

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>::StringIterator() noexcept
    : m_Ptr(nullptr)
    , m_String(nullptr)
{
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>::StringIterator(Ptr ptr, StringT* base) noexcept
    : m_Ptr(ptr)
    , m_String(base)
{
}

template <typename T, typename StringT>
template <typename U, typename StringU>
inline constexpr StringIterator<T, StringT>::
    StringIterator(StringIterator<U, StringU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<StringT>, DecayT<StringU>>
    : m_Ptr(static_cast<T*>(o.m_Ptr))
    , m_String(o.m_String)
{
}

template <typename T, typename StringT>
template <typename U, typename StringU>
inline constexpr StringIterator<T, StringT>&
StringIterator<T, StringT>::operator=(StringIterator<U, StringU> const& o) noexcept
    requires PtrConvertibleToT<U, T> && SameAsT<DecayT<StringT>, DecayT<StringU>>
{
    m_Ptr = static_cast<T*>(o.m_Ptr);
    m_String = o.m_String;
    return *this;
}

template <typename T, typename StringT>
inline constexpr bool
StringIterator<T, StringT>::CheckIterator(StringIterator const& iter) const noexcept
{
    return iter.m_String && iter.m_String->IsValidIterator(iter);
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>::Ptr
StringIterator<T, StringT>::Address() const noexcept
{
    return m_Ptr;
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::IsValid() const noexcept
{
    return CheckIterator(*this);
}

template <typename T, typename StringT>
inline constexpr bool
StringIterator<T, StringT>::IsValidWith(StringIterator const& o) const noexcept
{
    return m_String && m_String->IsValidRange(*this, o);
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::Reset() noexcept
{
    if (m_String == nullptr) [[unlikely]] {
        return false;
    }

    auto begin = m_String->Begin();
    m_Ptr = begin.m_Ptr;
    return true;
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::Finish() noexcept
{
    if (m_String == nullptr) [[unlikely]] {
        return false;
    }

    auto end = m_String->End();
    m_Ptr = end.m_Ptr;
    return true;
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::Increment() noexcept
{
    if (!CheckIterator(*this + 1)) [[unlikely]] {
        return false;
    }

    ++m_Ptr;
    return true;
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::Decrement() noexcept
{
    if (!CheckIterator(*this - 1)) [[unlikely]] {
        return false;
    }

    --m_Ptr;
    return true;
}

template <typename T, typename StringT>
inline constexpr bool StringIterator<T, StringT>::Advance(OffsetT d) noexcept
{
    if (!CheckIterator(*this + d)) [[unlikely]] {
        return false;
    }

    m_Ptr += d;
    return true;
}

template <typename T, typename StringT>
inline constexpr T* StringIterator<T, StringT>::operator->() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return m_Ptr;
}

template <typename T, typename StringT>
inline constexpr T& StringIterator<T, StringT>::operator*() const noexcept
{
    ASSERT(CheckIterator(*this), "invalid access");
    return *m_Ptr;
}

template <typename T, typename StringT>
inline constexpr T& StringIterator<T, StringT>::operator[](OffsetT const o) const noexcept
{
    ASSERT(CheckIterator(*this + o), "invalid access");
    return *(m_Ptr + o);
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>& StringIterator<T, StringT>::operator++() noexcept
{
    ++m_Ptr;
    return *this;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>& StringIterator<T, StringT>::operator--() noexcept
{
    --m_Ptr;
    return *this;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>&
StringIterator<T, StringT>::operator+=(OffsetT d) noexcept
{
    m_Ptr += d;
    return *this;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>&
StringIterator<T, StringT>::operator-=(OffsetT d) noexcept
{
    m_Ptr -= d;
    return *this;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT> StringIterator<T, StringT>::operator++(int32) noexcept
{
    StringIterator t(*this);
    ++t.m_Ptr;
    return t;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT> StringIterator<T, StringT>::operator--(int32) noexcept
{
    StringIterator t(*this);
    --t.m_Ptr;
    return t;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>
StringIterator<T, StringT>::operator+(OffsetT d) const noexcept
{
    StringIterator t(*this);
    t.m_Ptr += d;
    return t;
}

template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>
StringIterator<T, StringT>::operator-(OffsetT d) const noexcept
{
    StringIterator t(*this);
    t.m_Ptr -= d;
    return t;
}

export template <typename T, typename StringT>
inline constexpr StringIterator<T, StringT>
operator+(OffsetT n, StringIterator<T, StringT> const& iter) noexcept
{
    return iter + n;
}

export template <typename T, typename U, typename StringT, typename StringU>
inline constexpr OffsetT operator-(StringIterator<T, StringT> const& l,
                                   StringIterator<U, StringU> const& r) noexcept
    requires SameAsT<DecayT<StringT>, DecayT<StringU>>
{
    return static_cast<OffsetT>(l.Address() - r.Address());
}

export template <typename T, typename U, typename StringT, typename StringU>
inline constexpr bool operator==(StringIterator<T, StringT> const& l,
                                 StringIterator<U, StringU> const& r) noexcept
    requires SameAsT<DecayT<StringT>, DecayT<StringU>> && EqualityComparableWithT<T*, U*>
{
    return l.Address() == r.Address();
}

export template <typename T, typename U, typename StringT, typename StringU>
inline constexpr auto operator<=>(StringIterator<T, StringT> const& l,
                                  StringIterator<U, StringU> const& r) noexcept
    requires SameAsT<DecayT<StringT>, DecayT<StringU>> && ThreeWayComparableWithT<T*, U*>
{
    return l.Address() <=> r.Address();
}

} // namespace mini