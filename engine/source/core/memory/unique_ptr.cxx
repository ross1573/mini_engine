export module mini.core:unique_ptr;

import :type;
import :deleter;
import :utility_operation;

namespace mini {

export template <NonRefT T, DeleterT<T> DelT = DefaultDeleter<T>>
class UniquePtr {
private:
    template <NonRefT U, DeleterT<U> DelU>
    friend class UniquePtr;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

private:
    Pointer m_ptr;
    [[emptyable_address]] DelT m_deleter;

public:
    constexpr UniquePtr() noexcept;
    constexpr ~UniquePtr() noexcept;
    constexpr UniquePtr(UniquePtr&&) noexcept;
    explicit constexpr UniquePtr(Pointer) noexcept;
    constexpr UniquePtr(Pointer, DelT const&) noexcept;
    constexpr UniquePtr(Pointer, DelT&&) noexcept;
    constexpr UniquePtr(NullptrT) noexcept;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    constexpr UniquePtr(UniquePtr<T, DelU>&&) noexcept
        requires ConvertibleToT<DelU, DelT>;

    constexpr Pointer Get() const noexcept;
    constexpr bool Valid() const noexcept;

    constexpr Pointer Detach() noexcept;
    constexpr void Swap(UniquePtr&) noexcept;
    constexpr void Reset(NullptrT) noexcept;
    constexpr void Reset(Pointer = Pointer()) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U*) noexcept;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Pointer() const noexcept;

    constexpr UniquePtr& operator=(NullptrT) noexcept;
    constexpr UniquePtr& operator=(UniquePtr&&) noexcept;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    constexpr UniquePtr& operator=(UniquePtr<U, DelU>&&) noexcept
        requires ConvertibleToT<DelU, DelT>;

private:
    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    UniquePtr(UniquePtr<T, DelU> const&) = delete;
    UniquePtr(UniquePtr const&) = delete;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    UniquePtr& operator=(UniquePtr<U, DelU> const&) = delete;
    UniquePtr& operator=(UniquePtr const&) = delete;
};

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr() noexcept
    : m_ptr(nullptr)
    , m_deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::~UniquePtr() noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_deleter(MoveArg(other.m_deleter))
{
    other.m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr, DelT const& del) noexcept
    : m_ptr(ptr)
    , m_deleter(del)
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr, DelT&& del) noexcept
    : m_ptr(ptr)
    , m_deleter(MoveArg(del))
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(NullptrT) noexcept
    : m_ptr(nullptr)
    , m_deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
inline constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr<T, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
    : m_ptr(static_cast<Pointer>(other.m_ptr))
    , m_deleter(MoveArg(other.m_deleter))
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::Get() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool UniquePtr<T, DelT>::Valid() const noexcept
{
    return m_ptr != nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::Detach() noexcept
{
    Pointer tmp = m_ptr;
    m_ptr = nullptr;
    return tmp;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Swap(UniquePtr& other) noexcept
{
    mini::Swap(m_ptr, other.m_ptr);
    mini::Swap(m_deleter, other.m_deleter);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Reset(NullptrT) noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Reset(Pointer ptr) noexcept
{
    m_deleter(m_ptr);
    m_ptr = ptr;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U>
inline constexpr void UniquePtr<T, DelT>::Reset(U* ptr) noexcept
{
    m_deleter(m_ptr);
    m_ptr = static_cast<Pointer>(ptr);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::operator->() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Reference UniquePtr<T, DelT>::operator*() const noexcept
{
    return *(m_ptr);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::operator bool() const noexcept
{
    return m_ptr != nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::operator Pointer() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(NullptrT) noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(UniquePtr&& other) noexcept
{
    m_deleter(m_ptr);
    m_ptr = other.m_ptr;
    m_deleter = MoveArg(other.m_deleter);
    other.m_ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
inline constexpr UniquePtr<T, DelT>&
UniquePtr<T, DelT>::operator=(UniquePtr<U, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
{
    m_deleter(m_ptr);
    m_ptr = static_cast<Pointer>(other.m_ptr);
    m_deleter = MoveArg(other.m_deleter);
    other.m_ptr = nullptr;
    return *this;
}

export template <NonRefT T, typename... Args>
inline constexpr UniquePtr<T> MakeUnique(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return UniquePtr<T>(new T(ForwardArg<Args>(args)...));
}

export template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
inline constexpr bool operator==(UniquePtr<T, DelT> const& l, UniquePtr<U, DelU> const& r) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return l.Get() == r.Get();
}

export template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
inline constexpr bool operator<=>(UniquePtr<T, DelT> const& l, UniquePtr<U, DelU> const& r) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return l.Get() <=> r.Get();
}

export template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool operator==(UniquePtr<T, DelT> const& p, NullptrT) noexcept
{
    return p.Get() == nullptr;
}

export template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool operator<=>(UniquePtr<T, DelT> const& p, NullptrT) noexcept
{
    return p.Get() <=> nullptr;
}

} // namespace mini