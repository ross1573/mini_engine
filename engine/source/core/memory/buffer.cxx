module;

#include "assertion.h"

export module mini.core:buffer;

import :type;
import :memory;
import :allocator;

export namespace mini {

template <typename T, AllocatorT<T> AllocT>
class DynamicBuffer {
protected:
    [[no_unique_address]] AllocT m_Alloc;
    SizeT m_Capacity;
    T* m_Buffer;

protected:
    inline constexpr DynamicBuffer(T* ptr, SizeT cap, AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(cap)
        , m_Buffer(ptr)
    {
    }

public:
    inline constexpr DynamicBuffer() noexcept
        : m_Alloc{}
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    inline constexpr DynamicBuffer(AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    inline constexpr DynamicBuffer(AllocT&& alloc) noexcept
        : m_Alloc(MoveArg(alloc))
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    inline constexpr DynamicBuffer(DynamicBuffer&& other) noexcept
        : m_Capacity(Exchange(other.m_Capacity, SizeT(0)))
        , m_Buffer(Exchange(other.m_Buffer, nullptr))
    {
        mini::Swap(m_Alloc, other.m_Alloc);
    }

    inline constexpr DynamicBuffer(DynamicBuffer&& other, AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(Exchange(other.m_Capacity, SizeT(0)))
        , m_Buffer(Exchange(other.m_Buffer, nullptr))
    {
    }

    inline constexpr DynamicBuffer(SizeT capacity) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Alloc{}
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Alloc(alloc)
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT&& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Alloc(MoveArg(alloc))
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    inline constexpr ~DynamicBuffer() { Deallocate(); }
    constexpr DynamicBuffer(DynamicBuffer const&) = delete;

    inline constexpr T* Data() noexcept { return m_Buffer; }
    inline constexpr T const* Data() const noexcept { return m_Buffer; }
    inline constexpr SizeT Capacity() const noexcept { return m_Capacity; }

    inline constexpr void Allocate(SizeT size) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_Buffer == nullptr, "buffer should be deallocated first");
        AllocResult<T> buffer = m_Alloc.Allocate(size);
        m_Buffer = buffer.pointer;
        m_Capacity = buffer.capacity;
    }

    inline constexpr void Deallocate() noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        if (m_Buffer == nullptr) {
            return;
        }

        m_Alloc.Deallocate(m_Buffer, m_Capacity);
        m_Buffer = nullptr;
        m_Capacity = 0;
    }

    [[nodiscard]] inline constexpr DynamicBuffer Increment(SizeT size) const noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocResult<T> newBuffer = m_Alloc.Increment(m_Capacity, size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_Alloc);
    }

    [[nodiscard]] inline constexpr DynamicBuffer Resize(SizeT size) const noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocResult<T> newBuffer = m_Alloc.Allocate(size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_Alloc);
    }

    inline constexpr void Swap(DynamicBuffer& other) noexcept
    {
        mini::Swap(m_Buffer, other.m_Buffer);
        mini::Swap(m_Capacity, other.m_Capacity);
        mini::Swap(m_Alloc, other.m_Alloc);
    }

    [[nodiscard]] inline AllocT const& GetAllocator() const noexcept { return m_Alloc; }

    inline constexpr bool operator==(DynamicBuffer const& other) const noexcept { return m_Buffer == other.m_Buffer; }

    inline constexpr DynamicBuffer& operator=(DynamicBuffer&& other) noexcept
    {
        Deallocate();
        m_Buffer = other.m_Buffer;
        m_Capacity = other.m_Capacity;
        m_Alloc = MoveArg(other.m_Alloc);
        other.m_Buffer = nullptr;
        other.m_Capacity = 0;
        return *this;
    }

    constexpr DynamicBuffer& operator=(DynamicBuffer const&) = delete;
};

} // namespace mini