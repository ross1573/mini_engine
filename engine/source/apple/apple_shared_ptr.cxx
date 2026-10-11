export module mini.apple:shared_ptr;

export import mini.core;
import :foundation;

export namespace mini {

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
class SharedPtr<T> {
private:
    template <NonReferenceT U>
    friend class SharedPtr;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

private:
    T* m_ptr;

public:
    constexpr SharedPtr() noexcept;
    ~SharedPtr() noexcept;
    SharedPtr(SharedPtr const& other) noexcept;
    constexpr SharedPtr(SharedPtr&& other) noexcept;
    template <NonReferenceT U>
    SharedPtr(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    explicit SharedPtr(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;
    template <DerivedFromT<NS::Object> U>
    SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept;
    template <DerivedFromT<NS::Object> U>
    constexpr SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept;
    constexpr SharedPtr(nullptr_t) noexcept;

    constexpr Pointer get() const noexcept;
    constexpr bool valid() const noexcept;

    constexpr void swap(SharedPtr& other) noexcept;
    void reset() noexcept;
    template <NonReferenceT U>
    void reset(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;

    template <NonReferenceT U>
    constexpr bool equals(SharedPtr<U> const& other) const noexcept
        requires DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;

    constexpr operator PtrView<T>() const noexcept;

    SharedPtr& operator=(SharedPtr const& other) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&& other) noexcept;
    constexpr SharedPtr& operator=(nullptr_t) noexcept;
    template <NonReferenceT U>
    SharedPtr& operator=(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr& operator=(SharedPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;
};

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_ptr(nullptr)
{
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>::~SharedPtr() noexcept
{
    if (m_ptr) {
        m_ptr->release();
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_ptr(other.m_ptr)
{
    if (m_ptr) {
        m_ptr->retain();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
{
    if (m_ptr) {
        m_ptr->retain();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
inline SharedPtr<T>::SharedPtr(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(ptr))
{
    if (m_ptr) {
        m_ptr->retain();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <DerivedFromT<NS::Object> U>
inline SharedPtr<T>::SharedPtr([[maybe_unused]] SharedPtr<U> const& other, Pointer ptr) noexcept
    : m_ptr(ptr)
{
    if (m_ptr) {
        m_ptr->retain();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <DerivedFromT<NS::Object> U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept
    : m_ptr(ptr)
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::SharedPtr(nullptr_t) noexcept
    : m_ptr(nullptr)
{
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::get() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr bool SharedPtr<T>::valid() const noexcept
{
    return m_ptr != nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr void SharedPtr<T>::swap(SharedPtr& other) noexcept
{
    Pointer tmp = m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = tmp;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
inline void SharedPtr<T>::reset() noexcept
{
    if (m_ptr) {
        m_ptr->release();
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
inline void SharedPtr<T>::reset(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->release();
    }

    m_ptr = static_cast<T*>(ptr);

    if (m_ptr) {
        m_ptr->retain();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
constexpr bool SharedPtr<T>::equals(SharedPtr<U> const& other) const noexcept
    requires DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>
{
    return m_ptr == other.m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::operator->() const noexcept
{
    ASSERT(m_ptr, "null pointer deference");
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::Reference SharedPtr<T>::operator*() const noexcept
{
    ASSERT(m_ptr, "null pointer deference");
    return *m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>::operator PtrView<T>() const noexcept
{
    return PtrView<T>(m_ptr);
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (m_ptr) {
        m_ptr->release();
    }

    m_ptr = other.m_ptr;

    if (m_ptr) {
        m_ptr->retain();
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_ptr) {
        m_ptr->release();
    }

    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);

    if (m_ptr) {
        m_ptr->retain();
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(nullptr_t) noexcept
{
    if (m_ptr) {
        m_ptr->release();
        m_ptr = nullptr;
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, NS::Object>
template <NonReferenceT U>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    other.m_ptr = nullptr;

    return *this;
}

template <NonReferenceT T, UnboundAllocatorT AllocT, typename... Args>
    requires DerivedFromT<T, NS::Object>
SharedPtr<T> allocateShared(AllocT const&, Args&&...) = deleted_function("NSObject should not be constructed directly");

template <NonReferenceT T, typename... Args>
    requires DerivedFromT<T, NS::Object>
SharedPtr<T> makeShared(Args&&...) = deleted_function("NSObject should not be constructed directly");

template <NonReferenceT T>
constexpr SharedPtr<T> transfer(T* other) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return SharedPtr<T>(SharedPtr<T>{}, other);
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> transfer(U* other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(SharedPtr<T>{}, static_cast<T*>(other));
}

template <NonReferenceT T, NonReferenceT U>
constexpr bool operator==(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>
{
    return lhs.get() == rhs.Get();
}

template <NonReferenceT T, NonReferenceT U>
constexpr bool operator==(SharedPtr<T> const& lhs, U* rhs) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>
{
    return lhs.get() == rhs;
}

template <NonReferenceT T, NonReferenceT U>
constexpr bool operator<=>(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> && EqualityComparableWithT<T*, U*>
{
    return lhs.get() <=> rhs.Get();
}

template <NonReferenceT T, NonReferenceT U>
constexpr auto operator<=>(SharedPtr<T> const lhs, U* rhs) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object> && ThreeWayComparableWithT<T*, U*>
{
    return lhs.get() <=> rhs;
}

template <NonReferenceT T>
constexpr bool operator==(SharedPtr<T> const lhs, nullptr_t) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return lhs.get() == nullptr;
}

template <NonReferenceT T>
constexpr auto operator<=>(SharedPtr<T> const lhs, nullptr_t) noexcept
    requires DerivedFromT<T, NS::Object>
{
    return lhs.get() <=> nullptr;
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> staticCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, static_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> staticCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(moveCast(other), static_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
constexpr SharedPtr<T> dynamicCast(SharedPtr<U> const&) noexcept = deleted_function("dynamic cast of "
                                                                                    "objective-c "
                                                                                    "objects are not supported");

template <NonReferenceT T, NonReferenceT U>
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
constexpr SharedPtr<T> dynamicCast(SharedPtr<U>&&) noexcept = deleted_function("dynamic cast of objective-c "
                                                                               "objects are not supported");

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> constCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, const_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> constCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> reinterpretCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> reinterpretCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, NS::Object> && DerivedFromT<U, NS::Object>
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.get()));
}

} // namespace mini