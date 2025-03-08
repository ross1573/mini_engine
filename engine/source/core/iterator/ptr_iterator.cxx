export module mini.core:ptr_iterator;

import :type;
import :iterator;

export namespace mini
{

template <PtrT T>
class PtrIterator
{
    template <PtrT U>
    friend class PtrIterator;

public:
    typedef RemovePtrT<T> Value;
    typedef Value* Ptr;
    typedef Value& Ref;

protected:
    Ptr m_Ptr;

public:
    constexpr PtrIterator() noexcept;
    explicit constexpr PtrIterator(Ptr p) noexcept;

    constexpr Ptr Address() const noexcept;

    constexpr bool IsValid() const noexcept;
    constexpr bool IsValidWith(PtrIterator const&) const noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    constexpr Ref operator[](OffsetT) const noexcept;

    constexpr PtrIterator& operator++() noexcept;
    constexpr PtrIterator& operator--() noexcept;
    constexpr PtrIterator& operator+=(OffsetT) noexcept;
    constexpr PtrIterator& operator-=(OffsetT) noexcept;
    constexpr PtrIterator operator++(int) noexcept;
    constexpr PtrIterator operator--(int) noexcept;
    constexpr PtrIterator operator+(OffsetT) const noexcept;
    constexpr PtrIterator operator-(OffsetT) const noexcept;

    template <PtrT U>
    constexpr PtrIterator(PtrIterator<U> const&) noexcept
        requires ConvertibleToT<U, T>;

    template <PtrT U>
    constexpr PtrIterator& operator=(PtrIterator<U> const&) noexcept
        requires ConvertibleToT<U, T>;
};

template <PtrT T>
inline constexpr PtrIterator<T>::PtrIterator() noexcept
    : m_Ptr(nullptr)
{
}

template <PtrT T>
inline constexpr PtrIterator<T>::PtrIterator(Ptr p) noexcept
    : m_Ptr(p)
{
}

template <PtrT T>
template <PtrT U>
inline constexpr PtrIterator<T>::PtrIterator(PtrIterator<U> const& o)
noexcept requires ConvertibleToT<U, T>
    : m_Ptr(o.m_Ptr)
{
}

template <PtrT T>
template <PtrT U>
inline constexpr PtrIterator<T>&
PtrIterator<T>::operator=(PtrIterator<U> const& o) noexcept
    requires ConvertibleToT<U, T>
{
    m_Ptr = o.m_Ptr;
    return *this;
}

template <PtrT T>
inline constexpr PtrIterator<T>::Ptr PtrIterator<T>::Address() const noexcept
{
    return m_Ptr;
}

template <PtrT T>
inline constexpr bool PtrIterator<T>::IsValid() const noexcept
{
    return true;
}

template <PtrT T>
inline constexpr bool PtrIterator<T>::IsValidWith(PtrIterator const&) const noexcept
{
    return true;
}

template <PtrT T>
inline constexpr bool PtrIterator<T>::Increment() noexcept
{
    ++m_Ptr;
    return true;
}

template <PtrT T>
inline constexpr bool PtrIterator<T>::Decrement() noexcept
{
    --m_Ptr;
    return true;
}

template <PtrT T>
inline constexpr bool PtrIterator<T>::Advance(OffsetT s) noexcept
{
    m_Ptr += s;
    return true;
}

template <PtrT T>
inline constexpr RemovePtrT<T>* PtrIterator<T>::operator->() const noexcept
{
    return m_Ptr;
}

template <PtrT T>
inline constexpr RemovePtrT<T>& PtrIterator<T>::operator*() const noexcept
{
    return *m_Ptr;
}

template <PtrT T>
inline constexpr RemovePtrT<T>& PtrIterator<T>::operator[](OffsetT o) const noexcept
{
    return *(m_Ptr + o);
}

template <PtrT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator++() noexcept
{
    ++m_Ptr;
    return *this;
}

template <PtrT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator--() noexcept
{
    --m_Ptr;
    return *this;
}

template <PtrT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator+=(OffsetT d) noexcept
{
    m_Ptr += d;
    return *this;
}

template <PtrT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator-=(OffsetT d) noexcept
{
    m_Ptr -= d;
    return *this;
}

template <PtrT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator++(int) noexcept
{
    PtrIterator t(*this);
    ++(*this);
    return t;
}

template <PtrT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator--(int) noexcept
{
    PtrIterator t(*this);
    --(*this);
    return t;
}

template <PtrT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator+(OffsetT d) const noexcept
{
    PtrIterator t(*this);
    t += d;
    return t;
}

template <PtrT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator-(OffsetT d) const noexcept
{
    PtrIterator t(*this);
    t -= d;
    return t;
}

template <PtrT T>
inline constexpr PtrIterator<T> operator+(OffsetT n, PtrIterator<T> const& iter) noexcept
{
    return iter + n;
}

template <PtrT T, PtrT U>
inline constexpr OffsetT operator-(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
{
    return static_cast<OffsetT>(l.operator->() - r.operator->());
}

template <PtrT T, PtrT U>
inline constexpr bool operator==(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    return l.operator->() == r.operator->();
}

template <PtrT T, PtrT U>
inline constexpr auto operator<=>(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
    requires ThreeWayComparableWithT<T, U>
{
    return l.operator->() <=> r.operator->();
}

} // namespace mini

export namespace mini::memory
{

template <PtrT T>
inline constexpr T AddressOf(PtrIterator<T> const& iter) noexcept
{
    return iter.operator->();
}

} // namespace mini::memory