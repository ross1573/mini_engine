module;

#include <memory>

#include "assertion.h"

export module mini.core:allocator;

import :type;
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
#ifndef CLANG
    { alloc.template Rebind<U>() } -> UnbindedAllocatorT;
#else
    alloc.template Rebind<U>(); // TODO: clang complains about recursion, while others dont
#endif
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
        if (std::is_constant_evaluated()) {
            // TODO: only compiler can do constexpr allocate
            ptr = std::allocator<T>{}.allocate(size);
        }
        else {
            // ptr = static_cast<T*>(::operator new(size * sizeof(T), std::nothrow_t{}));
            ptr = static_cast<T*>(::operator new(size * sizeof(T)));
            VERIFY(ptr, "allocation failed. possible out-of-memory");
        }

        return { .pointer = ptr, .capacity = size };
    }

    [[nodiscard]] inline constexpr AllocResult<T> Increment(SizeT oldCap, SizeT size) const noexcept
    {
        SizeT newCap = oldCap < size ? oldCap + size : oldCap << 1;
        CONSTEXPR_ASSERT(newCap != 0, "invalid capacity on buffer increment");
        return Allocate(newCap);
    }

    inline constexpr void Deallocate(Ptr loc, SizeT size) const noexcept
    {
        if (std::is_constant_evaluated()) {
            // TODO: only compiler can do constexpr deallocate
            std::allocator<T>{}.deallocate(loc, size);
            return;
        }

        //::operator delete(MakeVoidPtr(loc), std::nothrow_t{});
        ::operator delete(MakeVoidPtr(loc));
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
