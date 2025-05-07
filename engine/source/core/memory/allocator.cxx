module;

#include <memory>

#include "assertion.h"

// TODO: MSVC throws an error when compiling with nothrow tag
#if MSVC // clang-format off
#  define NOTHROW_T
#else
#  define NOTHROW_T ,std::nothrow_t{}
#endif // clang-format on

// TODO: clang complains about recursion, while others dont
#if CLANG
template <typename T>
concept AlwaysTrue = true;
#  define REBIND_RESULT_T AlwaysTrue
#else
#  define REBIND_RESULT_T UnbindedAllocatorT
#endif

// TODO: only compiler can do constexpr (de)allocate
#define CONSTEXPR_ALLOC(type, x)         std::allocator<type>{}.allocate(x)
#define CONSTEXPR_DEALLOC(type, x, size) std::allocator<type>{}.deallocate(x, size)

export module mini.core:allocator;

import :type;
import :utility;
import :memory;

namespace mini {

export template <typename T>
struct AllocResult;

export template <typename T>
struct Allocator;

template <typename AllocT>
struct IsDefaultAlloc : FalseT {};

template <typename T>
struct IsDefaultAlloc<mini::Allocator<T>> : TrueT {};

export template <typename T>
concept UnbindedAllocatorT = CopyableT<T> && requires(T alloc, SizeT s, typename T::Ptr loc) {
    requires !RefT<typename T::Value>;
    requires PtrT<typename T::Ptr>;

    { alloc.Allocate(s) } -> SameAsT<AllocResult<typename T::Value>>;
    { alloc.Increment(s, s) } -> SameAsT<AllocResult<typename T::Value>>;
    { alloc.Deallocate(loc, s) };
};

export template <typename AllocT, typename T>
concept AllocatorT = UnbindedAllocatorT<AllocT> && SameAsT<typename AllocT::Value, T>;

export template <typename AllocT, typename T>
concept NoThrowAllocatorT =
    AllocatorT<AllocT, T> && NoThrowCopyableT<AllocT> &&
    NoThrowCallableT<decltype(&AllocT::Allocate), SizeT> &&
    NoThrowCallableT<decltype(&AllocT::Increment), SizeT, SizeT> &&
    NoThrowCallableT<decltype(&AllocT::Deallocate), typename AllocT::Ptr, SizeT>;

template <typename AllocT, typename T>
concept AllocatorDecayT = AllocatorT<DecayT<AllocT>, T>;

template <typename AllocT, typename U>
concept AllocRebindDeclaredT = requires(AllocT alloc) {
    { alloc.template Rebind<U>() } -> REBIND_RESULT_T;
};

export template <typename T>
struct AllocResult {
    T* pointer;
    SizeT capacity;
};

export template <typename T>
struct Allocator {
    typedef T Value;
    typedef T* Ptr;

    [[nodiscard]] inline constexpr AllocResult<T> Allocate(SizeT size) const noexcept
    {
        Ptr ptr = nullptr;
        if (IsConstantEvaluated()) {
            ptr = CONSTEXPR_ALLOC(T, size);
        }
        else {
            ptr = static_cast<T*>(::operator new(size * sizeof(T) NOTHROW_T));
            VERIFY(ptr, "allocation failed. possible out-of-memory");
        }

        return { .pointer = ptr, .capacity = size };
    }

    [[nodiscard]] inline constexpr AllocResult<T> Increment(SizeT oldCap, SizeT size) const noexcept
    {
        SizeT newCap = oldCap < size ? oldCap + size : oldCap << 1;
        ASSERT(newCap != 0, "invalid capacity on buffer increment");
        return Allocate(newCap);
    }

    inline constexpr void Deallocate(Ptr loc, SizeT size) const noexcept
    {
        if (IsConstantEvaluated()) {
            if (loc == nullptr) {
                return;
            }

            CONSTEXPR_DEALLOC(T, loc, size);
            return;
        }

        ::operator delete(MakeVoidPtr(loc) NOTHROW_T);
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

export struct DummyAllocator {
    typedef void Value;
    typedef void* Ptr;

    template <typename T>
    AllocResult<void> Allocate(T&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
    }

    template <typename T>
    AllocResult<void> Increment(T&&, T&&) const noexcept
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
inline constexpr Allocator<U> RebindAllocator(DummyAllocator)
{
    return mini::Allocator<U>{};
}

export inline constexpr bool operator==(DummyAllocator const&, DummyAllocator const&)
{
    return true;
}

template <typename AllocT, typename U>
concept AllocRebindOverloadedT = requires(AllocT alloc) {
    { RebindAllocator<U>(alloc) } -> AllocatorDecayT<U>;
};

export template <typename AllocT, typename U>
concept RebindableWithT = UnbindedAllocatorT<AllocT> &&
                          (AllocRebindDeclaredT<AllocT, U> || AllocRebindOverloadedT<AllocT, U>);

} // namespace mini
