#pragma once

#include "core/utility.h"
#include "memory/memory.h"

namespace mini
{
template <typename T> struct AllocResult;
template <typename T> struct Allocator;
}

namespace mini::detail
{

template <typename AllocT>
struct IsDefaultAlloc : FalseT {};

template <typename T>
struct IsDefaultAlloc<mini::Allocator<T>> : TrueT {};

template <typename T>
inline constexpr bool DefaultAlloc = IsDefaultAlloc<T>::value;

template <typename T>
concept UnbindedAllocatorT = CopyableT<T> &&
    requires(T alloc, SizeT s, typename T::Ptr loc)
{
    !RefT<typename T::Value>;
    PtrT<typename T::Ptr>;

    { alloc.Allocate(s) } -> SameAsT<AllocResult<typename T::Value>>;
    { alloc.Increment(s, s) } -> SameAsT<AllocResult<typename T::Value>>;
    { alloc.Deallocate(loc, s) };
};

template <typename AllocT, typename T>
concept AllocatorT = UnbindedAllocatorT<AllocT> && SameAsT<typename AllocT::Value, T>;

template <typename AllocT, typename T>
concept NoThrowAllocatorT = (AllocatorT<AllocT, T> &&
                             NoThrowCopyableT<AllocT> &&
                             NoThrowCallableT<decltype(&AllocT::Allocate), SizeT> &&
                             NoThrowCallableT<decltype(&AllocT::Increment), SizeT, SizeT> &&
                             NoThrowCallableT<decltype(&AllocT::Deallocate),
                             typename AllocT::Ptr, SizeT>);

template <typename AllocT, typename T>
concept AllocatorRefT = AllocatorT<RemoveRefT<AllocT>, T>;

template <typename AllocT, typename U>
concept AllocRebindDeclaredT = requires(AllocT alloc)
{
    { alloc.template Rebind<U>() } -> AllocatorRefT<U>;
};

template <typename AllocT, typename U>
concept AllocRebindOverloadedT = requires(AllocT alloc)
{
    { RebindAllocator<U>(alloc) } -> AllocatorRefT<U>;
};

template <typename AllocT, typename U>
concept RebindableWithT = (UnbindedAllocatorT<AllocT> &&
                           (AllocRebindDeclaredT<AllocT, U> || AllocRebindOverloadedT<AllocT, U>));

} // namespace mini::detail

namespace mini
{

template <typename T>
struct AllocResult
{
    T* pointer;
    SizeT capacity;
};

template <typename AllocT, typename T>
concept AllocatorT = detail::AllocatorT<AllocT, T>;

template <typename AllocT, typename T>
concept NoThrowAllocatorT = detail::NoThrowAllocatorT<AllocT, T>;

template <typename AllocT>
concept UnbindedAllocatorTT = detail::UnbindedAllocatorT<AllocT>;

template <typename AllocT, typename U>
concept RebindableWithT = detail::RebindableWithT<AllocT, U>;

template <typename T>
struct Allocator
{
    typedef T Value;
    typedef T* Ptr;

    [[nodiscard]] constexpr AllocResult<T> Allocate(SizeT size) const noexcept
    {
        Ptr ptr = nullptr;
        if (std::is_constant_evaluated())
        {
            // TODO: only compiler can do constexpr allocate
            ptr = std::allocator<T>{}.allocate(size);
        }
        else
        {
            ptr = static_cast<T*>(::operator new(size * sizeof(T), std::nothrow_t{}));
            VERIFY(ptr, "allocation failed. possible out of memory");
        }

        return {.pointer = ptr, .capacity = size};
    }

    [[nodiscard]] constexpr AllocResult<T> Increment(SizeT oldCap, SizeT size) const noexcept
    {
        SizeT newCap = oldCap < size ? oldCap + size : oldCap << 1;
        CONSTEXPR_ASSERT(newCap != 0, "invalid capacity on buffer increment");
        return Allocate(newCap);
    }

    constexpr void Deallocate(Ptr loc, SizeT size) const noexcept
    {
        if (std::is_constant_evaluated())
        {
            // TODO: only compiler can do constexpr deallocate
            std::allocator<T>{}.deallocate(loc, size);
            return;
        }

        ::operator delete(detail::MakeVoidPtr(loc), std::nothrow_t{});
    }
};

template <typename U>
[[force_inline]] constexpr decltype(auto) RebindAllocator(auto const& alloc)
    requires detail::AllocRebindDeclaredT<decltype(alloc), U>
{
    return alloc.template Rebind<U>();
}

template <typename U>
[[force_inline]] constexpr auto&& RebindAllocator(auto&& alloc)
    requires detail::AllocatorT<RemoveRefT<decltype(alloc)>, U>
{
    return ForwardArg<RemoveRefT<decltype(alloc)>>(alloc);
}

template <typename U>
[[force_inline]] constexpr auto RebindAllocator(auto const& alloc)
    requires (!detail::AllocatorT<RemoveRefT<
        decltype(alloc)>, U>&& detail::DefaultAlloc<RemoveRefT<decltype(alloc)>>)
{
    return mini::Allocator<U>{};
}

template <typename T, typename U>
[[force_inline]] constexpr bool operator==(Allocator<T> const&, Allocator<U> const&)
{
    return true;
}

struct DummyAllocator
{
    typedef void Value;
    typedef void* Ptr;

    template <typename T>
    constexpr AllocResult<void> Allocate(T&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
        return {nullptr, 0};
    }

    template <typename T>
    constexpr AllocResult<void> Increment(T&&, T&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
        return {nullptr, 0};
    }

    template <typename T, typename U>
    constexpr void Deallocate(T&&, U&&) const noexcept
    {
        NEVER_CALLED("dummy allocator should be rebinded");
    }
};

template <typename U>
[[force_inline]] constexpr auto RebindAllocator(mini::DummyAllocator)
{
    return mini::Allocator<U>{};
}

[[force_inline]] constexpr bool operator==(DummyAllocator const&, DummyAllocator const&)
{
    return true;
}

} // namespace mini