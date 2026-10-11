export module mini.core:trivial_buffer;

import :type;
import :utility_operation;
import :memory_operation;
import :allocator;

namespace mini::memory {

template <TrivialT T>
class TrivialBuffer {
private:
    size_t m_capacity;
    T* m_buffer;

private:
    constexpr TrivialBuffer(T* ptr, size_t size)
        : m_capacity(size)
        , m_buffer(ptr)
    {
        memory::beginLifetime(ptr, ptr + size);
    }

public:
    constexpr TrivialBuffer() = default;
    constexpr ~TrivialBuffer() = default;
    constexpr TrivialBuffer(TrivialBuffer const&) = default;

    constexpr TrivialBuffer(TrivialBuffer&& other) noexcept
        : m_capacity(mini::exchange(other.m_capacity, 0))
        , m_buffer(mini::exchange(other.m_buffer, nullptr))
    {
    }

    template <AllocatorT<T> AllocT>
    constexpr TrivialBuffer(size_t size, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_capacity(0)
        , m_buffer(nullptr)
    {
        allocate(size, alloc);
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept { return m_capacity; }
    [[nodiscard]] constexpr T* data() noexcept { return m_buffer; }
    [[nodiscard]] constexpr T const* data() const noexcept { return m_buffer; }

    template <AllocatorT<T> AllocT>
    constexpr void allocate(size_t size, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = alloc.allocate(size);
        m_buffer = buffer.pointer;
        m_capacity = buffer.capacity;
        memory::beginLifetime(m_buffer, m_buffer + m_capacity);
    }

    template <AllocatorT<T> AllocT>
    constexpr void deallocate(AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        alloc.deallocate(m_buffer, m_capacity);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] constexpr TrivialBuffer increment(size_t size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        size_t newCapacity = m_capacity < size ? m_capacity + size : m_capacity << 1;
        ASSERT(newCapacity != 0, "invalid capacity on buffer increment");

        AllocationResult<T> newBuffer = alloc.allocate(newCapacity);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] constexpr TrivialBuffer resize(size_t size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = alloc.allocate(size);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    constexpr void swap(TrivialBuffer& other) noexcept
    {
        mini::swap(m_buffer, other.m_buffer);
        mini::swap(m_capacity, other.m_capacity);
    }

    template <AllocatorT<T> AllocT>
    constexpr void assign(TrivialBuffer&& other, AllocT const& alloc) noexcept
    {
        deallocate(alloc);
        m_buffer = mini::exchange(other.m_buffer, nullptr);
        m_capacity = mini::exchange(other.m_capacity, 0);
    }

    constexpr bool operator==(TrivialBuffer const& other) const noexcept { return m_buffer == other.m_buffer; }

    constexpr TrivialBuffer& operator=(TrivialBuffer const&) = default;
};

} // namespace mini::memory