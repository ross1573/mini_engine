export module mini.utility:shared_ptr;

import mini.type;
import mini.memory;
import :shared_counter;

namespace mini {

template <typename T, typename AllocT, typename DelT>
class SharedBlock : public SharedCounter {
private:
    typedef SharedCounter Base;

    T* m_Ptr;
    [[emptyable_address]] AllocT m_Alloc;
    [[emptyable_address]] DelT m_Deleter;

public:
    template <typename AllocU, typename DelU>
    inline constexpr SharedBlock(T* ptr, AllocU&& alloc, DelU&& del) noexcept
        : Base()
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
    typedef SharedCounter Base;

    alignas(alignof(T)) byte m_Buffer[sizeof(T)];
    [[emptyable_address]] AllocT m_Alloc;

public:
    template <typename AllocU, typename... Args>
        requires ConstructibleFromT<T, Args...>
    inline constexpr InplaceSharedBlock(AllocU&& alloc, Args&&... args)
        noexcept(NoThrowConstructibleFromT<T, Args...>)
        : Base()
        , m_Alloc(ForwardArg<AllocU>(alloc))
    {
        T* ptr = reinterpret_cast<T*>(&m_Buffer[0]);
        memory::ConstructAt(ptr, ForwardArg<Args>(args)...);
    }

    inline constexpr T* Get() noexcept { return reinterpret_cast<T*>(&m_Buffer[0]); }

    inline constexpr void DeletePtr() noexcept override
    {
        T* ptr = reinterpret_cast<T*>(&m_Buffer[0]);
        memory::DestructAt(ptr);
    }

    inline constexpr void DeleteSharedBlock() noexcept override
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

template <NoThrowDefaultConstructibleT T, typename AllocT>
class InplaceSharedBlock<T, AllocT> : public SharedCounter {
private:
    typedef SharedCounter Base;

    T m_Buffer;
    [[emptyable_address]] AllocT m_Alloc;

public:
    template <typename AllocU, typename... Args>
        requires ConstructibleFromT<T, Args...>
    inline constexpr InplaceSharedBlock(AllocU&& alloc, Args&&... args)
        noexcept(NoThrowConstructibleFromT<T, Args...>)
        : Base()
        , m_Alloc(ForwardArg<AllocU>(alloc))
    {
        memory::ConstructAt(memory::AddressOf(m_Buffer), ForwardArg<Args>(args)...);
    }

    inline constexpr T* Get() noexcept { return AddressOf(m_Buffer); }

    inline constexpr void DeletePtr() noexcept override
    {
        T* ptr = AddressOf(m_Buffer);
        memory::DestructAt(ptr);
    }

    inline constexpr void DeleteSharedBlock() noexcept override
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
class SharedPtr {
private:
    typedef SharedCounter Base;

    template <NonRefT U>
    friend class SharedPtr;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;

private:
    Ptr m_Ptr;
    Base* m_Counter;

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
    constexpr SharedPtr(SharedPtr<U> const&, Ptr) noexcept;
    template <NonRefT U>
    constexpr SharedPtr(SharedPtr<U>&&, Ptr) noexcept;

    template <typename DelT = DummyDeleter, typename AllocT = UnbindedAllocator>
    constexpr SharedPtr(NullptrT, DelT = {}, AllocT = {}) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr SharedPtr(U*, DelT&&) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnbindedAllocatorT AllocT>
    constexpr SharedPtr(U*, DelT&&, AllocT const&)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    constexpr Ptr Get() const noexcept;
    constexpr bool IsValid() const noexcept;

    constexpr void Swap(SharedPtr&) noexcept;
    constexpr void Reset() noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr void Reset(U*) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
    constexpr void Reset(U*, DelT&&) noexcept;
    template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnbindedAllocatorT AllocT>
    constexpr void Reset(U*, DelT&&, AllocT const&)
        requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    template <NonRefT U>
    constexpr bool Equals(SharedPtr<U> const&) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonRefT U>
    constexpr bool OwnerEquals(SharedPtr<U> const&) const noexcept;

    constexpr Ptr operator->() const noexcept;
    constexpr Ref operator*() const noexcept;
    explicit constexpr operator bool() const noexcept;
    explicit constexpr operator Ptr() const noexcept;

    constexpr SharedPtr& operator=(NullptrT) noexcept;
    constexpr SharedPtr& operator=(SharedPtr const&) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U>
    constexpr SharedPtr& operator=(SharedPtr<U>&&) noexcept;

private:
    template <typename AllocT, typename DelT>
    constexpr void AllocateBlock(T*, AllocT&&, DelT&&);
    template <typename AllocT, typename... Args>
    constexpr void AllocateInplaceBlock(AllocT&&, Args&&...);

    template <NonRefT U, UnbindedAllocatorT AllocT, typename... Args>
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
    AllocateBlock(static_cast<T*>(ptr), UnbindedAllocator{}, DefaultDeleter<T>{});
}

template <NonRefT T>
template <NonRefT U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other, Ptr ptr) noexcept
    : m_Ptr(ptr)
    , m_Counter(other.m_Counter)
{
    ASSERT(m_Counter);
    m_Counter->Retain();
}

template <NonRefT T>
template <NonRefT U>
inline constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Ptr ptr) noexcept
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
    AllocateBlock(static_cast<T*>(ptr), UnbindedAllocator{}, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnbindedAllocatorT AllocT>
inline constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    AllocateBlock(static_cast<T*>(ptr), alloc, ForwardArg<DelT>(del));
}

template <NonRefT T>
inline constexpr SharedPtr<T>::Ptr SharedPtr<T>::Get() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
inline constexpr bool SharedPtr<T>::IsValid() const noexcept
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

    AllocateBlock(static_cast<T*>(ptr), UnbindedAllocator{}, DefaultDeleter<T>{});
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
inline constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del) noexcept
{
    if (m_Counter) {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), UnbindedAllocator{}, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnbindedAllocatorT AllocT>
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
inline constexpr SharedPtr<T>::Ptr SharedPtr<T>::operator->() const noexcept
{
    return m_Ptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::Ref SharedPtr<T>::operator*() const noexcept
{
    return *m_Ptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::operator bool() const noexcept
{
    return m_Ptr != nullptr;
}

template <NonRefT T>
inline constexpr SharedPtr<T>::operator Ptr() const noexcept
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
inline constexpr void SharedPtr<T>::AllocateBlock(T* ptr, AllocT&& alloc, DelT&& del)
{
    typedef SharedBlock<T, AllocT, DelT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, ptr, ForwardArg<AllocT>(alloc), ForwardArg<DelT>(del));
    m_Ptr = ptr;
    m_Counter = result.pointer;
}

template <NonRefT T>
template <typename AllocT, typename... Args>
inline constexpr void SharedPtr<T>::AllocateInplaceBlock(AllocT&& alloc, Args&&... args)
{
    typedef InplaceSharedBlock<T, AllocT> Block;

    auto&& blockAlloc = RebindAllocator<Block>(alloc);
    AllocResult<Block> result = blockAlloc.Allocate(1);

    memory::ConstructAt(result.pointer, ForwardArg<AllocT>(alloc), ForwardArg<Args>(args)...);
    m_Ptr = result.pointer->Get();
    m_Counter = result.pointer;
}

export template <NonRefT T, UnbindedAllocatorT AllocT, typename... Args>
inline constexpr SharedPtr<T> AllocateShared(AllocT const& alloc, Args&&... args)
    requires RebindableWithT<AllocT, InplaceSharedBlock<T, AllocT>> &&
             ConstructibleFromT<T, Args...>
{
    SharedPtr<T> ret;
    ret.AllocateInplaceBlock(alloc, ForwardArg<Args>(args)...);
    return ret;
}

export template <NonRefT T, typename... Args>
inline constexpr SharedPtr<T> MakeShared(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return AllocateShared<T, UnbindedAllocator, Args...>({}, ForwardArg<Args>(args)...);
}

export template <NonRefT T, NonRefT U>
inline constexpr auto operator==(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
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
inline constexpr auto operator==(SharedPtr<T> const s, NullptrT) noexcept
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
