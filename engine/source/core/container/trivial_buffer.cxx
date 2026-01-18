export module mini.core:trivial_buffer;

import :type;
import :utility_operation;
import :memory_operation;
import :allocator;

namespace mini::memory {

template <TrivialT T>
class TrivialBuffer {
private:
    SizeT m_Capacity;
    T* m_Buffer;

private:
    inline constexpr TrivialBuffer(T* ptr, SizeT capacity)
        : m_Capacity(capacity)
        , m_Buffer(ptr)
    {
        memory::BeginLifetime(ptr, ptr + capacity);
    }

public:
    constexpr TrivialBuffer() = default;
    constexpr ~TrivialBuffer() = default;
    constexpr TrivialBuffer(TrivialBuffer const&) = default;

    inline constexpr TrivialBuffer(TrivialBuffer&& other) noexcept
        : m_Capacity(mini::Exchange(other.m_Capacity, 0))
        , m_Buffer(mini::Exchange(other.m_Buffer, nullptr))
    {
    }

    template <AllocatorT<T> AllocT>
    inline constexpr TrivialBuffer(SizeT capacity, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity, alloc);
    }

    inline constexpr SizeT Capacity() const noexcept { return m_Capacity; }
    [[nodiscard]] inline constexpr T* Data() noexcept { return m_Buffer; }
    [[nodiscard]] inline constexpr T const* Data() const noexcept { return m_Buffer; }

    template <AllocatorT<T> AllocT>
    inline constexpr void Allocate(SizeT size, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_Buffer == nullptr, "buffer should be deallocated first");
        AllocationResult<T> buffer = alloc.Allocate(size);
        m_Buffer = buffer.pointer;
        m_Capacity = buffer.capacity;
        memory::BeginLifetime(m_Buffer, m_Buffer + m_Capacity);
    }

    template <AllocatorT<T> AllocT>
    inline constexpr void Deallocate(AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        memory::EndLifetime(m_Buffer, m_Buffer + m_Capacity);
        alloc.Deallocate(m_Buffer, m_Capacity);
        m_Buffer = nullptr;
        m_Capacity = 0;
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] inline constexpr TrivialBuffer Increment(SizeT size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocationResult<T> newBuffer = alloc.Increment(m_Capacity, size);
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
        mini::Swap(m_Buffer, other.m_Buffer);
        mini::Swap(m_Capacity, other.m_Capacity);
    }

    template <AllocatorT<T> AllocT>
    inline constexpr void Assign(TrivialBuffer&& other, AllocT const& alloc) noexcept
    {
        Deallocate(alloc);
        m_Buffer = mini::Exchange(other.m_Buffer, nullptr);
        m_Capacity = mini::Exchange(other.m_Capacity, 0);
    }

    inline constexpr bool operator==(TrivialBuffer const& other) const noexcept
    {
        return m_Buffer == other.m_Buffer;
    }

    constexpr TrivialBuffer& operator=(TrivialBuffer const&) = default;
};

} // namespace mini::memory