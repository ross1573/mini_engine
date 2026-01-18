export module mini.core:trivial_buffer;

import :type;
import :utility_operation;
import :memory_operation;
import :allocator;

namespace mini::memory {

template <TrivialT T>
class TrivialBuffer {
private:
    SizeT m_capacity;
    T* m_buffer;

private:
    inline constexpr TrivialBuffer(T* ptr, SizeT capacity)
        : m_capacity(capacity)
        , m_buffer(ptr)
    {
        memory::BeginLifetime(ptr, ptr + capacity);
    }

public:
    constexpr TrivialBuffer() = default;
    constexpr ~TrivialBuffer() = default;
    constexpr TrivialBuffer(TrivialBuffer const&) = default;

    inline constexpr TrivialBuffer(TrivialBuffer&& other) noexcept
        : m_capacity(mini::Exchange(other.m_capacity, 0))
        , m_buffer(mini::Exchange(other.m_buffer, nullptr))
    {
    }

    template <AllocatorT<T> AllocT>
    inline constexpr TrivialBuffer(SizeT capacity, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_capacity(0)
        , m_buffer(nullptr)
    {
        Allocate(capacity, alloc);
    }

    inline constexpr SizeT Capacity() const noexcept { return m_capacity; }
    [[nodiscard]] inline constexpr T* Data() noexcept { return m_buffer; }
    [[nodiscard]] inline constexpr T const* Data() const noexcept { return m_buffer; }

    template <AllocatorT<T> AllocT>
    inline constexpr void Allocate(SizeT size, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = alloc.Allocate(size);
        m_buffer = buffer.pointer;
        m_capacity = buffer.capacity;
        memory::BeginLifetime(m_buffer, m_buffer + m_capacity);
    }

    template <AllocatorT<T> AllocT>
    inline constexpr void Deallocate(AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        memory::EndLifetime(m_buffer, m_buffer + m_capacity);
        alloc.Deallocate(m_buffer, m_capacity);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] inline constexpr TrivialBuffer Increment(SizeT size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = alloc.Increment(m_capacity, size);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] inline constexpr TrivialBuffer Resize(SizeT size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = alloc.Allocate(size);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    inline constexpr void Swap(TrivialBuffer& other) noexcept
    {
        mini::Swap(m_buffer, other.m_buffer);
        mini::Swap(m_capacity, other.m_capacity);
    }

    template <AllocatorT<T> AllocT>
    inline constexpr void Assign(TrivialBuffer&& other, AllocT const& alloc) noexcept
    {
        Deallocate(alloc);
        m_buffer = mini::Exchange(other.m_buffer, nullptr);
        m_capacity = mini::Exchange(other.m_capacity, 0);
    }

    inline constexpr bool operator==(TrivialBuffer const& other) const noexcept
    {
        return m_buffer == other.m_buffer;
    }

    constexpr TrivialBuffer& operator=(TrivialBuffer const&) = default;
};

} // namespace mini::memory