export module mini.core:unique_ptr;

import :type;
import :utility;
import :allocator;
import :deleter;

export namespace mini {

template <NonRefT T, DeleterT<T> DelT = DefaultDeleter<T>>
class UniquePtr {
private:
    template <NonRefT U, DeleterT<U> DelU>
    friend class UniquePtr;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

private:
    Ptr m_Ptr;
    [[no_unique_address]] DelT m_Deleter;

public:
    constexpr UniquePtr() noexcept;
    constexpr ~UniquePtr();
    constexpr UniquePtr(UniquePtr&&) noexcept;
    explicit constexpr UniquePtr(Ptr) noexcept;
    constexpr UniquePtr(Ptr, DelT const&) noexcept;
    constexpr UniquePtr(Ptr, DelT&&) noexcept;
    constexpr UniquePtr(NullptrT) noexcept;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    constexpr UniquePtr(UniquePtr<T, DelU>&&) noexcept
        requires ConvertibleToT<DelU, DelT>;

    constexpr Ptr Get() const noexcept;
    constexpr bool IsValid() const noexcept;

    constexpr Ptr Detach() noexcept;
    constexpr void Swap(UniquePtr&) noexcept;
    constexpr void Reset(NullptrT) noexcept;
    constexpr void Reset(Ptr = Ptr()) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U*) noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Ptr() const noexcept;

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
    : m_Ptr(nullptr)
    , m_Deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::~UniquePtr()
{
    m_Deleter(m_Ptr);
    m_Ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr&& other) noexcept
    : m_Ptr(other.m_Ptr)
    , m_Deleter(MoveArg(other.m_Deleter))
{
    other.m_Ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Ptr ptr) noexcept
    : m_Ptr(ptr)
    , m_Deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Ptr ptr, DelT const& del) noexcept
    : m_Ptr(ptr)
    , m_Deleter(del)
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(Ptr ptr, DelT&& del) noexcept
    : m_Ptr(ptr)
    , m_Deleter(MoveArg(del))
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::UniquePtr(NullptrT) noexcept
    : m_Ptr(nullptr)
    , m_Deleter{}
{
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
inline constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr<T, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
    : m_Ptr(static_cast<Ptr>(other.m_Ptr))
    , m_Deleter(MoveArg(other.m_Deleter))
{
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Ptr UniquePtr<T, DelT>::Get() const noexcept
{
    return m_Ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool UniquePtr<T, DelT>::IsValid() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Ptr UniquePtr<T, DelT>::Detach() noexcept
{
    Ptr tmp = m_Ptr;
    m_Ptr = nullptr;
    return tmp;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Swap(UniquePtr& other) noexcept
{
    mini::Swap(m_Ptr, other.m_Ptr);
    mini::Swap(m_Deleter, other.m_Deleter);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Reset(NullptrT) noexcept
{
    m_Deleter(m_Ptr);
    m_Ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr void UniquePtr<T, DelT>::Reset(Ptr ptr) noexcept
{
    m_Deleter(m_Ptr);
    m_Ptr = ptr;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U>
inline constexpr void UniquePtr<T, DelT>::Reset(U* ptr) noexcept
{
    m_Deleter(m_Ptr);
    m_Ptr = static_cast<Ptr>(ptr);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Ptr UniquePtr<T, DelT>::operator->() const noexcept
{
    return m_Ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::Ref UniquePtr<T, DelT>::operator*() const noexcept
{
    return *(m_Ptr);
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::operator bool() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>::operator Ptr() const noexcept
{
    return m_Ptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(NullptrT) noexcept
{
    m_Deleter(m_Ptr);
    m_Ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(UniquePtr&& other) noexcept
{
    m_Deleter(m_Ptr);
    m_Ptr = other.m_Ptr;
    m_Deleter = MoveArg(other.m_Deleter);
    other.m_Ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
inline constexpr UniquePtr<T, DelT>&
UniquePtr<T, DelT>::operator=(UniquePtr<U, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
{
    m_Deleter(m_Ptr);
    m_Ptr = static_cast<Ptr>(other.m_Ptr);
    m_Deleter = MoveArg(other.m_Deleter);
    other.m_Ptr = nullptr;
    return *this;
}

template <NonRefT T, typename... Args>
inline constexpr UniquePtr<T> MakeUnique(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return UniquePtr<T>(new T(ForwardArg<Args>(args)...));
}

template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
inline constexpr bool operator==(UniquePtr<T, DelT> const& l, UniquePtr<U, DelU> const& r) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return l.Get() == r.Get();
}

template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
inline constexpr bool operator<=>(UniquePtr<T, DelT> const& l, UniquePtr<U, DelU> const& r) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return l.Get() <=> r.Get();
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool operator==(UniquePtr<T, DelT> const& p, NullptrT) noexcept
{
    return p.Get() == nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
inline constexpr bool operator<=>(UniquePtr<T, DelT> const& p, NullptrT) noexcept
{
    return p.Get() <=> nullptr;
}

} // namespace mini