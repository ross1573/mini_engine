module;

#include "assertion.h"

export module mini.core:buffer;

import :type;
import :memory;
import :allocator;

namespace mini {

export template <typename T, AllocatorT<T> AllocT>
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
    inline constexpr DynamicBuffer() noexcept = default;
    inline constexpr ~DynamicBuffer() { Deallocate(); }

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
        : m_Alloc{}
        , m_Capacity(Exchange(other.m_Capacity, SizeT(0)))
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
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT const& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Alloc(alloc)
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
    }

    inline constexpr DynamicBuffer(SizeT capacity, AllocT&& alloc)
        noexcept(NoThrowAllocatorT<AllocT, T>)
        : m_Alloc(MoveArg(alloc))
        , m_Capacity(0)
        , m_Buffer(nullptr)
    {
        Allocate(capacity);
    }

    inline constexpr SizeT Capacity() const noexcept { return m_Capacity; }
    [[nodiscard]] inline constexpr T* Data() noexcept { return m_Buffer; }
    [[nodiscard]] inline constexpr T const* Data() const noexcept { return m_Buffer; }
    [[nodiscard]] inline constexpr AllocT const& GetAllocator() const noexcept { return m_Alloc; }

    inline constexpr void Allocate(SizeT size) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        ASSERT(m_Buffer == nullptr, "buffer should be deallocated first");
        AllocResult<T> buffer = m_Alloc.Allocate(size);
        m_Buffer = buffer.pointer;
        m_Capacity = buffer.capacity;
    }

    inline constexpr void Deallocate() noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        m_Alloc.Deallocate(m_Buffer, m_Capacity);
        m_Buffer = nullptr;
        m_Capacity = 0;
    }

    [[nodiscard]] inline constexpr DynamicBuffer Increment(SizeT size) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocResult<T> newBuffer = m_Alloc.Increment(m_Capacity, size);
        return DynamicBuffer(newBuffer.pointer, newBuffer.capacity, m_Alloc);
    }

    [[nodiscard]] inline constexpr DynamicBuffer Resize(SizeT size) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
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

    inline constexpr bool operator==(DynamicBuffer const& other) const noexcept
    {
        return m_Buffer == other.m_Buffer;
    }

    inline constexpr DynamicBuffer& operator=(DynamicBuffer&& other) noexcept
    {
        Deallocate();
        m_Alloc = MoveArg(other.m_Alloc);
        m_Buffer = mini::Exchange(other.m_Buffer, nullptr);
        m_Capacity = mini::Exchange(other.m_Capacity, 0);
        return *this;
    }

    DynamicBuffer(DynamicBuffer const&) = delete;
    DynamicBuffer& operator=(DynamicBuffer const&) = delete;
};

export template <TrivialT T>
class TrivialBuffer {
private:
    SizeT m_Capacity;
    T* m_Buffer;

private:
    inline constexpr TrivialBuffer(T* ptr, SizeT capacity)
        : m_Capacity(capacity)
        , m_Buffer(ptr)
    {
        if (IsConstantEvaluated()) {
            memory::ConstructRangeArgs(ptr, ptr + capacity);
        }
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
        AllocResult<T> buffer = alloc.Allocate(size);
        m_Buffer = buffer.pointer;
        m_Capacity = buffer.capacity;

        if (IsConstantEvaluated()) {
            memory::ConstructRangeArgs(m_Buffer, m_Buffer + m_Capacity);
        }
    }

    template <AllocatorT<T> AllocT>
    inline constexpr void Deallocate(AllocT const& alloc) noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        if (IsConstantEvaluated()) {
            memory::DestructRange(m_Buffer, m_Buffer + m_Capacity);
        }

        alloc.Deallocate(m_Buffer, m_Capacity);
        m_Buffer = nullptr;
        m_Capacity = 0;
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] inline constexpr TrivialBuffer Increment(SizeT size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocResult<T> newBuffer = alloc.Increment(m_Capacity, size);
        return TrivialBuffer(newBuffer.pointer, newBuffer.capacity);
    }

    template <AllocatorT<T> AllocT>
    [[nodiscard]] inline constexpr TrivialBuffer Resize(SizeT size, AllocT const& alloc) const
        noexcept(NoThrowAllocatorT<AllocT, T>)
    {
        AllocResult<T> newBuffer = alloc.Allocate(size);
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
    TrivialBuffer& operator=(TrivialBuffer&& other) noexcept = delete;
};

} // namespace mini