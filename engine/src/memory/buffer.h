#pragma once

#include "core/utility.h"
#include "memory/allocator.h"

namespace mini
{

template <
    typename T,
    AllocatorT<T> AllocT
>
class DynamicBuffer
{
protected:
    [[no_unique_address]] AllocT m_Alloc;
    SizeT m_Capacity;
    T* m_Buffer;

protected:
    [[force_inline]] constexpr DynamicBuffer(T* ptr, SizeT cap, AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(cap)
        , m_Buffer(ptr)
    {
    }

public:
    [[force_inline]] constexpr DynamicBuffer() noexcept
        : m_Alloc{}
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    [[force_inline]] constexpr DynamicBuffer(AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    [[force_inline]] constexpr DynamicBuffer(AllocT&& alloc) noexcept
        : m_Alloc(MoveArg(alloc))
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
    }

    constexpr DynamicBuffer(DynamicBuffer&& other) noexcept
        : m_Capacity(Exchange(other.m_Capacity, SizeT(0)))
        , m_Buffer(Exchange(other.m_Buffer, nullptr))
    {
        mini::Swap(m_Alloc, other.m_Alloc);
    }

    constexpr DynamicBuffer(DynamicBuffer&& other, AllocT const& alloc) noexcept
        : m_Alloc(alloc)
        , m_Capacity(Exchange(other.m_Capacity, SizeT(0)))
        , m_Buffer(Exchange(other.m_Buffer, nullptr))
    {
    }

    constexpr DynamicBuffer(SizeT capacity)
        : m_Alloc{}
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    constexpr DynamicBuffer(SizeT capacity, AllocT const& alloc)
        : m_Alloc(alloc)
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    constexpr DynamicBuffer(SizeT capacity, AllocT&& alloc)
        : m_Alloc(MoveArg(alloc))
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
        m_Capacity = capacity;
    }

    constexpr ~DynamicBuffer() { Deallocate(); }
    constexpr DynamicBuffer(DynamicBuffer const&) = delete;

    [[force_inline]] constexpr T* Data() noexcept { return m_Buffer; }
    [[force_inline]] constexpr T const* Data() const noexcept { return m_Buffer; }
    [[force_inline]] constexpr SizeT Capacity() const noexcept { return m_Capacity; }

    constexpr void Allocate(SizeT size)
    {
        ASSERT(m_Buffer == nullptr, "buffer should be deallocated first");
        AllocResult<T> buffer = m_Alloc.Allocate(size);
        m_Buffer = buffer.pointer;
        m_Capacity = buffer.capacity;
    }

    constexpr void Deallocate()
    {
        if (m_Buffer == nullptr)
        {
            return;
        }

        m_Alloc.Deallocate(m_Buffer, m_Capacity);
        m_Buffer = nullptr;
        m_Capacity = 0;
    }

    [[nodiscard]] constexpr DynamicBuffer Increment(SizeT size) const
    {
        AllocResult<T> newBuffer = m_Alloc.Increment(m_Capacity, size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_Alloc);
    }

    [[nodiscard]] constexpr DynamicBuffer Resize(SizeT size) const
    {
        AllocResult<T> newBuffer = m_Alloc.Allocate(size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_Alloc);
    }

    [[force_inline]] constexpr void Swap(DynamicBuffer& other) noexcept
    {
        mini::Swap(m_Buffer, other.m_Buffer);
        mini::Swap(m_Capacity, other.m_Capacity);
        mini::Swap(m_Alloc, other.m_Alloc);
    }

    [[nodiscard, force_inline]] AllocT const& GetAllocator() const
    {
        return m_Alloc;
    }

    [[force_inline]] constexpr bool operator==(DynamicBuffer const& other) const noexcept
    {
        return m_Buffer == other.m_Buffer;
    }

    constexpr DynamicBuffer& operator=(DynamicBuffer&& other) noexcept
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