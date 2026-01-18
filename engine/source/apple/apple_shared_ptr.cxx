export module mini.apple:shared_ptr;

import mini.core;

export namespace mini {

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
class SharedPtr<T> {
private:
    template <NonRefT U>
    friend class SharedPtr;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

private:
    T* m_Ptr;

public:
    constexpr SharedPtr() noexcept;
    ~SharedPtr() noexcept;
    SharedPtr(SharedPtr const&) noexcept;
    constexpr SharedPtr(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U>
    SharedPtr(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr(SharedPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U>
    explicit SharedPtr(U*) noexcept;
    template <DerivedFromT<NS::Object> U>
    SharedPtr(SharedPtr<U> const&, Ptr) noexcept;
    template <DerivedFromT<NS::Object> U>
    constexpr SharedPtr(SharedPtr<U>&&, Ptr) noexcept;
    constexpr SharedPtr(NullptrT) noexcept;

    constexpr Ptr Get() const noexcept;
    constexpr bool Valid() const noexcept;

    constexpr void Swap(SharedPtr&) noexcept;
    void Reset() noexcept;
    template <PtrConvertibleToT<T> U>
    void Reset(U*) noexcept;

    template <NonRefT U>
    constexpr bool Equals(SharedPtr<U> const&) const noexcept
        requires DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Ptr() const noexcept;

    SharedPtr& operator=(SharedPtr const&) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&&) noexcept;
    constexpr SharedPtr& operator=(NullptrT) noexcept;
    template <PtrConvertibleToT<T> U>
    SharedPtr& operator=(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U>&&) noexcept;
};

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_Ptr(nullptr)
{
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>::~SharedPtr() noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_Ptr(other.m_Ptr)
{
    if (m_Ptr) {
        m_Ptr->retain();
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_Ptr(other.m_Ptr)
{
    other.m_Ptr = nullptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
{
    if (m_Ptr) {
        m_Ptr->retain();
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
{
    other.m_Ptr = nullptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline SharedPtr<T>::SharedPtr(U* ptr) noexcept
    : m_Ptr(static_cast<T*>(ptr))
{
    if (m_Ptr) {
        m_Ptr->retain();
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <DerivedFromT<NS::Object> U>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> const&, Ptr ptr) noexcept
    : m_Ptr(ptr)
{
    if (m_Ptr) {
        m_Ptr->retain();
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <DerivedFromT<NS::Object> U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Ptr ptr) noexcept
    : m_Ptr(ptr)
{
    other.m_Ptr = nullptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::SharedPtr(NullptrT) noexcept
    : m_Ptr(nullptr)
{
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::Ptr SharedPtr<T>::Get() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr bool SharedPtr<T>::Valid() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr void SharedPtr<T>::Swap(SharedPtr& other) noexcept
{
    Ptr tmp = m_Ptr;
    m_Ptr = other.m_Ptr;
    other.m_Ptr = tmp;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline void SharedPtr<T>::Reset() noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline void SharedPtr<T>::Reset(U* ptr) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
    }

    m_Ptr = static_cast<T*>(ptr);

    if (m_Ptr) {
        m_Ptr->retain();
    }
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <NonRefT U>
inline constexpr bool SharedPtr<T>::Equals(SharedPtr<U> const& other) const noexcept
    requires DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>
{
    return m_Ptr == other.m_Ptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::Ptr SharedPtr<T>::operator->() const noexcept
{
    ASSERT(m_Ptr, "null pointer deference");
    return m_Ptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::Ref SharedPtr<T>::operator*() const noexcept
{
    ASSERT(m_Ptr, "null pointer deference");
    return *m_Ptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>::operator Ptr() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
    }

    m_Ptr = other.m_Ptr;

    if (m_Ptr) {
        m_Ptr->retain();
    }

    return *this;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
    }

    m_Ptr = other.m_Ptr;
    other.m_Ptr = nullptr;

    return *this;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);

    if (m_Ptr) {
        m_Ptr->retain();
    }

    return *this;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(NullptrT) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
        m_Ptr = nullptr;
    }

    return *this;
}

template <NonRefT T>
    requires DerivedFromT<T, NS::Object>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
{
    if (m_Ptr) {
        m_Ptr->release();
    }

    m_Ptr = static_cast<T*>(other.m_Ptr);
    other.m_Ptr = nullptr;

    return *this;
}

template <NonRefT T, UnboundAllocatorT AllocT, typename... Args>
SharedPtr<T> AllocateShared(AllocT const&, Args&&...)
    requires DerivedFromT<T, NS::Object>
{
    NEVER_CALLED("NSObject should not be constructed directly");
}

template <NonRefT T, typename... Args>
SharedPtr<T> MakeShared(Args&&...)
    requires DerivedFromT<T, NS::Object>
{
    NEVER_CALLED("NSObject should not be constructed directly");
}

template <NonRefT T>
inline constexpr SharedPtr<T> TransferShared(T* other) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return SharedPtr<T>(SharedPtr<T>{}, static_cast<T*>(other));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> TransferShared(U* other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(SharedPtr<T>{}, static_cast<T*>(other));
}

template <NonRefT T, NonRefT U>
inline constexpr bool operator==(SharedPtr<T> const& l, U* r) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> &&
             EqualityComparableWithT<T*, U*>
{
    return l.Get() == r;
}

template <NonRefT T, NonRefT U>
inline constexpr auto operator<=>(SharedPtr<T> const l, U* r) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> &&
             ThreeWayComparableWithT<T*, U*>
{
    return l.Get() <=> r;
}

template <NonRefT T>
inline constexpr bool operator==(SharedPtr<T> const s, NullptrT) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return s.Get() == nullptr;
}

template <NonRefT T>
inline constexpr auto operator<=>(SharedPtr<T> const s, NullptrT) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return s.Get() <=> nullptr;
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, static_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(MoveArg(other), static_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const&) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    // TODO: is this even possible..?
    NEVER_CALLED("dynamic cast of objective-c objects are not supported");
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&&) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    // TODO: is this even possible..?
    NEVER_CALLED("dynamic cast of objective-c objects are not supported");
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, const_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.Get()));
}

} // namespace mini