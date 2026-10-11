export module mini.core:dynamic_buffer;

import :type;
import :utility_operation;
import :allocator;

namespace mini::memory {

template <typename T, AllocatorT<T> AllocT>
class DynamicBuffer {
protected:
    [[emptyable_address]] AllocT m_alloc;
    size_t m_capacity;
    T* m_buffer;

protected:
    constexpr DynamicBuffer(T* ptr, size_t size, AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(size)
        , m_buffer(ptr)
    {
    }

public:
    constexpr DynamicBuffer() noexcept = default;
    constexpr ~DynamicBuffer() { deallocate(); }

    constexpr DynamicBuffer(AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(0)
        , m_buffer(nullptr)
    {
    }

    constexpr DynamicBuffer(AllocT&& alloc) noexcept
        : m_alloc(moveCast(alloc))
        , m_capacity(0)
        , m_buffer(nullptr)
    {
    }

    constexpr DynamicBuffer(DynamicBuffer&& other) noexcept
        : m_alloc{}
        , m_capacity(exchange(other.m_capacity, size_t{0}))
        , m_buffer(exchange(other.m_buffer, nullptr))
    {
        mini::swap(m_alloc, other.m_alloc);
    }

    constexpr DynamicBuffer(DynamicBuffer&& other, AllocT const& alloc) noexcept
        : m_alloc(alloc)
        , m_capacity(exchange(other.m_capacity, size_t{0}))
        , m_buffer(exchange(other.m_buffer, nullptr))
    {
    }

    constexpr DynamicBuffer(size_t size) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc{}
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        allocate(size);
    }

    constexpr DynamicBuffer(size_t size, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc(alloc)
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        allocate(size);
    }

    constexpr DynamicBuffer(size_t size, AllocT&& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_alloc(moveCast(alloc))
        , m_capacity(0)
        , m_buffer(nullptr)
    {
        allocate(size);
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept { return m_capacity; }
    [[nodiscard]] constexpr T* data() noexcept { return m_buffer; }
    [[nodiscard]] constexpr T const* data() const noexcept { return m_buffer; }
    [[nodiscard]] constexpr AllocT const& allocator() const noexcept { return m_alloc; }

    constexpr void allocate(size_t size) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = m_alloc.allocate(size);
        m_buffer = buffer.pointer;
        m_capacity = buffer.capacity;
    }

    constexpr void deallocate() noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        m_alloc.deallocate(m_buffer, m_capacity);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    [[nodiscard]] constexpr DynamicBuffer increment(size_t size) const noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        size_t target = m_capacity < size ? m_capacity + size : m_capacity << 1;
        ASSERT(target != 0, "invalid capacity on buffer increment");

        AllocationResult<T> newBuffer = m_alloc.allocate(target);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_alloc);
    }

    [[nodiscard]] constexpr DynamicBuffer resize(size_t size) const noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = m_alloc.allocate(size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_alloc);
    }

    constexpr void swap(DynamicBuffer& other) noexcept
    {
        mini::swap(m_buffer, other.m_buffer);
        mini::swap(m_capacity, other.m_capacity);
        mini::swap(m_alloc, other.m_alloc);
    }

    constexpr bool operator==(DynamicBuffer const& other) const noexcept { return m_buffer == other.m_buffer; }

    constexpr DynamicBuffer& operator=(DynamicBuffer&& other) noexcept
    {
        deallocate();
        m_alloc = moveCast(other.m_alloc);
        m_buffer = mini::exchange(other.m_buffer, nullptr);
        m_capacity = mini::exchange(other.m_capacity, 0);
        return *this;
    }

    DynamicBuffer(DynamicBuffer const&) = delete;
    DynamicBuffer& operator=(DynamicBuffer const&) = delete;
};

} // namespace mini::memory