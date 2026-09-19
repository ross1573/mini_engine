export module mini.core:shared_ptr;

import :type;
import :utility_operation;
import :memory_operation;
import :allocator;
import :deleter;
import :shared_counter;

namespace mini {

template <typename T, typename AllocT, typename DelT>
class SharedBlock : public SharedCounter {
private:
    T* m_ptr;
    [[emptyable_address]] AllocT m_alloc;
    [[emptyable_address]] DelT m_deleter;

public:
    template <typename AllocU, typename DelU>
    constexpr SharedBlock(T* ptr, AllocU&& alloc, DelU&& del) noexcept
        : SharedCounter()
        , m_ptr(ptr)
        , m_alloc(ForwardArg<AllocU>(alloc))
        , m_deleter(ForwardArg<DelU>(del))
    {
    }

    constexpr T* Get() noexcept { return m_ptr; }

protected:
    constexpr void DeletePtr() noexcept override
    {
        m_deleter(m_ptr);
        m_ptr = nullptr;
    }

    constexpr void DeleteSharedBlock() noexcept override
    {
        auto alloc = RebindAllocator<SharedBlock>(MoveArg(m_alloc));
        memory::DestructAt(this);
        alloc.Deallocate(this, 1);
    }

public:
    SharedBlock(SharedBlock const&) = delete;
    SharedBlock(SharedBlock&&) = delete;

    SharedBlock& operator=(SharedBlock const&) = delete;
    SharedBlock& operator=(SharedBlock&&) = delete;
};

template <typename T, typename AllocT>
class InplaceSharedBlock : public SharedCounter {
private:
    alignas(alignof(T)) byte m_buffer[sizeof(T)];
    [[emptyable_address]] AllocT m_alloc;

public:
    template <typename AllocU, typename... Args>
        requires ConstructibleFromT<T, Args...>
    InplaceSharedBlock(AllocU&& alloc, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
        : SharedCounter()
        , m_alloc(ForwardArg<AllocU>(alloc))
    {
        T* ptr = reinterpret_cast<T*>(&m_buffer[0]);
        memory::ConstructAt(ptr, ForwardArg<Args>(args)...);
    }

    T* Get() noexcept { return reinterpret_cast<T*>(&m_buffer[0]); }

protected:
    void DeletePtr() noexcept override
    {
        T* ptr = reinterpret_cast<T*>(&m_buffer[0]);
        memory::DestructAt(ptr);
    }

    void DeleteSharedBlock() noexcept override
    {
        auto alloc = RebindAllocator<InplaceSharedBlock>(MoveArg(m_alloc));
        memory::DestructAt(this);
        alloc.Deallocate(this, 1);
    }

public:
    InplaceSharedBlock(InplaceSharedBlock const&) = delete;
    InplaceSharedBlock(InplaceSharedBlock&&) = delete;

    InplaceSharedBlock& operator=(InplaceSharedBlock const&) = delete;
    InplaceSharedBlock& operator=(InplaceSharedBlock&&) = delete;
};

export template <NonRefT T>
class WeakPtr;

export template <NonRefT T>
class SharedPtr {
private:
    template <NonRefT U>
    friend class SharedPtr;
    template <NonRefT U>
    friend class WeakPtr;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

private:
    Pointer m_ptr;
    SharedCounter* m_counter;

public:
    constexpr SharedPtr() noexcept;
    constexpr ~SharedPtr();
    constexpr SharedPtr(SharedPtr const& other) noexcept;
    constexpr SharedPtr(SharedPtr&& other) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr(SharedPtr<U> const& other) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr(SharedPtr<U>&& other) noexcept;
    template <PtrConvertibleToT<T> U>
    explicit constexpr SharedPtr(U* ptr) noexcept;
    template <NonRefT U>
    constexpr SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept;
    template <NonRefT U>
    constexpr SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept;

    template <typename DelT = UnboundDeleter, typename AllocT = UnboundAllocator>
    constexpr SharedPtr(nullptr_t, DelT deleter = { }, AllocT allocator = { }) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr SharedPtr(U* ptr, DelT&& deleter) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr SharedPtr(U* ptr, DelT&& deleter, AllocT const& allocator)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    [[nodiscard]] constexpr Pointer Get() const noexcept;
    [[nodiscard]] constexpr bool Valid() const noexcept;

    constexpr void Swap(SharedPtr& other) noexcept;
    constexpr void Reset() noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U* ptr) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr void Reset(U* ptr, DelT&& deleter) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr void Reset(U* ptr, DelT&& deleter, AllocT const& allocator)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    template <NonRefT U>
    [[nodiscard]] constexpr bool Equals(SharedPtr<U> const& other) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonRefT U>
    [[nodiscard]] constexpr bool OwnerEquals(SharedPtr<U> const& other) const noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Pointer() const noexcept;

    constexpr SharedPtr& operator=(nullptr_t) noexcept;
    constexpr SharedPtr& operator=(SharedPtr const& other) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&& other) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U> const& other) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U>&& other) noexcept;

private:
    template <typename AllocT, typename DelT>
    constexpr void AllocateBlock(T* ptr, AllocT const& allocator, DelT&& deleter);
    template <typename AllocT, typename... Args>
    constexpr void AllocateInplaceBlock(AllocT const& allocator, Args&&... args);

    template <NonRefT U, UnboundAllocatorT AllocT, typename... Args>
    friend constexpr SharedPtr<U> AllocateShared(AllocT const&, Args&&...)
        requires RebindableWithT<AllocT, InplaceSharedBlock<U, AllocT>> && ConstructibleFromT<U, Args...>;
};

template <NonRefT T>
constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonRefT T>
constexpr SharedPtr<T>::~SharedPtr()
{
    if (m_counter != nullptr) {
        m_counter->Release();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonRefT T>
constexpr SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->Retain();
    }
}

template <NonRefT T>
constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->Retain();
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr SharedPtr<T>::SharedPtr(U* ptr) noexcept
{
    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{ }, DefaultDeleter<T>{ });
}

template <NonRefT T>
template <NonRefT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_counter(other.m_counter)
{
    ASSERT(m_counter, "object already released");
    m_counter->Retain();
}

template <NonRefT T>
template <NonRefT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonRefT T>
template <typename DelT, typename AllocT>
constexpr SharedPtr<T>::SharedPtr(nullptr_t, DelT /*unused*/, AllocT /*unused*/) noexcept
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del) noexcept
{
    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{ }, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    AllocateBlock(static_cast<T*>(ptr), alloc, ForwardArg<DelT>(del));
}

template <NonRefT T>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::Get() const noexcept
{
    return m_ptr;
}

template <NonRefT T>
constexpr bool SharedPtr<T>::Valid() const noexcept
{
    if (m_ptr == nullptr) {
        return false;
    }

    if constexpr (ValidatableT<T>) {
        return m_ptr->Valid();
    }

    return true;
}

template <NonRefT T>
constexpr void SharedPtr<T>::Swap(SharedPtr& other) noexcept
{
    mini::Swap(m_ptr, other.m_ptr);
    mini::Swap(m_counter, other.m_counter);
}

template <NonRefT T>
constexpr void SharedPtr<T>::Reset() noexcept
{
    if (m_counter != nullptr) {
        m_counter->Release();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr void SharedPtr<T>::Reset(U* ptr) noexcept
{
    if (m_counter) {
        m_counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{ }, DefaultDeleter<T>{ });
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del) noexcept
{
    if (m_counter) {
        m_counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{ }, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    if (m_counter) {
        m_counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), alloc, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <NonRefT U>
constexpr bool SharedPtr<T>::Equals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_ptr == other.m_ptr && m_counter == other.m_counter;
}

template <NonRefT T>
template <NonRefT U>
constexpr bool SharedPtr<T>::OwnerEquals(SharedPtr<U> const& other) const noexcept
{
    return m_counter == other.m_counter;
}

template <NonRefT T>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::operator->() const noexcept
{
    return m_ptr;
}

template <NonRefT T>
constexpr SharedPtr<T>::Reference SharedPtr<T>::operator*() const noexcept
{
    return *m_ptr;
}

template <NonRefT T>
constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_ptr != nullptr;
}

template <NonRefT T>
constexpr SharedPtr<T>::operator Pointer() const noexcept
{
    return m_ptr;
}

template <NonRefT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(nullptr_t) noexcept
{
    if (m_counter != nullptr) {
        m_counter->Release();
    }

    m_ptr = nullptr;
    m_counter = nullptr;

    return *this;
}

template <NonRefT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (m_counter == other.m_counter) {
        return *this;
    }

    if (m_counter != nullptr) {
        m_counter->Release();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->Retain();
    }

    return *this;
}

template <NonRefT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->Release();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
{
    if (m_counter == other.m_counter) {
        return *this;
    }

    if (m_counter != nullptr) {
        m_counter->Release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->Retain();
    }

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
{
    if (m_counter) {
        m_counter->Release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonRefT T>
template <typename AllocT, typename DelT>
constexpr void SharedPtr<T>::AllocateBlock(T* ptr, AllocT const& alloc, DelT&& del)
{
    typedef SharedBlock<T, AllocT, DelT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, ptr, alloc, ForwardArg<DelT>(del));
    m_ptr = ptr;
    m_counter = result.pointer;
}

template <NonRefT T>
template <typename AllocT, typename... Args>
constexpr void SharedPtr<T>::AllocateInplaceBlock(AllocT const& alloc, Args&&... args)
{
    typedef InplaceSharedBlock<T, AllocT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, alloc, ForwardArg<Args>(args)...);
    m_ptr = result.pointer->Get();
    m_counter = result.pointer;
}

export template <NonRefT T, UnboundAllocatorT AllocT, typename... Args>
constexpr SharedPtr<T> AllocateShared(AllocT const& alloc, Args&&... args)
    requires RebindableWithT<AllocT, InplaceSharedBlock<T, AllocT>> && ConstructibleFromT<T, Args...>
{
    SharedPtr<T> ret;

    if consteval {
        T* ptr = ::new T(ForwardArg<Args>(args)...);
        ret.AllocateBlock(ptr, alloc, DefaultDeleter<T>{ });
    } else {
        ret.AllocateInplaceBlock(alloc, ForwardArg<Args>(args)...);
    }

    return ret;
}

export template <NonRefT T, typename... Args>
constexpr SharedPtr<T> MakeShared(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return AllocateShared<T, UnboundAllocator, Args...>({ }, ForwardArg<Args>(args)...);
}

export template <NonRefT T, NonRefT U>
constexpr bool operator==(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return lhs.Get() == rhs.Get();
}

export template <NonRefT T, NonRefT U>
constexpr auto operator<=>(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return lhs.Get() <=> rhs.Get();
}

export template <NonRefT T>
constexpr bool operator==(SharedPtr<T> const ptr, nullptr_t) noexcept
{
    return ptr.Get() == nullptr;
}

export template <NonRefT T>
constexpr auto operator<=>(SharedPtr<T> const ptr, nullptr_t) noexcept
{
    return ptr.Get() <=> nullptr;
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, static_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), static_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(other, ptr) : SharedPtr<T>();
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(MoveArg(other), ptr) : SharedPtr<T>();
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, const_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.Get()));
}

} // namespace mini
