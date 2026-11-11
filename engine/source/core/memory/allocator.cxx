module;

#include "memory.h"

export module mini.core:allocator;

import :type;
import :assert;
import :numeric;
import :memory_operation;

namespace mini {

export template <typename T>
struct AllocationResult;

export template <typename T>
struct Allocator;

export template <typename T>
concept UnboundAllocatorT = CopyableT<T> && requires(T alloc, size_t size, T::Pointer loc) {
    requires !ReferenceT<typename T::Value>;
    requires PointerT<typename T::Pointer>;
    requires PointerT<typename T::ConstPointer>;

    { alloc.allocate(size) } -> SameAsT<AllocationResult<typename T::Value>>;
    { alloc.deallocate(loc, size) };
};

export template <typename AllocT, typename T>
concept AllocatorT = UnboundAllocatorT<AllocT> && SameAsT<typename AllocT::Value, T>;

export template <typename AllocT, typename T>
concept NoThrowAllocatorT =
    AllocatorT<AllocT, T> && NoThrowCopyableT<AllocT> && NoThrowCallableT<decltype(&AllocT::allocate), size_t> &&
    NoThrowCallableT<decltype(&AllocT::deallocate), typename AllocT::Pointer, size_t>;

template <typename AllocT, typename T>
concept AllocatorDecayT = AllocatorT<DecayT<AllocT>, T>;

export template <typename T>
struct AllocationResult {
    T* pointer;
    size_t capacity;
};

export template <typename T>
struct Allocator {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    [[nodiscard]] constexpr AllocationResult<T> allocate(size_t size) const noexcept;
    constexpr void deallocate(Pointer loc, size_t size) const noexcept;
};

template <typename T>
constexpr AllocationResult<T> Allocator<T>::allocate(size_t size) const noexcept
{
    if consteval {
        Pointer ptr = CONSTEXPR_ALLOC(T, size);
        return {.pointer = ptr, .capacity = size};
    }

    size_t bytes = size * sizeof(T);
    try {
        Pointer ptr = static_cast<T*>(BUILTIN_OPERATOR_NEW(bytes));
        return {.pointer = ptr, .capacity = size};
    } catch (...) {
        ASSERT(false, "allocate of {}({}*{}) failed. possible out-of-memory", bytes, size, sizeof(T));
    }

    return {.pointer = nullptr, .capacity = size};
}

template <typename T>
constexpr void Allocator<T>::deallocate(Pointer loc, size_t size) const noexcept
{
    if consteval {
        if (loc == nullptr) {
            return;
        }

        CONSTEXPR_DEALLOC(T, loc, size);
        return;
    }

    try {
        BUILTIN_OPERATOR_DELETE(static_cast<void*>(loc));
    } catch (...) {
        ASSERT(false, "deallocate failed");
    }
}

export template <typename U, typename T>
constexpr T&& rebind(RemoveRefT<T> const& alloc)
    requires AllocatorT<T, U>
{
    return forwardCast<T>(alloc);
}

export template <typename U, typename T>
constexpr T&& rebind(RemoveRefT<T>&& alloc)
    requires AllocatorT<T, U>
{
    return forwardCast<T>(alloc);
}

export template <typename U, typename T>
constexpr mini::Allocator<U> rebind(mini::Allocator<T> const& /*unused*/)
{
    return mini::Allocator<U>{};
}

export template <typename T, typename U>
constexpr bool operator==(Allocator<T> const& /*unused*/, Allocator<U> const& /*unused*/)
{
    return true;
}

export struct UnboundAllocator {
public:
    typedef void Value;
    typedef void* Pointer;
    typedef void const* ConstPointer;

    template <typename T>
    AllocationResult<void> allocate(T&& /*unused*/) const noexcept
    {
        UNSUPPORTED("dummy allocator should be rebinded");
    }

    template <typename T, typename U>
    void deallocate(T&& /*unused*/, U&& /*unused*/) const noexcept
    {
        UNSUPPORTED("dummy allocator should be rebinded");
    }
};

export template <typename U>
constexpr Allocator<U> rebind(UnboundAllocator /*unused*/)
{
    return mini::Allocator<U>{};
}

export constexpr bool operator==(UnboundAllocator const& /*unused*/, UnboundAllocator const& /*unused*/)
{
    return true;
}

template <typename AllocT, typename U>
concept AllocRebindDeclaredT = requires(AllocT alloc) {
    { rebind<U>(alloc) } -> AllocatorDecayT<U>;
};

export template <typename AllocT, typename U>
concept RebindableWithT = UnboundAllocatorT<AllocT> && AllocRebindDeclaredT<AllocT, U>;

} // namespace mini