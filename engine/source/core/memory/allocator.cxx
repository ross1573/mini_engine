module;

#include "memory.h"

// TODO: clang complains about recursion, while others dont
#if CLANG
template <typename T>
concept AlwaysTrue = true;
#  define REBIND_RESULT_T AlwaysTrue
#else
#  define REBIND_RESULT_T UnboundAllocatorT
#endif

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

template <typename AllocT>
struct IsDefaultAlloc : FalseT { };

template <typename T>
struct IsDefaultAlloc<Allocator<T>> : TrueT { };

export template <typename T>
concept UnboundAllocatorT = CopyableT<T> && requires(T alloc, size_t s, typename T::Pointer loc) {
    requires !ReferenceT<typename T::Value>;
    requires PointerT<typename T::Pointer>;
    requires PointerT<typename T::ConstPointer>;

    { alloc.Allocate(s) } -> SameAsT<AllocationResult<typename T::Value>>;
    { alloc.Deallocate(loc, s) };
};

export template <typename AllocT, typename T>
concept AllocatorT = UnboundAllocatorT<AllocT> && SameAsT<typename AllocT::Value, T>;

export template <typename AllocT, typename T>
concept NoThrowAllocatorT = AllocatorT<AllocT, T> && NoThrowCopyableT<AllocT> &&
                            NoThrowCallableT<decltype(&AllocT::Allocate), size_t> &&
                            NoThrowCallableT<decltype(&AllocT::Deallocate), typename AllocT::Pointer, size_t>;

template <typename AllocT, typename T>
concept AllocatorDecayT = AllocatorT<DecayT<AllocT>, T>;

template <typename AllocT, typename U>
concept AllocRebindDeclaredT = requires(AllocT alloc) {
    { alloc.template Rebind<U>() } -> REBIND_RESULT_T;
};

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

    [[nodiscard]] inline constexpr AllocationResult<T> Allocate(size_t size) const noexcept
    {
        if consteval {
            Pointer ptr = CONSTEXPR_ALLOC(T, size);
            return { .pointer = ptr, .capacity = size };
        }

        size_t bytes = size * sizeof(T);
        try {
            Pointer ptr = static_cast<T*>(BUILTIN_OPERATOR_NEW(bytes));
            return { .pointer = ptr, .capacity = size };
        } catch (...) {
            ASSERT(false, "allocate of {}({}*{}) failed. possible out-of-memory", bytes, size, sizeof(T));
        }

        return { .pointer = nullptr, .capacity = size };
    }

    inline constexpr void Deallocate(Pointer loc, size_t size) const noexcept
    {
        if consteval {
            if (loc == nullptr) {
                return;
            }

            CONSTEXPR_DEALLOC(T, loc, size);
            return;
        }

        try {
            BUILTIN_OPERATOR_DELETE(memory::MakeVoidPtr(loc));
        } catch (...) {
            ASSERT(false, "deallocate failed");
        }
    }
};

export template <typename U, typename T>
inline constexpr decltype(auto) RebindAllocator(T const& alloc)
    requires AllocRebindDeclaredT<T, U>
{
    return alloc.template Rebind<U>();
}

export template <typename U, typename T>
inline constexpr T&& RebindAllocator(T&& alloc)
    requires AllocatorT<T, U>
{
    return ForwardArg<T>(alloc);
}

export template <typename U, typename T>
inline constexpr mini::Allocator<U> RebindAllocator(T const&)
    requires IsDefaultAlloc<T>::value
{
    return mini::Allocator<U>{ };
}

export template <typename T, typename U>
inline constexpr bool operator==(Allocator<T> const&, Allocator<U> const&)
{
    return true;
}

export struct UnboundAllocator {
public:
    typedef void Value;
    typedef void* Pointer;
    typedef void const* ConstPointer;

    template <typename T>
    AllocationResult<void> Allocate(T&&) const noexcept
    {
        UNSUPPORTED("dummy allocator should be rebinded");
    }

    template <typename T, typename U>
    void Deallocate(T&&, U&&) const noexcept
    {
        UNSUPPORTED("dummy allocator should be rebinded");
    }
};

export template <typename U>
inline constexpr Allocator<U> RebindAllocator(UnboundAllocator)
{
    return mini::Allocator<U>{ };
}

export inline constexpr bool operator==(UnboundAllocator const&, UnboundAllocator const&)
{
    return true;
}

template <typename AllocT, typename U>
concept AllocRebindOverloadedT = requires(AllocT alloc) {
    { RebindAllocator<U>(alloc) } -> AllocatorDecayT<U>;
};

export template <typename AllocT, typename U>
concept RebindableWithT = UnboundAllocatorT<AllocT> &&
                          (AllocRebindDeclaredT<AllocT, U> || AllocRebindOverloadedT<AllocT, U>);

} // namespace mini