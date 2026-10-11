export module mini.core:shared_ptr;

import :type;
import :assert;
import :utility_operation;
import :memory_operation;
import :allocator;
import :deleter;
import :shared_counter;
import :ptr_view;

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
        , m_alloc(forwardCast<AllocU>(alloc))
        , m_deleter(forwardCast<DelU>(del))
    {
    }

    constexpr T* get() noexcept { return m_ptr; }

protected:
    constexpr void deletePointer() noexcept override
    {
        m_deleter(m_ptr);
        m_ptr = nullptr;
    }

    constexpr void deleteSharedBlock() noexcept override
    {
        using mini::rebind;
        auto alloc = rebind<SharedBlock>(moveCast(m_alloc));
        memory::destructAt(this);
        alloc.deallocate(this, 1);
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
        , m_alloc(forwardCast<AllocU>(alloc))
    {
        T* ptr = reinterpret_cast<T*>(&m_buffer[0]);
        memory::constructAt(ptr, forwardCast<Args>(args)...);
    }

    T* get() noexcept { return reinterpret_cast<T*>(&m_buffer[0]); }

protected:
    void deletePointer() noexcept override
    {
        T* ptr = reinterpret_cast<T*>(&m_buffer[0]);
        memory::destructAt(ptr);
    }

    void deleteSharedBlock() noexcept override
    {
        using mini::rebind;
        auto alloc = rebind<InplaceSharedBlock>(moveCast(m_alloc));
        memory::destructAt(this);
        alloc.deallocate(this, 1);
    }

public:
    InplaceSharedBlock(InplaceSharedBlock const&) = delete;
    InplaceSharedBlock(InplaceSharedBlock&&) = delete;

    InplaceSharedBlock& operator=(InplaceSharedBlock const&) = delete;
    InplaceSharedBlock& operator=(InplaceSharedBlock&&) = delete;
};

export template <NonReferenceT T>
class WeakPtr;

export template <NonReferenceT T>
class SharedPtr {
private:
    template <NonReferenceT U>
    friend class SharedPtr;
    template <NonReferenceT U>
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
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    explicit constexpr SharedPtr(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept;
    template <NonReferenceT U>
    constexpr SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept;

    template <typename DelT = UnboundDeleter, typename AllocT = UnboundAllocator>
    constexpr SharedPtr(nullptr_t, DelT deleter = {}, AllocT allocator = {}) noexcept;
    template <NonReferenceT U, DeleterT<T> DelT>
    constexpr SharedPtr(U* ptr, DelT&& deleter) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr SharedPtr(U* ptr, DelT&& deleter, AllocT const& allocator)
        requires ConvertibleToT<U*, T*> && RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    [[nodiscard]] constexpr Pointer get() const noexcept;
    [[nodiscard]] constexpr bool valid() const noexcept;

    constexpr void swap(SharedPtr& other) noexcept;
    constexpr void reset() noexcept;
    template <NonReferenceT U>
    constexpr void reset(U* ptr) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U, DeleterT<T> DelT>
    constexpr void reset(U* ptr, DelT&& deleter) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
    constexpr void reset(U* ptr, DelT&& deleter, AllocT const& allocator)
        requires ConvertibleToT<U*, T*> && RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>;

    template <NonReferenceT U>
    [[nodiscard]] constexpr bool equals(SharedPtr<U> const& other) const noexcept
        requires EqualityComparableWithT<T*, U*>;
    template <NonReferenceT U>
    [[nodiscard]] constexpr bool ownerEquals(SharedPtr<U> const& other) const noexcept;

    [[nodiscard]] constexpr Pointer operator->() const noexcept;
    [[nodiscard]] constexpr Reference operator*() const noexcept;

    template <NonReferenceT U>
    constexpr operator PtrView<U>() const noexcept
        requires ConvertibleToT<T*, U*>;

    constexpr SharedPtr& operator=(nullptr_t) noexcept;
    constexpr SharedPtr& operator=(SharedPtr const& other) noexcept;
    constexpr SharedPtr& operator=(SharedPtr&& other) noexcept;
    template <NonReferenceT U>
    constexpr SharedPtr& operator=(SharedPtr<U> const& other) noexcept
        requires ConvertibleToT<U*, T*>;
    template <NonReferenceT U>
    constexpr SharedPtr& operator=(SharedPtr<U>&& other) noexcept
        requires ConvertibleToT<U*, T*>;

private:
    template <typename AllocT, typename DelT>
    constexpr void allocateBlock(T* ptr, AllocT const& allocator, DelT&& deleter);
    template <typename AllocT, typename... Args>
    constexpr void allocateInplaceBlock(AllocT const& allocator, Args&&... args);

    template <NonReferenceT U, UnboundAllocatorT AllocT, typename... Args>
    friend constexpr SharedPtr<U> allocateShared(AllocT const&, Args&&...)
        requires RebindableWithT<AllocT, InplaceSharedBlock<U, AllocT>> && ConstructibleFromT<U, Args...>;
};

template <NonReferenceT T>
constexpr SharedPtr<T>::SharedPtr() noexcept
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonReferenceT T>
constexpr SharedPtr<T>::~SharedPtr()
{
    if (m_counter != nullptr) {
        m_counter->release();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
constexpr SharedPtr<T>::SharedPtr(SharedPtr const& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->retain();
    }
}

template <NonReferenceT T>
constexpr SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : m_ptr(other.m_ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    if (m_counter != nullptr) {
        m_counter->retain();
    }
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
    : m_ptr(static_cast<T*>(other.m_ptr))
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    allocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, DefaultDeleter<T>{});
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U> const& other, Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_counter(other.m_counter)
{
    ASSERT(m_counter, "object already released");
    m_counter->retain();
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::SharedPtr(SharedPtr<U>&& other, Pointer ptr) noexcept
    : m_ptr(ptr)
    , m_counter(other.m_counter)
{
    other.m_ptr = nullptr;
    other.m_counter = nullptr;
}

template <NonReferenceT T>
template <typename DelT, typename AllocT>
constexpr SharedPtr<T>::SharedPtr(nullptr_t, DelT /*unused*/, AllocT /*unused*/) noexcept
    : m_ptr(nullptr)
    , m_counter(nullptr)
{
}

template <NonReferenceT T>
template <NonReferenceT U, DeleterT<T> DelT>
constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del) noexcept
    requires ConvertibleToT<U*, T*>
{
    allocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, forwardCast<DelT>(del));
}

template <NonReferenceT T>
template <NonReferenceT U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
constexpr SharedPtr<T>::SharedPtr(U* ptr, DelT&& del, AllocT const& alloc)
    requires ConvertibleToT<U*, T*> && RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    allocateBlock(static_cast<T*>(ptr), alloc, forwardCast<DelT>(del));
}

template <NonReferenceT T>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::get() const noexcept
{
    return m_ptr;
}

template <NonReferenceT T>
constexpr bool SharedPtr<T>::valid() const noexcept
{
    if (m_ptr == nullptr) {
        return false;
    }

    if constexpr (ValidatableT<T>) {
        return m_ptr->valid();
    }

    return true;
}

template <NonReferenceT T>
constexpr void SharedPtr<T>::swap(SharedPtr& other) noexcept
{
    mini::swap(m_ptr, other.m_ptr);
    mini::swap(m_counter, other.m_counter);
}

template <NonReferenceT T>
constexpr void SharedPtr<T>::reset() noexcept
{
    if (m_counter != nullptr) {
        m_counter->release();
        m_counter = nullptr;
        m_ptr = nullptr;
    }
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr void SharedPtr<T>::reset(U* ptr) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter) {
        m_counter->release();
    }

    allocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, DefaultDeleter<T>{});
}

template <NonReferenceT T>
template <NonReferenceT U, DeleterT<T> DelT>
constexpr void SharedPtr<T>::reset(U* ptr, DelT&& del) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter) {
        m_counter->release();
    }

    allocateBlock(static_cast<T*>(ptr), UnboundAllocator{}, forwardCast<DelT>(del));
}

template <NonReferenceT T>
template <NonReferenceT U, DeleterT<T> DelT, UnboundAllocatorT AllocT>
constexpr void SharedPtr<T>::reset(U* ptr, DelT&& del, AllocT const& alloc)
    requires ConvertibleToT<U*, T*> && RebindableWithT<AllocT, SharedBlock<T, AllocT, DelT>>
{
    if (m_counter) {
        m_counter->release();
    }

    allocateBlock(static_cast<T*>(ptr), alloc, forwardCast<DelT>(del));
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr bool SharedPtr<T>::equals(SharedPtr<U> const& other) const noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return m_ptr == other.m_ptr && m_counter == other.m_counter;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr bool SharedPtr<T>::ownerEquals(SharedPtr<U> const& other) const noexcept
{
    return m_counter == other.m_counter;
}

template <NonReferenceT T>
constexpr SharedPtr<T>::Pointer SharedPtr<T>::operator->() const noexcept
{
    ASSERT(valid(), "deference on invalid object pointer");
    return m_ptr;
}

template <NonReferenceT T>
constexpr SharedPtr<T>::Reference SharedPtr<T>::operator*() const noexcept
{
    ASSERT(valid(), "deference on invalid object pointer");
    return *m_ptr;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>::operator PtrView<U>() const noexcept
    requires ConvertibleToT<T*, U*>
{
    return PtrView<U>(m_ptr);
}

template <NonReferenceT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(nullptr_t) noexcept
{
    if (m_counter != nullptr) {
        m_counter->release();
    }

    m_ptr = nullptr;
    m_counter = nullptr;

    return *this;
}

template <NonReferenceT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr const& other) noexcept
{
    if (this == addressof(other)) {
        return *this;
    }

    if (m_counter != nullptr) {
        m_counter->release();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->retain();
    }

    return *this;
}

template <NonReferenceT T>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept
{
    if (m_counter != nullptr) {
        m_counter->release();
    }

    m_ptr = other.m_ptr;
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> const& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter == other.m_counter) {
        return *this;
    }

    if (m_counter != nullptr) {
        m_counter->release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;

    if (m_counter != nullptr) {
        m_counter->retain();
    }

    return *this;
}

template <NonReferenceT T>
template <NonReferenceT U>
constexpr SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept
    requires ConvertibleToT<U*, T*>
{
    if (m_counter) {
        m_counter->release();
    }

    m_ptr = static_cast<T*>(other.m_ptr);
    m_counter = other.m_counter;
    other.m_ptr = nullptr;
    other.m_counter = nullptr;

    return *this;
}

template <NonReferenceT T>
template <typename AllocT, typename DelT>
constexpr void SharedPtr<T>::allocateBlock(T* ptr, AllocT const& alloc, DelT&& del)
{
    typedef SharedBlock<T, AllocT, DelT> Block;
    using mini::rebind;

    auto&& blockAlloc = rebind<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.allocate(1);

    memory::constructAt(result.pointer, ptr, alloc, forwardCast<DelT>(del));
    m_ptr = ptr;
    m_counter = result.pointer;
}

template <NonReferenceT T>
template <typename AllocT, typename... Args>
constexpr void SharedPtr<T>::allocateInplaceBlock(AllocT const& alloc, Args&&... args)
{
    typedef InplaceSharedBlock<T, AllocT> Block;
    using mini::rebind;

    auto&& blockAlloc = rebind<Block>(alloc);
    AllocationResult<Block> result = blockAlloc.allocate(1);

    memory::constructAt(result.pointer, alloc, forwardCast<Args>(args)...);
    m_ptr = result.pointer->get();
    m_counter = result.pointer;
}

export template <NonReferenceT T, UnboundAllocatorT AllocT, typename... Args>
constexpr SharedPtr<T> allocateShared(AllocT const& alloc, Args&&... args)
    requires RebindableWithT<AllocT, InplaceSharedBlock<T, AllocT>> && ConstructibleFromT<T, Args...>
{
    SharedPtr<T> ret;

    if consteval {
        T* ptr = ::new T(forwardCast<Args>(args)...);
        ret.allocateBlock(ptr, alloc, DefaultDeleter<T>{});
    } else {
        ret.allocateInplaceBlock(alloc, forwardCast<Args>(args)...);
    }

    return ret;
}

export template <NonReferenceT T, typename... Args>
constexpr SharedPtr<T> makeShared(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    return allocateShared<T, UnboundAllocator, Args...>({}, forwardCast<Args>(args)...);
}

export template <NonReferenceT T, NonReferenceT U>
constexpr bool operator==(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires EqualityComparableWithT<T*, U*>
{
    return lhs.get() == rhs.get();
}

export template <NonReferenceT T, NonReferenceT U>
constexpr auto operator<=>(SharedPtr<T> const& lhs, SharedPtr<U> const& rhs) noexcept
    requires ThreeWayComparableWithT<T*, U*>
{
    return lhs.get() <=> rhs.get();
}

export template <NonReferenceT T>
constexpr bool operator==(SharedPtr<T> const ptr, nullptr_t) noexcept
{
    return ptr.get() == nullptr;
}

export template <NonReferenceT T>
constexpr auto operator<=>(SharedPtr<T> const ptr, nullptr_t) noexcept
{
    return ptr.get() <=> nullptr;
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> staticCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, static_cast<T*>(other.get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> staticCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(moveCast(other), static_cast<T*>(other.get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> dynamicCast(SharedPtr<U> const& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.get());
    return ptr ? SharedPtr<T>(other, ptr) : SharedPtr<T>();
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> dynamicCast(SharedPtr<U>&& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.get());
    return ptr ? SharedPtr<T>(moveCast(other), ptr) : SharedPtr<T>();
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> constCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, const_cast<T*>(other.get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> constCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(moveCast(other), const_cast<T*>(other.get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> reinterpretCast(SharedPtr<U> const& other) noexcept
{
    return SharedPtr<T>(other, reinterpret_cast<T*>(other.get()));
}

export template <NonReferenceT T, NonReferenceT U>
constexpr SharedPtr<T> reinterpretCast(SharedPtr<U>&& other) noexcept
{
    return SharedPtr<T>(moveCast(other), reinterpret_cast<T*>(other.get()));
}

} // namespace mini
