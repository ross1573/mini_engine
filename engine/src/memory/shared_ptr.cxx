module;

#include <atomic>
#include "core/define.h"
#include "core/assert.h"

export module mini.memory:shared_ptr;

import mini.core;
import :common;
import :allocator;
import :deleter;
import :static_buffer;

using MemoryOrder = std::memory_order;

namespace mini
{

#ifdef MSVC
#pragma warning(push)
#pragma warning(disable: 4582) // silence warning for ctor not called
#endif

template <typename T>
struct AtomicUnion
{

    union
    {
        std::atomic<T> atomic;
        T value;
    };

    inline constexpr AtomicUnion(T val) noexcept
    {
        if (std::is_constant_evaluated()) value = val;
        else mini::memory::ConstructAt(&atomic, val);
    }

    inline constexpr ~AtomicUnion() noexcept
    {
        if (!std::is_constant_evaluated())
        {
            mini::memory::DestructAt(&atomic);
        }
    }

    inline constexpr void Store(T val, MemoryOrder order) noexcept
    {
        if (std::is_constant_evaluated()) value = val;
        else atomic.store(val, order);
    }

    inline constexpr T Load(MemoryOrder order) const noexcept
    {
        if (std::is_constant_evaluated()) return value;
        else return atomic.load(order);
    }

    inline constexpr T FetchAdd(T val, MemoryOrder order) noexcept
    {
        if (std::is_constant_evaluated())
        {
            T tmp = value;
            value += val;
            return tmp;
        }
        else return atomic.fetch_add(val, order);
    }

    inline constexpr T FetchSub(T val, MemoryOrder order) noexcept
    {
        if (std::is_constant_evaluated())
        {
            T tmp = value;
            value -= val;
            return tmp;
        }
        else return atomic.fetch_sub(val, order);
    }

    inline constexpr void ThreadFence(MemoryOrder order) const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            std::atomic_thread_fence(order);
        }
    }

    AtomicUnion(AtomicUnion const&) = delete;
    AtomicUnion& operator=(AtomicUnion const&) = delete;
};

#ifdef MSVC
#pragma warning(pop)
#endif

class SharedCounter
{
    typedef AtomicUnion<int32> Counter;

private:
    Counter m_Count;
    Counter m_Weak;

public:
    inline constexpr SharedCounter(SizeT count = 1) noexcept
        : m_Count((int32)count)
        , m_Weak((int32)count)
    {
    }

    inline constexpr SharedCounter(SizeT count, SizeT weakCount) noexcept
        : m_Count((int32)count)
        , m_Weak((int32)weakCount)
    {
    }

    constexpr virtual ~SharedCounter() = default;

    inline constexpr SizeT Count() const noexcept
    {
        return (SizeT)m_Count.Load(MemoryOrder::relaxed);
    }

    inline constexpr SizeT WeakCount() const noexcept
    {
        return (SizeT)m_Weak.Load(MemoryOrder::relaxed);
    }

    inline constexpr void Retain(SizeT count = 1) noexcept
    {
        m_Count.FetchAdd((int32)count, MemoryOrder::relaxed);
        m_Weak.FetchAdd((int32)count, MemoryOrder::relaxed);
    }

    inline constexpr void RetainWeak(SizeT count = 1) noexcept
    {
        m_Weak.FetchAdd((int32)count, MemoryOrder::relaxed);
    }

    inline constexpr void Release(SizeT count = 1) noexcept
    {
        int32 last = m_Count.FetchSub((int32)count, MemoryOrder::release);
        if (last == (int32)count) [[likely]]
        {
            m_Count.ThreadFence(MemoryOrder::acquire);
            DeletePtr();
        }
        else
        {
            ASSERT(last > (int32)count, "strong ref count is below zero");
        }

        ReleaseWeak(count);
    }

    inline constexpr void ReleaseWeak(SizeT count = 1) noexcept
    {
        int32 last = m_Weak.FetchSub((int32)count, MemoryOrder::release);
        if (last == (int32)count) [[likely]]
        {
            m_Weak.ThreadFence(MemoryOrder::acquire);
            DeleteSharedBlock();
        }
        else
        {
            ASSERT(last > (int32)count, "weak ref count is below zero");
        }
    }

protected:
    virtual void DeletePtr() noexcept = 0;
    virtual void DeleteSharedBlock() noexcept = 0;

private:
    SharedCounter(SharedCounter const&) = delete;
    SharedCounter(SharedCounter&&) = delete;

    SharedCounter& operator=(SharedCounter const&) = delete;
    SharedCounter& operator=(SharedCounter&&) = delete;
};

} // namespace mini

export namespace mini
{

template <
    typename T,
    typename AllocT,
    typename DelT
>
class SharedBlock : public SharedCounter
{
    typedef SharedCounter Base;

    T* m_Ptr;
    [[no_unique_address]] AllocT m_Alloc;
    [[no_unique_address]] DelT m_Deleter;

public:
    template <typename AllocU, typename DelU>
    inline constexpr SharedBlock(T* ptr, AllocU&& alloc, DelU&& del) noexcept
        : Base()
        , m_Ptr(ptr)
        , m_Alloc(ForwardArg<AllocU>(alloc))
        , m_Deleter(ForwardArg<DelU>(del))
    {
    }

    inline constexpr T* Get() noexcept
    {
        return m_Ptr;
    }

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

template <
    typename T,
    typename AllocT
>
class InplaceSharedBlock : public SharedCounter
{
    typedef SharedCounter Base;
    typedef StaticBuffer<T, 1> Buffer;

    Buffer m_Value;
    [[no_unique_address]] AllocT m_Alloc;

public:
    template <typename AllocU, typename... Args>
        requires ConstructibleFromT<T, Args...>
    inline constexpr InplaceSharedBlock(AllocU&& alloc, Args&&... args)
        noexcept(NoThrowConstructibleFromT<T, Args...>)
        : Base()
        , m_Alloc(ForwardArg<AllocU>(alloc))
    {
        memory::ConstructAt(m_Value.Data(), ForwardArg<Args>(args)...);
    }

    inline constexpr T* Get() noexcept
    {
        return m_Value.Data();
    }

    inline constexpr void DeletePtr() noexcept override
    {
        memory::DestructAt(m_Value.Data());
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

template <NonRefT T>
class SharedPtr
{
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
    template <PtrConvertibleToT<T> U> constexpr SharedPtr(SharedPtr<U> const&) noexcept;
    template <PtrConvertibleToT<T> U> constexpr SharedPtr(SharedPtr<U>&&) noexcept;
    template <PtrConvertibleToT<T> U> explicit constexpr SharedPtr(U*) noexcept;
    template <NonRefT U> constexpr SharedPtr(SharedPtr<U> const&, Ptr) noexcept;
    template <NonRefT U> constexpr SharedPtr(SharedPtr<U>&&, Ptr) noexcept;

    template <typename DelT = DummyDeleter, typename AllocT = DummyAllocator>
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
    friend constexpr SharedPtr<U> AllocateShared(AllocT const&, Args&&...) requires
        RebindableWithT<AllocT, InplaceSharedBlock<U, AllocT>>&& ConstructibleFromT<U, Args...>;
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
    if (m_Counter)
    {
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
    if (m_Counter != nullptr)
    {
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
    if (m_Counter != nullptr)
    {
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
    AllocateBlock(static_cast<T*>(ptr), DummyAllocator{}, DefaultDeleter<T>{});
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
    AllocateBlock(static_cast<T*>(ptr), DummyAllocator{}, ForwardArg<DelT>(del));
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
    if (m_Counter)
    {
        m_Counter->Release();
        m_Counter = nullptr;
        m_Ptr = nullptr;
    }
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr void SharedPtr<T>::Reset(U* ptr) noexcept
{
    if (m_Counter)
    {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), DummyAllocator{}, DefaultDeleter<T>{});
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT>
inline constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del) noexcept
{
    if (m_Counter)
    {
        m_Counter->Release();
    }

    AllocateBlock(static_cast<T*>(ptr), DummyAllocator{}, ForwardArg<DelT>(del));
}

template <NonRefT T>
template <PtrConvertibleToT<T> U, DeleterT<T> DelT, UnbindedAllocatorT AllocT>
inline constexpr void SharedPtr<T>::Reset(U* ptr, DelT&& del, AllocT const& alloc)
    requires RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    if (m_Counter)
    {
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
    if (m_Counter)
    {
        m_Counter->Release();
    }

    m_Ptr = nullptr;
    m_Counter = nullptr;

    return *this;
}

template <NonRefT T>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& o) noexcept
{
    if (m_Counter)
    {
        m_Counter->Release();
    }

    m_Ptr = o.m_Ptr;
    m_Counter = o.m_Counter;

    if (m_Counter)
    {
        m_Counter->Retain();
    }

    return *this;
}

template <NonRefT T>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& o) noexcept
{
    if (m_Counter)
    {
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
    if (m_Counter)
    {
        m_Counter->Release();
    }

    m_Ptr = static_cast<T*>(o.m_Ptr);
    m_Counter = o.m_Counter;

    if (m_Counter)
    {
        m_Counter->Retain();
    }

    return *this;
}

template <NonRefT T>
template <PtrConvertibleToT<T> U>
inline constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& o) noexcept
{
    if (m_Counter)
    {
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

template <NonRefT T, UnbindedAllocatorT AllocT, typename... Args>
inline constexpr SharedPtr<T> AllocateShared(AllocT const& alloc, Args&&... args)
    requires RebindableWithT<AllocT, InplaceSharedBlock<T, AllocT>>&& ConstructibleFromT<T, Args...>
{
    SharedPtr<T> ret;
    ret.AllocateInplaceBlock(alloc, ForwardArg<Args>(args)...);
    return ret;
}

template <NonRefT T, typename... Args>
inline constexpr SharedPtr<T> MakeShared(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return AllocateShared<T, DummyAllocator, Args...>({}, ForwardArg<Args>(args)...);
}

template <NonRefT T, NonRefT U>
inline constexpr auto operator==(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return l.Get() == r.Get();
}

template <NonRefT T, NonRefT U>
inline constexpr auto operator<=>(SharedPtr<T> const& l, SharedPtr<U> const& r) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return l.Get() <=> r.Get();
}

template <NonRefT T>
inline constexpr auto operator==(SharedPtr<T> const s, NullptrT) noexcept
{
    return s.Get() == nullptr;
}

template <NonRefT T>
inline constexpr auto operator<=>(SharedPtr<T> const s, NullptrT) noexcept
{
    return s.Get() <=> nullptr;
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, static_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> StaticCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), static_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U> const& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(other, ptr) : SharedPtr<T>();
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> DynamicCast(SharedPtr<U>&& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.Get());
    return ptr ? SharedPtr<T>(MoveArg(other), ptr) : SharedPtr<T>();
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, const_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ConstCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), const_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.Get()));
}

template <NonRefT T, NonRefT U>
inline constexpr SharedPtr<T> ReinterpretCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(MoveArg(other), reinterpret_cast<T*>(other.Get()));
}

} // namespace mini
