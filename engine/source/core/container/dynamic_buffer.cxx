export module mini.core:dynamic_buffer;

import :type;
import :utility_operation;
import :allocator;

namespace mini::memory {

template <typename T, AllocatorT<T> AllocT>
class DynamicBuffer {
protected:
    [[emptyable_address]] AllocT m_alloc;
    SizeT m_capacity;
    T* m_buffer;

protected:
    inline constexpr DynamicBuffer(T* ptr, SizeT cap, AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(cap)
        , m_buffer(ptr)
    {
    }

public:
    inline constexpr DynamicBuffer() noexcept = default;
    inline constexpr ~DynamicBuffer() { Deallocate(); }

    inline constexpr DynamicBuffer(AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(0)
        , m_buffer(nullptr)
    {
    }

    inline constexpr DynamicBuffer(AllocT&& alloc) noexcept
        : m_alloc(MoveArg(alloc))
        , m_capacity(0)
        , m_buffer(nullptr)
    {
    }

    inline constexpr DynamicBuffer(DynamicBuffer&& other) noexcept
        : m_alloc{}
        , m_capacity(Exchange(other.m_capacity, SizeT(0)))
        , m_buffer(Exchange(other.m_buffer, nullptr))
    {
        mini::Swap(m_alloc, other.m_alloc);
    }

    inline constexpr DynamicBuffer(DynamicBuffer&& other, AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(Exchange(other.m_capacity, SizeT(0)))
        , m_buffer(Exchange(other.m_buffer, nullptr))
    {
    }

    inline constexpr DynamicBuffer(SizeT capacity) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc{}
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        Allocate(capacity);
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc(alloc)
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        Allocate(capacity);
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT&& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc(MoveArg(alloc))
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        Allocate(capacity);
    }

    inline constexpr SizeT Capacity() const noexcept { return m_capacity; }
    [[nodiscard]] inline constexpr T* Data() noexcept { return m_buffer; }
    [[nodiscard]] inline constexpr T const* Data() const noexcept { return m_buffer; }
    [[nodiscard]] inline constexpr AllocT const& GetAllocator() const noexcept { return m_alloc; }

    inline constexpr void Allocate(SizeT size) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = m_alloc.Allocate(size);
        m_buffer = buffer.pointer;
        m_capacity = buffer.capacity;
    }

    inline constexpr void Deallocate() noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        m_alloc.Deallocate(m_buffer, m_capacity);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    [[nodiscard]] inline constexpr DynamicBuffer Increment(SizeT size) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = m_alloc.Increment(m_capacity, size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_alloc);
    }

    [[nodiscard]] inline constexpr DynamicBuffer Resize(SizeT size) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = m_alloc.Allocate(size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_alloc);
    }

    inline constexpr void Swap(DynamicBuffer& other) noexcept
    {
        mini::Swap(m_buffer, other.m_buffer);
        mini::Swap(m_capacity, other.m_capacity);
        mini::Swap(m_alloc, other.m_alloc);
    }

    inline constexpr bool operator==(DynamicBuffer const& other) const noexcept
    {
        return m_buffer == other.m_buffer;
    }

    inline constexpr DynamicBuffer& operator=(DynamicBuffer&& other) noexcept
    {
        Deallocate();
        m_alloc = MoveArg(other.m_alloc);
        m_buffer = mini::Exchange(other.m_buffer, nullptr);
        m_capacity = mini::Exchange(other.m_capacity, 0);
        return *this;
    }

    DynamicBuffer(DynamicBuffer const&) = delete;
    DynamicBuffer& operator=(DynamicBuffer const&) = delete;
};

} // namespace mini::memory