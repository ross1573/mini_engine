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
    T* m_Ptr;
    [[emptyable_address]] AllocT m_Alloc;
    [[emptyable_address]] DelT m_Deleter;

public:
    template <typename AllocU, typename DelU>
    inline constexpr SharedBlock(T* ptr, AllocU&& alloc, DelU&& del) noexcept
        : SharedCounter()
        , m_Ptr(ptr)
        , m_Alloc(ForwardArg<AllocU>(alloc))
        , m_Deleter(ForwardArg<DelU>(del))
    {
    }

    inline constexpr T* Get() noexcept { return m_Ptr; }

    inline constexpr void DeletePtr() noexcept override
    {
        m_Deleter(m_Ptr);
        m_Ptr = nullptr;
    }

    inline constexpr void DeleteSharedBlock() noexcept override
    {
        auto alloc = RebindAllocator<SharedBlock>(MoveArg(m_Alloc));
        memory::DestructAt(this);
        alloc.Deallocate(this, 1);
    }

private:
    SharedBlock(SharedBlock const&) = delete;
    SharedBlock(SharedBlock&&) = delete;

    SharedBlock& operator=(SharedBlock const&) = delete;
    SharedBlock& operator=(SharedBlock&&) = delete;
};

template <typename T, typename AllocT>
class InplaceSharedBlock : public SharedCounter {
private:
    alignas(alignof(T)) byte m_Buffer[sizeof(T)];
    [[emptyable_address]] AllocT m_Alloc;

public:
    template <typename AllocU, typename... Args>
        requires ConstructibleFromT<T, Args...>
    inline InplaceSharedBlock(AllocU&& alloc, Args&&... args)
        noexcept(NoThrowConstructibleFromT<T, Args...>)
        : SharedCounter()
        , m_Alloc(ForwardArg<AllocU>(alloc))
    {
        T* ptr = reinterpret_cast<T*>(&m_Buffer[0]);
        memory::ConstructAt(ptr, ForwardArg<Args>(args)...);
    }

    inline T* Get() noexcept { return reinterpret_cast<T*>(&m_Buffer[0]); }

    inline void DeletePtr() noexcept override
    {
        T* ptr = reinterpret_cast<T*>(&m_Buffer[0]);
        memory::DestructAt(ptr);
    }

    inline void DeleteSharedBlock() noexcept override
    {
        auto alloc = RebindAllocator<InplaceSharedBlock>(MoveArg(m_Alloc));
        memory::DestructAt(this);
        alloc.Deallocate(this, 1);
    }

private:
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
    Pointer m_Ptr;
    SharedCounter* m_Counter;

public:
    constexpr SharedPtr() noexcept;
    constexpr ~SharedPtr();
    constexpr SharedPtr(SharedPtr const&) noexcept;
    constexpr SharedPtr(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr(SharedPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U>
    explicit constexpr SharedPtr(U*) noexcept;
    template <NonRefT U>
    constexpr SharedPtr(SharedPtr<U> const&, Pointer) noexcept;
    template <NonRefT U>
    constexpr SharedPtr(SharedPtr<U>&&, Pointer) noexcept;

    template <typename DelT = UnboundDeleter, typename AllocT = UnboundAllocator>
    constexpr SharedPtr(NullptrT, DelT = {}, AllocT = {}) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr SharedPtr(U*, DelT&&) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr SharedPtr(U*, DelT&&, AllocT const&)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    constexpr Pointer Get() const noexcept;
    constexpr bool Valid() const noexcept;

    constexpr void Swap(SharedPtr&) noexcept;
    constexpr void Reset() noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U*) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr void Reset(U*, DelT&&) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr void Reset(U*, DelT&&, AllocT const&)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    template <NonRefT U>
    constexpr bool Equals(SharedPtr<U> const&) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonRefT U>
    constexpr bool OwnerEquals(SharedPtr<U> const&) const noexcept;

    constexpr Pointer operator->() const noexcept;
    constexpr Reference operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Pointer() const noexcept;

    constexpr SharedPtr& operator=(NullptrT) noexcept;
    constexpr SharedPtr& operator=(SharedPtr const&) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U>&&) noexcept;

private:
    template <typename AllocT, typename DelT>
    constexpr void AllocateBlock(T*, AllocT const&, DelT&&);
    template <typename AllocT, typename... Args>
    constexpr void AllocateInplaceBlock(AllocT const&, Args&&...);

    template <NonRefT U, UnboundAllocatorT AllocT, typename... Args>
    friend constexpr SharedPtr<U> AllocateShared(AllocT const&, Args&&...)
        requires RebindableWithT<AllocT, InplaceSharedBlock<U, AllocT>> &&
                 ConstructibleFromT<U, Args...>;
};

template <NonRefT T>
inline constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_Ptr(nullptr)
    , m_Counter(nullptr)
{
}

template <NonRefT T>
inline constexpr SharedPtr<T>::~SharedPtr()
{
    if (m_Counter) {
        m_Counter->Release();
        m_Counter = nullptr;
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_Ptr(other.m_Ptr)
    , m_Counter(other.m_Counter)
{
    if (m_Counter != nullptr) {
        m_Counter->Retain();
    }
}

template <NonRefT T>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_Ptr(other.m_Ptr)
    , m_Counter(other.m_Counter)
{
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
    , m_Counter(other.m_Counter)
{
    if (m_Counter != nullptr) {
        m_Counter->Retain();
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    : m_Ptr(static_cast<T*>(other.m_Ptr))
    , m_Counter(other.m_Counter)
{
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>::SharedPtr(U* ptr) noexcept
{
    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, DefaultDeleter<T>{});
}

template <NonRefT T>
template <NonRefT U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept
    : m_Ptr(ptr)
    , m_Counter(other.m_Counter)
{
    ASSERT(m_Counter);
    m_Counter->Retain();
}

template <NonRefT T>
template <NonRefT U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept
    : m_Ptr(ptr)
    , m_Counter(other.m_Counter)
{
    other.m_Ptr = nullptr;
    other.m_Counter = nullptr;
}

template <NonRefT T>
template <typename DelT, typename AllocT>
inline constexpr SharedPtr<T>::SharedPtr(NullptrT, DelT, AllocT) noexcept
    : m_Ptr(nullptr)
    , m_Counter(nullptr)
{
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
inline constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del) noexcept
{
    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
inline constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    AllocateBlock(static_cast<T*>(ptr), alloc, ForwardArg<DelT>(del));
}

template <NonRefT T>
inline constexpr SharedPtr<T>::Pointer SharedPtr<T>::Get() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
inline constexpr bool SharedPtr<T>::Valid() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T>
inline constexpr void SharedPtr<T>::Swap(SharedPtr& o) noexcept
{
    mini::Swap(m_Ptr, o.m_Ptr);
    mini::Swap(m_Counter, o.m_Counter);
}

template <NonRefT T>
inline constexpr void SharedPtr<T>::Reset() noexcept
{
    if (m_Counter) {
        m_Counter->Release();
        m_Counter = nullptr;
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr void SharedPtr<T>::Reset(U* ptr) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, DefaultDeleter<T>{});
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
inline constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
inline constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    if (m_Counter) {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), alloc, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <NonRefT U>
inline constexpr bool SharedPtr<T>::Equals(SharedPtr<U> const& o) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_Ptr == o.m_Ptr && m_Counter == o.m_Counter;
}

template <NonRefT T>
template <NonRefT U>
inline constexpr bool SharedPtr<T>::OwnerEquals(SharedPtr<U> const& o) const noexcept
{
    return m_Counter == o.m_Counter;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::Pointer SharedPtr<T>::operator->() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::Reference SharedPtr<T>::operator*() const noexcept
{
    return *m_Ptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::operator Pointer() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(NullptrT) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    m_Ptr = nullptr;
    m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& o) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    m_Ptr = o.m_Ptr;
    m_Counter = o.m_Counter;

    if (m_Counter) {
        m_Counter->Retain();
    }

    return *this;
}

template <NonRefT T>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& o) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    m_Ptr = o.m_Ptr;
    m_Counter = o.m_Counter;
    o.m_Ptr = nullptr;
    o.m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& o) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    m_Ptr = static_cast<T*>(o.m_Ptr);
    m_Counter = o.m_Counter;

    if (m_Counter) {
        m_Counter->Retain();
    }

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& o) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    m_Ptr = static_cast<T*>(o.m_Ptr);
    m_Counter = o.m_Counter;
    o.m_Ptr = nullptr;
    o.m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
template <typename AllocT, typename DelT>
inline constexpr void SharedPtr<T>::AllocateBlock(T* ptr, AllocT const& alloc, DelT&& del)
{
    typedef SharedBlock<T, AllocT, DelT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, ptr, alloc, ForwardArg<DelT>(del));
    m_Ptr = ptr;
    m_Counter = result.pointer;
}

template <NonRefT T>
template <typename AllocT, typename... Args>
inline constexpr void SharedPtr<T>::AllocateInplaceBlock(AllocT const& alloc, Args&&... args)
{
    typedef InplaceSharedBlock<T, AllocT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, alloc, ForwardArg<Args>(args)...);
    m_Ptr = result.pointer->Get();
    m_Counter = result.pointer;
}

export template <NonRefT T, UnboundAllocatorT AllocT, typename... Args>
inline constexpr SharedPtr<T> AllocateShared(AllocT const& alloc, Args&&... args)
    requires RebindableWithT<AllocT, InplaceSharedBlock<T, AllocT>> &&
             ConstructibleFromT<T, Args...>
{
    SharedPtr<T> ret;

    if consteval {
        T* ptr = ::new T(ForwardArg<Args>(args)...);
        ret.AllocateBlock(ptr, alloc, DefaultDeleter<T>{});
    } else {
        ret.AllocateInplaceBlock(alloc, ForwardArg<Args>(args)...);
    }

    return ret;
}

export template <NonRefT T, typename... Args>
inline constexpr SharedPtr<T> MakeShared(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return AllocateShared<T, UnboundAllocator, Args...>({}, ForwardArg<Args>(args)...);
}

export template <NonRefT T, NonRefT U>
inline constexpr bool operator==(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return l.Get() == r.Get();
}

export template <NonRefT T, NonRefT U>
inline constexpr auto operator<=>(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return l.Get() <=> r.Get();
}

export template <NonRefT T>
inline constexpr bool operator==(SharedPtr<T> const s, NullptrT) noexcept
{
    return s.Get() == nullptr;
}

export template <NonRefT T>
inline constexpr auto operator<=>(SharedPtr<T> const s, NullptrT) noexcept
{
    return s.Get() <=> nullptr;
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, static_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), static_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(other, ptr) : SharedPtr<T>();
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(MoveArg(other), ptr) : SharedPtr<T>();
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, const_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.Get()));
}

export template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.Get()));
}

} // namespace mini
