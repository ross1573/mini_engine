export module mini.windows:shared_ptr;

export import mini.core;

export namespace mini {

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
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
    SharedPtr(SharedPtr const&) noexcept;
    constexpr SharedPtr(SharedPtr&&) noexcept;
    template <NonReferenceT U>
    SharedPtr(SharedPtr<U> const&) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U>&&) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    explicit SharedPtr(U*) noexcept
        requires ConvertibleToT<U*, T*>;
    template <DerivedFromT<IUnknown> U>
    SharedPtr(SharedPtr<U> const&, Pointer) noexcept;
    template <DerivedFromT<IUnknown> U>
    constexpr SharedPtr(SharedPtr<U>&&, Pointer) noexcept;
    constexpr SharedPtr(nullptr_t) noexcept;

    constexpr Pointer get() const noexcept;
    constexpr bool valid() const noexcept;

    constexpr void swap(SharedPtr&) noexcept;
    void reset() noexcept;
    template <NonReferenceT U>
    void reset(U*) noexcept
        requires ConvertibleToT<U*, T*>;

    template <NonReferenceT U>
    constexpr bool equals(SharedPtr<U> const&) const noexcept
        requires DerivedFromT<U, IUnknown> && EqualityComparableWithT<T*, U*>;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    constexpr Pointer* operator&() noexcept;           // TODO: remove
    explicit constexpr operator bool() const noexcept; // TODO: remove
    constexpr operator Pointer() const noexcept;       // TODO: remove

    constexpr operator PtrView<T>() const noexcept;

    SharedPtr& operator=(SharedPtr const&) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&&) noexcept;
    constexpr SharedPtr& operator=(nullptr_t) noexcept;
    template <NonReferenceT U>
    SharedPtr& operator=(SharedPtr<U> const&) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr& operator=(SharedPtr<U>&&) noexcept
        requires ConvertibleToT<U*, T*>;
};

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_ptr(nullptr)
{
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline SharedPtr<T>::~SharedPtr() noexcept
{
    if (m_ptr) {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_ptr(other.m_ptr)
{
    if (m_ptr) {
        m_ptr->AddRef();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_ptr(other.m_ptr)
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
{
    if (m_ptr) {
        m_ptr->AddRef();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline SharedPtr<T>::SharedPtr(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(ptr))
{
    if (m_ptr) {
        m_ptr->AddRef();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <DerivedFromT<IUnknown> U>
inline SharedPtr<T>::SharedPtr(SharedPtr<U> const&, Pointer ptr) noexcept
    : m_ptr(ptr)
{
    if (m_ptr) {
        m_ptr->AddRef();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <DerivedFromT<IUnknown> U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept
    : m_ptr(ptr)
{
    other.m_ptr = nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::SharedPtr(nullptr_t) noexcept
    : m_ptr(nullptr)
{
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::Pointer SharedPtr<T>::get() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr bool SharedPtr<T>::valid() const noexcept
{
    return m_ptr != nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr void SharedPtr<T>::swap(SharedPtr& o) noexcept
{
    Pointer tmp = m_ptr;
    m_ptr = o.m_ptr;
    o.m_ptr = tmp;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline void SharedPtr<T>::reset() noexcept
{
    if (m_ptr) {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline void SharedPtr<T>::reset(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->Release();
    }

    m_ptr = static_cast<T*>(ptr);

    if (m_ptr) {
        m_ptr->AddRef();
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline constexpr bool SharedPtr<T>::equals(SharedPtr<U> const& other) const noexcept
    requires DerivedFromT<U, IUnknown> && EqualityComparableWithT<T*, U*>
{
    return m_ptr == other.m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::Pointer SharedPtr<T>::operator->() const noexcept
{
    ASSERT(m_ptr, "nullpointer deference");
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::Reference SharedPtr<T>::operator*() const noexcept
{
    ASSERT(m_ptr, "nullpointer deference");
    return *m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
constexpr SharedPtr<T>::operator PtrView<T>() const noexcept
{
    return PtrView<T>(m_ptr);
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::Pointer* SharedPtr<T>::operator&() noexcept
{
    return &m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_ptr != nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>::operator Pointer() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (m_ptr) {
        m_ptr->Release();
    }

    m_ptr = other.m_ptr;

    if (m_ptr) {
        m_ptr->AddRef();
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_ptr) {
        m_ptr->Release();
    }

    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(nullptr_t) noexcept
{
    if (m_ptr) {
        m_ptr->Release();
        m_ptr = nullptr;
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->Release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);

    if (m_ptr) {
        m_ptr->AddRef();
    }

    return *this;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_ptr) {
        m_ptr->Release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    other.m_ptr = nullptr;

    return *this;
}

template <NonReferenceT T, UnboundAllocatorT AllocT, typename... Args>
    requires DerivedFromT<T, IUnknown>
SharedPtr<T> allocateShared(AllocT const&, Args&&...) = deleted_function("COM interface should not be "
                                                                         "constructed directly");

template <NonReferenceT T, typename... Args>
    requires DerivedFromT<T, IUnknown>
SharedPtr<T> makeShared(Args&&...) = deleted_function("COM interface should not be constructed directly");

template <NonReferenceT T, NonReferenceT U>
inline constexpr bool operator==(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown> && EqualityComparableWithT<T*, U*>
{
    return l.get() == r.get();
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr auto operator<=>(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown> && ThreeWayComparableWithT<T*, U*>
{
    return l.Get <=> r.get();
}

template <NonReferenceT T>
inline constexpr auto operator==(SharedPtr<T> const s, nullptr_t) noexcept
    requires DerivedFromT<T, IUnknown>
{
    return s.get() == nullptr;
}

template <NonReferenceT T>
inline constexpr auto operator<=>(SharedPtr<T> const s, nullptr_t) noexcept
    requires DerivedFromT<T, IUnknown>
{
    return s.get() <=> nullptr;
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> staticCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, static_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> staticCast(SharedPtr<U>&& other)
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(moveCast(other), static_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> dynamicCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    SharedPtr<T> result;
    if (other == nullptr) [[unlikely]] {
        return result;
    }

    other->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&result));
    return result;
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> dynamicCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    SharedPtr<T> result;
    if (other == nullptr) [[unlikely]] {
        return result;
    }

    other->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&result));
    other->reset();
    return result;
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> constCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, const_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> constCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(moveCast(other), const_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> reinterpretCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> reinterpretCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(moveCast(other), reinterpret_cast<T*>(other.get()));
}

} // namespace mini