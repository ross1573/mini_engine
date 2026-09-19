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
    Pointer m_ptr;

public:
    constexpr PtrIterator() noexcept;
    explicit constexpr PtrIterator(Pointer ptr) noexcept;

    [[nodiscard]] constexpr Pointer Address() const noexcept;
    [[nodiscard]] constexpr bool Valid() const noexcept;
    [[nodiscard]] constexpr bool ValidWith(PtrIterator const& other) const noexcept;

    constexpr bool Increment() noexcept;
    constexpr bool Decrement() noexcept;
    constexpr bool Advance(offset_t offset) noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;
    [[nodiscard]] constexpr Reference operator[](offset_t offset) const noexcept;

    constexpr PtrIterator& operator++() noexcept;
    constexpr PtrIterator& operator--() noexcept;
    constexpr PtrIterator& operator+=(offset_t offset) noexcept;
    constexpr PtrIterator& operator-=(offset_t offset) noexcept;
    constexpr PtrIterator operator++(int32) noexcept;
    constexpr PtrIterator operator--(int32) noexcept;
    constexpr PtrIterator operator+(offset_t offset) const noexcept;
    constexpr PtrIterator operator-(offset_t offset) const noexcept;

    template <PointerT U>
    constexpr PtrIterator(PtrIterator<U> const& other) noexcept
        requires ConvertibleToT<U, T>;

    template <PointerT U>
    constexpr PtrIterator& operator=(PtrIterator<U> const& other) noexcept
        requires ConvertibleToT<U, T>;
};

template <PointerT T>
constexpr PtrIterator<T>::PtrIterator() noexcept
    : m_ptr(nullptr)
{
}

template <PointerT T>
constexpr PtrIterator<T>::PtrIterator(Pointer ptr) noexcept
    : m_ptr(ptr)
{
}

template <PointerT T>
template <PointerT U>
constexpr PtrIterator<T>::PtrIterator(PtrIterator<U> const& other) noexcept
    requires ConvertibleToT<U, T>
    : m_ptr(other.m_ptr)
{
}

template <PointerT T>
template <PointerT U>
constexpr PtrIterator<T>& PtrIterator<T>::operator=(PtrIterator<U> const& other) noexcept
    requires ConvertibleToT<U, T>
{
    m_ptr = other.m_ptr;
    return *this;
}

template <PointerT T>
constexpr PtrIterator<T>::Pointer PtrIterator<T>::Address() const noexcept
{
    return m_ptr;
}

template <PointerT T>
constexpr bool PtrIterator<T>::Valid() const noexcept
{
    return true;
}

template <PointerT T>
constexpr bool PtrIterator<T>::ValidWith([[maybe_unused]] PtrIterator const& other) const noexcept
{
    return true;
}

template <PointerT T>
constexpr bool PtrIterator<T>::Increment() noexcept
{
    ++m_ptr;
    return true;
}

template <PointerT T>
constexpr bool PtrIterator<T>::Decrement() noexcept
{
    --m_ptr;
    return true;
}

template <PointerT T>
constexpr bool PtrIterator<T>::Advance(offset_t offset) noexcept
{
    m_ptr += offset;
    return true;
}

template <PointerT T>
constexpr RemovePtrT<T>* PtrIterator<T>::operator->() const noexcept
{
    return m_ptr;
}

template <PointerT T>
constexpr RemovePtrT<T>& PtrIterator<T>::operator*() const noexcept
{
    return *m_ptr;
}

template <PointerT T>
constexpr RemovePtrT<T>& PtrIterator<T>::operator[](offset_t offset) const noexcept
{
    return *(m_ptr + offset);
}

template <PointerT T>
constexpr PtrIterator<T>& PtrIterator<T>::operator++() noexcept
{
    ++m_ptr;
    return *this;
}

template <PointerT T>
constexpr PtrIterator<T>& PtrIterator<T>::operator--() noexcept
{
    --m_ptr;
    return *this;
}

template <PointerT T>
constexpr PtrIterator<T>& PtrIterator<T>::operator+=(offset_t offset) noexcept
{
    m_ptr += offset;
    return *this;
}

template <PointerT T>
constexpr PtrIterator<T>& PtrIterator<T>::operator-=(offset_t offset) noexcept
{
    m_ptr -= offset;
    return *this;
}

template <PointerT T>
constexpr PtrIterator<T> PtrIterator<T>::operator++(int32) noexcept
{
    PtrIterator t(*this);
    ++(*this);
    return t;
}

template <PointerT T>
constexpr PtrIterator<T> PtrIterator<T>::operator--(int32) noexcept
{
    PtrIterator t(*this);
    --(*this);
    return t;
}

template <PointerT T>
constexpr PtrIterator<T> PtrIterator<T>::operator+(offset_t offset) const noexcept
{
    PtrIterator t(*this);
    t += offset;
    return t;
}

template <PointerT T>
constexpr PtrIterator<T> PtrIterator<T>::operator-(offset_t offset) const noexcept
{
    PtrIterator t(*this);
    t -= offset;
    return t;
}

export template <PointerT T>
constexpr PtrIterator<T> operator+(offset_t offset, PtrIterator<T> const& iter) noexcept
{
    return iter + offset;
}

export template <PointerT T, PointerT U>
constexpr offset_t operator-(PtrIterator<T> const& lhs, PtrIterator<U> const& rhs) noexcept
{
    return static_cast<offset_t>(lhs.operator->() - rhs.operator->());
}

export template <PointerT T, PointerT U>
constexpr bool operator==(PtrIterator<T> const& lhs, PtrIterator<U> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    return lhs.operator->() == rhs.operator->();
}

export template <PointerT T, PointerT U>
constexpr auto operator<=>(PtrIterator<T> const& lhs, PtrIterator<U> const& rhs) noexcept
    requires ThreeWayComparableWithT<T, U>
{
    return lhs.operator->() <=> rhs.operator->();
}

} // namespace mini