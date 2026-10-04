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
        requires ConvertibleToT<U*, T*>
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

    constexpr Pointer Get() const noexcept;
    constexpr bool Valid() const noexcept;

    constexpr void Swap(SharedPtr&) noexcept;
    void Reset() noexcept;
    template <NonReferenceT U>
    void Reset(U*) noexcept
        requires ConvertibleToT<U*, T*>;

    template <NonReferenceT U>
    constexpr bool Equals(SharedPtr<U> const&) const noexcept
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
inline constexpr SharedPtr<T>::Pointer SharedPtr<T>::Get() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr bool SharedPtr<T>::Valid() const noexcept
{
    return m_ptr != nullptr;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline constexpr void SharedPtr<T>::Swap(SharedPtr& o) noexcept
{
    Pointer tmp = m_ptr;
    m_ptr = o.m_ptr;
    o.m_ptr = tmp;
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
inline void SharedPtr<T>::Reset() noexcept
{
    if (m_ptr) {
        m_ptr->Release();
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
    requires DerivedFromT<T, IUnknown>
template <NonReferenceT U>
inline void SharedPtr<T>::Reset(U* ptr) noexcept
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
inline constexpr bool SharedPtr<T>::Equals(SharedPtr<U> const& other) const noexcept
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
SharedPtr<T> AllocateShared(AllocT const&, Args&&...) = deleted_function("COM interface should not be "
                                                                         "constructed directly");

template <NonReferenceT T, typename... Args>
    requires DerivedFromT<T, IUnknown>
SharedPtr<T> MakeShared(Args&&...) = deleted_function("COM interface should not be constructed directly");

template <NonReferenceT T, NonReferenceT U>
inline constexpr bool operator==(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown> && EqualityComparableWithT<T*, U*>
{
    return l.Get() == r.Get();
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr auto operator<=>(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown> && ThreeWayComparableWithT<T*, U*>
{
    return l.Get <=> r.Get();
}

template <NonReferenceT T>
inline constexpr auto operator==(SharedPtr<T> const s, nullptr_t) noexcept
    requires DerivedFromT<T, IUnknown>
{
    return s.Get() == nullptr;
}

template <NonReferenceT T>
inline constexpr auto operator<=>(SharedPtr<T> const s, nullptr_t) noexcept
    requires DerivedFromT<T, IUnknown>
{
    return s.Get() <=> nullptr;
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, static_cast<T*>(other.Get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other)
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(MoveArg(other), static_cast<T*>(other.Get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const& other) noexcept
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
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    SharedPtr<T> result;
    if (other == nullptr) [[unlikely]] {
        return result;
    }

    other->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&result));
    other->Reset();
    return result;
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, const_cast<T*>(other.Get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.Get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.Get()));
}

template <NonReferenceT T, NonReferenceT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
    requires DerivedFromT<T, IUnknown> && DerivedFromT<U, IUnknown>
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.Get()));
}

} // namespace mini