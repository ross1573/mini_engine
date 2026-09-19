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
    constexpr TrivialBuffer(T* ptr, size_t capacity)
        : m_capacity(capacity)
        , m_buffer(ptr)
    {
        memory::BeginLifetime(ptr, ptr + capacity);
    }

public:
    constexpr TrivialBuffer() = default;
    constexpr ~TrivialBuffer() = default;
    constexpr TrivialBuffer(TrivialBuffer const&) = default;

    constexpr TrivialBuffer(TrivialBuffer&& other) noexcept
        : m_capacity(mini::Exchange(other.m_capacity, 0))
        , m_buffer(mini::Exchange(other.m_buffer, nullptr))
    {
    }

    template <AllocatorT<T> AllocT>
    constexpr TrivialBuffer(size_t capacity, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_capacity(0)
        , m_buffer(nullptr)
    {
        Allocate(capacity, alloc);
    }

    [[nodiscard]] constexpr size_t Capacity() const noexcept { return m_capacity; }
    [[nodiscard]] constexpr T* Data() noexcept { return m_buffer; }
    [[nodiscard]] constexpr T const* Data() const noexcept { return m_buffer; }

    template <AllocatorT<T> AllocT>
    constexpr void Allocate(size_t size, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = alloc.Allocate(size);
        m_buffer = buffer.pointer;
        m_capacity = buffer.capacity;
        memory::BeginLifetime(m_buffer, m_buffer + m_capacity);
    }

    template <AllocatorT<T> AllocT>
    constexpr void Deallocate(AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        alloc.Deallocate(m_buffer, m_capacity);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] constexpr TrivialBuffer Increment(size_t size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        size_t capacity = m_capacity < size ? m_capacity + size : m_capacity << 1;
        ASSERT(capacity != 0, "invalid capacity on buffer increment");

        AllocationResult<T> newBuffer = alloc.Allocate(capacity);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] constexpr TrivialBuffer Resize(size_t size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = alloc.Allocate(size);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    constexpr void Swap(TrivialBuffer& other) noexcept
    {
        mini::Swap(m_buffer, other.m_buffer);
        mini::Swap(m_capacity, other.m_capacity);
    }

    template <AllocatorT<T> AllocT>
    constexpr void Assign(TrivialBuffer&& other, AllocT const& alloc) noexcept
    {
        Deallocate(alloc);
        m_buffer = mini::Exchange(other.m_buffer, nullptr);
        m_capacity = mini::Exchange(other.m_capacity, 0);
    }

    constexpr bool operator==(TrivialBuffer const& other) const noexcept { return m_buffer == other.m_buffer; }

    constexpr TrivialBuffer& operator=(TrivialBuffer const&) = default;
};

} // namespace mini::memory