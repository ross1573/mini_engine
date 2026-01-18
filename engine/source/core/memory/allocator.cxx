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
import :numeric;
import :memory_operation;

namespace mini {

export template <typename T>
struct AllocationResult;

export template <typename T>
struct Allocator;

template <typename AllocT>
struct IsDefaultAlloc : FalseT {};

template <typename T>
struct IsDefaultAlloc<Allocator<T>> : TrueT {};

export template <typename T>
concept UnboundAllocatorT = CopyableT<T> && requires(T alloc, SizeT s, typename T::Pointer loc) {
    requires !ReferenceT<typename T::Value>;
    requires PointerT<typename T::Pointer>;
    requires PointerT<typename T::ConstPointer>;

    { alloc.Allocate(s) } -> SameAsT<AllocationResult<typename T::Value>>;
    { alloc.Increment(s, s) } -> SameAsT<AllocationResult<typename T::Value>>;
    { alloc.Deallocate(loc, s) };
};

export template <typename AllocT, typename T>
concept AllocatorT = UnboundAllocatorT<AllocT> && SameAsT<typename AllocT::Value, T>;

export template <typename AllocT, typename T>
concept NoThrowAllocatorT =
    AllocatorT<AllocT, T> && NoThrowCopyableT<AllocT> &&
    NoThrowCallableT<decltype(&AllocT::Allocate), SizeT> &&
    NoThrowCallableT<decltype(&AllocT::Increment), SizeT, SizeT> &&
    NoThrowCallableT<decltype(&AllocT::Deallocate), typename AllocT::Pointer, SizeT>;

template <typename AllocT, typename T>
concept AllocatorDecayT = AllocatorT<DecayT<AllocT>, T>;

template <typename AllocT, typename U>
concept AllocRebindDeclaredT = requires(AllocT alloc) {
    { alloc.template Rebind<U>() } -> REBIND_RESULT_T;
};

export template <typename T>
struct AllocationResult {
    T* pointer;
    SizeT capacity;
};

export template <typename T>
struct Allocator {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    [[nodiscard]] inline constexpr AllocationResult<T> Allocate(SizeT size) const noexcept
    {
        Pointer ptr = nullptr;
        if consteval {
            ptr = CONSTEXPR_ALLOC(T, size);
        } else {
            ptr = static_cast<T*>(BUILTIN_OPERATOR_NEW(size * sizeof(T) NOTHROW_T));
            VERIFY(ptr, "allocation failed. possible out-of-memory");
        }

        return { .pointer = ptr, .capacity = size };
    }

    [[nodiscard]] inline constexpr AllocationResult<T> Increment(SizeT oldCap,
                                                                 SizeT size) const noexcept
    {
        SizeT newCap = oldCap < size ? oldCap + size : oldCap << 1;
        ASSERT(newCap != 0, "invalid capacity on buffer increment");
        return Allocate(newCap);
    }

    inline constexpr void Deallocate(Pointer loc, SizeT size) const noexcept
    {
        if consteval {
            if (loc == nullptr) {
                return;
            }

            CONSTEXPR_DEALLOC(T, loc, size);
            return;
        }

        BUILTIN_OPERATOR_DELETE(memory::MakeVoidPtr(loc) NOTHROW_T);
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
    return mini::Allocator<U>{};
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
        NEVER_CALLED("dummy allocator should be rebinded");
    }

    template <typename T>
    AllocationResult<void> Increment(T&&, T&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
    }

    template <typename T, typename U>
    void Deallocate(T&&, U&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
    }
};

export template <typename U>
inline constexpr Allocator<U> RebindAllocator(UnboundAllocator)
{
    return mini::Allocator<U>{};
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

namespace std {

export template <typename ValueT, template <typename> typename T>
    requires mini::AllocatorT<T<ValueT>, ValueT>
struct allocator_traits<T<ValueT>> {
    typedef T<ValueT> AllocT;
    typedef AllocT allocator_type;
    typedef mini::SizeT size_type;
    typedef mini::OffsetT difference_type;
    typedef typename AllocT::Value value_type;
    typedef typename AllocT::Pointer pointer;
    typedef typename mini::UnboundAllocator::Pointer void_pointer;
    typedef typename mini::UnboundAllocator::ConstPointer const_void_pointer;

    static constexpr pointer allocate(AllocT& alloc, size_type n)
    {
        return alloc.Allocate(n).pointer;
    }

    static constexpr pointer allocate(AllocT& alloc, size_type n, const_void_pointer)
    {
        return alloc.Allocate(n).pointer;
    }

    static constexpr std::allocation_result<pointer, size_type> allocate_at_least(AllocT& alloc,
                                                                                  size_type n)
    {
        mini::AllocationResult<ValueT> result = alloc.Allocate(n);
        return { .ptr = result.pointer, .count = n };
    }

    static constexpr void deallocate(AllocT& alloc, pointer ptr, size_type n)
    {
        alloc.deallocate(ptr, n);
    }

    template <typename U, typename... Args>
    static constexpr void construct(AllocT&, U* ptr, Args&&... args)
    {
        mini::memory::ConstructAt(ptr, mini::ForwardArg(args)...);
    }

    template <typename U>
    static constexpr void destroy(AllocT&, U* ptr)
    {
        mini::memory::DestructAt(ptr);
    }

    static constexpr size_type max_size(AllocT const&)
    {
        return mini::NumericLimit<size_type>::max / sizeof(value_type);
    }
};

} // namespace std