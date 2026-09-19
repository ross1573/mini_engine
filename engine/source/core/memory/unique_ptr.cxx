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
    constexpr UniquePtr(UniquePtr&& other) noexcept;
    explicit constexpr UniquePtr(Pointer ptr) noexcept;
    constexpr UniquePtr(Pointer ptr, DelT const& deleter) noexcept;
    constexpr UniquePtr(Pointer ptr, DelT&& deleter) noexcept;
    constexpr UniquePtr(nullptr_t) noexcept;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    constexpr UniquePtr(UniquePtr<T, DelU>&& other) noexcept
        requires ConvertibleToT<DelU, DelT>;

    [[nodiscard]] constexpr Pointer Get() const noexcept;
    [[nodiscard]] constexpr bool Valid() const noexcept;

    [[nodiscard]] constexpr Pointer Detach() noexcept;
    constexpr void Swap(UniquePtr& other) noexcept;
    constexpr void Reset(nullptr_t) noexcept;
    constexpr void Reset(Pointer ptr = Pointer()) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U* ptr) noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Pointer() const noexcept;

    constexpr UniquePtr& operator=(nullptr_t) noexcept;
    constexpr UniquePtr& operator=(UniquePtr&& other) noexcept;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    constexpr UniquePtr& operator=(UniquePtr<U, DelU>&& other) noexcept
        requires ConvertibleToT<DelU, DelT>;

public:
    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    UniquePtr(UniquePtr<T, DelU> const&) = delete;
    UniquePtr(UniquePtr const&) = delete;

    template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
    UniquePtr& operator=(UniquePtr<U, DelU> const&) = delete;
    UniquePtr& operator=(UniquePtr const&) = delete;
};

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr() noexcept
    : m_ptr(nullptr)
    , m_deleter{ }
{
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::~UniquePtr() noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_deleter(MoveArg(other.m_deleter))
{
    other.m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_deleter{ }
{
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr, DelT const& del) noexcept
    : m_ptr(ptr)
    , m_deleter(del)
{
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr(Pointer ptr, DelT&& del) noexcept
    : m_ptr(ptr)
    , m_deleter(MoveArg(del))
{
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::UniquePtr(nullptr_t) noexcept
    : m_ptr(nullptr)
    , m_deleter{ }
{
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
constexpr UniquePtr<T, DelT>::UniquePtr(UniquePtr<T, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
    : m_ptr(static_cast<Pointer>(other.m_ptr))
    , m_deleter(MoveArg(other.m_deleter))
{
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::Get() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr bool UniquePtr<T, DelT>::Valid() const noexcept
{
    if (m_ptr == nullptr) {
        return false;
    }

    if constexpr (ValidatableT<T>) {
        return m_ptr->Valid();
    }

    return true;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::Detach() noexcept
{
    Pointer tmp = m_ptr;
    m_ptr = nullptr;
    return tmp;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr void UniquePtr<T, DelT>::Swap(UniquePtr& other) noexcept
{
    mini::Swap(m_ptr, other.m_ptr);
    mini::Swap(m_deleter, other.m_deleter);
}

template <NonRefT T, DeleterT<T> DelT>
constexpr void UniquePtr<T, DelT>::Reset(nullptr_t) noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr void UniquePtr<T, DelT>::Reset(Pointer ptr) noexcept
{
    m_deleter(m_ptr);
    m_ptr = ptr;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U>
constexpr void UniquePtr<T, DelT>::Reset(U* ptr) noexcept
{
    m_deleter(m_ptr);
    m_ptr = static_cast<Pointer>(ptr);
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::Pointer UniquePtr<T, DelT>::operator->() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::Reference UniquePtr<T, DelT>::operator*() const noexcept
{
    return *m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::operator bool() const noexcept
{
    return m_ptr != nullptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>::operator Pointer() const noexcept
{
    return m_ptr;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(nullptr_t) noexcept
{
    m_deleter(m_ptr);
    m_ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(UniquePtr&& other) noexcept
{
    m_deleter(m_ptr);
    m_ptr = other.m_ptr;
    m_deleter = MoveArg(other.m_deleter);
    other.m_ptr = nullptr;
    return *this;
}

template <NonRefT T, DeleterT<T> DelT>
template <PtrConvertibleToT<T> U, DeleterT<U> DelU>
constexpr UniquePtr<T, DelT>& UniquePtr<T, DelT>::operator=(UniquePtr<U, DelU>&& other) noexcept
    requires ConvertibleToT<DelU, DelT>
{
    m_deleter(m_ptr);
    m_ptr = static_cast<Pointer>(other.m_ptr);
    m_deleter = MoveArg(other.m_deleter);
    other.m_ptr = nullptr;
    return *this;
}

export template <NonRefT T, typename... Args>
constexpr UniquePtr<T> MakeUnique(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return UniquePtr<T>(new T(ForwardArg<Args>(args)...));
}

export template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
constexpr bool operator==(UniquePtr<T, DelT> const& lhs, UniquePtr<U, DelU> const& rhs) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return lhs.Get() == rhs.Get();
}

export template <NonRefT T, DeleterT<T> DelT, NonRefT U, DeleterT<U> DelU>
constexpr bool operator<=>(UniquePtr<T, DelT> const& lhs, UniquePtr<U, DelU> const& rhs) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return lhs.Get() <=> rhs.Get();
}

export template <NonRefT T, DeleterT<T> DelT>
constexpr bool operator==(UniquePtr<T, DelT> const& ptr, nullptr_t) noexcept
{
    return ptr.Get() == nullptr;
}

export template <NonRefT T, DeleterT<T> DelT>
constexpr bool operator<=>(UniquePtr<T, DelT> const& ptr, nullptr_t) noexcept
{
    return ptr.Get() <=> nullptr;
}

} // namespace mini