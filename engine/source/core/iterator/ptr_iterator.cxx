export module mini.core:ptr_iterator;

import :type;
import :iterator;

namespace mini {

export template <PointerT T>
class PtrIterator {
private:
    template <PointerT U>
    friend class PtrIterator;

public:
    typedef RemovePtrT<T> Value;
    typedef Value* Pointer;
    typedef Value& Reference;

protected:
    Pointer m_Ptr;

public:
    constexpr PtrIterator() noexcept;
    explicit constexpr PtrIterator(Pointer p) noexcept;

    constexpr Pointer Address() const noexcept;

    constexpr bool Valid() const noexcept;
    constexpr bool ValidWith(PtrIterator const&) const noexcept;
    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(OffsetT) noexcept;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    constexpr Reference operator[](OffsetT) const noexcept;

    constexpr PtrIterator& operator++() noexcept;
    constexpr PtrIterator& operator--() noexcept;
    constexpr PtrIterator& operator+=(OffsetT) noexcept;
    constexpr PtrIterator& operator-=(OffsetT) noexcept;
    constexpr PtrIterator operator++(int32) noexcept;
    constexpr PtrIterator operator--(int32) noexcept;
    constexpr PtrIterator operator+(OffsetT) const noexcept;
    constexpr PtrIterator operator-(OffsetT) const noexcept;

    template <PointerT U>
    constexpr PtrIterator(PtrIterator<U> const&) noexcept
        requires ConvertibleToT<U, T>;

    template <PointerT U>
    constexpr PtrIterator& operator=(PtrIterator<U> const&) noexcept
        requires ConvertibleToT<U, T>;
};

template <PointerT T>
inline constexpr PtrIterator<T>::PtrIterator() noexcept
    : m_Ptr(nullptr)
{
}

template <PointerT T>
inline constexpr PtrIterator<T>::PtrIterator(Pointer p) noexcept
    : m_Ptr(p)
{
}

template <PointerT T>
template <PointerT U>
inline constexpr PtrIterator<T>::PtrIterator(PtrIterator<U> const& o) noexcept
    requires ConvertibleToT<U, T>
    : m_Ptr(o.m_Ptr)
{
}

template <PointerT T>
template <PointerT U>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator=(PtrIterator<U> const& o) noexcept
    requires ConvertibleToT<U, T>
{
    m_Ptr = o.m_Ptr;
    return *this;
}

template <PointerT T>
inline constexpr PtrIterator<T>::Pointer PtrIterator<T>::Address() const noexcept
{
    return m_Ptr;
}

template <PointerT T>
inline constexpr bool PtrIterator<T>::Valid() const noexcept
{
    return true;
}

template <PointerT T>
inline constexpr bool PtrIterator<T>::ValidWith(PtrIterator const&) const noexcept
{
    return true;
}

template <PointerT T>
inline constexpr bool PtrIterator<T>::Increment() noexcept
{
    ++m_Ptr;
    return true;
}

template <PointerT T>
inline constexpr bool PtrIterator<T>::Decrement() noexcept
{
    --m_Ptr;
    return true;
}

template <PointerT T>
inline constexpr bool PtrIterator<T>::Advance(OffsetT s) noexcept
{
    m_Ptr += s;
    return true;
}

template <PointerT T>
inline constexpr RemovePtrT<T>* PtrIterator<T>::operator->() const noexcept
{
    return m_Ptr;
}

template <PointerT T>
inline constexpr RemovePtrT<T>& PtrIterator<T>::operator*() const noexcept
{
    return *m_Ptr;
}

template <PointerT T>
inline constexpr RemovePtrT<T>& PtrIterator<T>::operator[](OffsetT o) const noexcept
{
    return *(m_Ptr + o);
}

template <PointerT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator++() noexcept
{
    ++m_Ptr;
    return *this;
}

template <PointerT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator--() noexcept
{
    --m_Ptr;
    return *this;
}

template <PointerT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator+=(OffsetT d) noexcept
{
    m_Ptr += d;
    return *this;
}

template <PointerT T>
inline constexpr PtrIterator<T>& PtrIterator<T>::operator-=(OffsetT d) noexcept
{
    m_Ptr -= d;
    return *this;
}

template <PointerT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator++(int32) noexcept
{
    PtrIterator t(*this);
    ++(*this);
    return t;
}

template <PointerT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator--(int32) noexcept
{
    PtrIterator t(*this);
    --(*this);
    return t;
}

template <PointerT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator+(OffsetT d) const noexcept
{
    PtrIterator t(*this);
    t += d;
    return t;
}

template <PointerT T>
inline constexpr PtrIterator<T> PtrIterator<T>::operator-(OffsetT d) const noexcept
{
    PtrIterator t(*this);
    t -= d;
    return t;
}

export template <PointerT T>
inline constexpr PtrIterator<T> operator+(OffsetT n, PtrIterator<T> const& iter) noexcept
{
    return iter + n;
}

export template <PointerT T, PointerT U>
inline constexpr OffsetT operator-(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
{
    return static_cast<OffsetT>(l.operator->() - r.operator->());
}

export template <PointerT T, PointerT U>
inline constexpr bool operator==(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    return l.operator->() == r.operator->();
}

export template <PointerT T, PointerT U>
inline constexpr auto operator<=>(PtrIterator<T> const& l, PtrIterator<U> const& r) noexcept
    requires ThreeWayComparableWithT<T, U>
{
    return l.operator->() <=> r.operator->();
}

} // namespace mini