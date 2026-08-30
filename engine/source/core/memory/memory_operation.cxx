module;

#include "memory.h"

export module mini.core:memory_operation;

import :type;
import :assert;
import :cmemory;
import :utility_operation;

namespace mini::memory {

template <typename T>
concept IndirectAddressableT = requires(T ele) {
    { ele.Address() } -> PointerT;
};

export template <typename T>
concept AddressableT = PointerT<T> || IndirectAddressableT<T>;

export template <typename T>
concept DereferencableT = requires(T ele) { *ele; };

template <typename T>
inline constexpr void* MakeVoidPtr(T* ptr)
{
    return const_cast<void*>(static_cast<const volatile void*>(ptr));
}

export template <AddressableT T>
inline constexpr decltype(auto) ToAddress(T const& ele) noexcept
{
    if constexpr (PointerT<T>) {
        return ele;
    } else if constexpr (IndirectAddressableT<T>) {
        return ele.Address();
    } else {
        UNSUPPORTED("unknown type for address conversion", T);
    }
}

export template <typename T>
inline constexpr decltype(auto) AddressOf(T& ele)
{
    return BUILTIN_ADDRESS_OF(ele);
}

template <typename T, typename U>
consteval bool IsTriviallyOperatable()
{
    if constexpr (PointerT<T> && PointerT<U>) {
        using ValueT = RemoveConstT<RemovePtrT<T>>;
        using ValueU = RemoveConstT<RemovePtrT<U>>;

        if constexpr (SameAsT<ValueT, ValueU>) {
            return TrivialT<ValueT> && TrivialT<ValueU>;
        }
    }

    // TODO: contiguous iterator case
    return false;
}

export template <PointerT T, PointerT U>
inline constexpr bool IsPtrOverlapping(T ptr, U begin, U end)
{
    if consteval {
        return false;
    }

    return (begin <= ptr) && (end > ptr);
}

export template <PointerT T, PointerT U>
inline constexpr bool IsPtrOverlapping(T b1, T e1, U b2, U e2)
{
    if consteval {
        return false;
    }

    return IsPtrOverlapping(b1, b2, e2) || IsPtrOverlapping(e1, b2, e2);
}

// msvc won't evaluate placement new at compile time if there's no return.
// this might be another stupid bug from msvc, since the expression has to be decorated with [[msvc::constexpr]]
template <typename T, typename... Args>
inline constexpr T* ConstructAtImpl(T* ptr, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    MSVC_CONSTEXPR return ::new (static_cast<void*>(ptr)) T(ForwardArg<Args>(args)...);
}

export template <NonArrT T, typename... Args>
inline constexpr void ConstructAt(T* ptr, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    ASSERT(ptr, "invalid location for object");

    if consteval {
        ConstructAtImpl(ptr, ForwardArg<Args>(args)...);
        return;
    }

    ::new (MakeVoidPtr(ptr)) T(ForwardArg<Args>(args)...);
}

export template <NoThrowDefaultConstructibleT T>
inline constexpr void BeginLifetime(T* begin, T* end) noexcept
{
    ASSERT(begin <= end, "invalid range");

    if consteval {
        for (; begin != end; ++begin) {
            ConstructAt(begin);
        }
    }
}

export template <NoThrowDefaultConstructibleT T>
inline constexpr void BeginLifetime(T* loc) noexcept
{
    if consteval {
        ConstructAt(loc);
    }
}

export template <NonArrT T>
inline constexpr void DestructAt(T* ptr) noexcept(DestructibleT<T>)
{
    ASSERT(ptr, "invalid location for object");

    if constexpr (TrivialT<T>) {
        return;
    }

    ptr->~T();
}

export template <typename T, typename... Args>
inline constexpr void ConstructRangeArgs(T begin, T end, Args&&... args)
{
    for (; begin != end; ++begin) {
        ConstructAt(ToAddress(begin), ForwardArg<Args>(args)...);
    }
}

export template <typename T, typename U>
inline constexpr void ConstructRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        BeginLifetime(dest, dest + size);
        MemCopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        ConstructAt(ToAddress(dest), *begin);
    }
}

export template <typename T, typename U>
inline constexpr void ConstructBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        BeginLifetime(dest - size, dest);
        MemCopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        ConstructAt(ToAddress(--dest), *(--end));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        BeginLifetime(dest, dest + size);
        MemCopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        ConstructAt(ToAddress(dest), MoveArg(*begin));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        BeginLifetime(dest - size, dest);
        MemCopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        ConstructAt(ToAddress(--dest), MoveArg(*(--end)));
    }
}

export template <typename T>
inline constexpr void DestructRange(T begin, T end)
{
    for (; begin != end; ++begin) {
        DestructAt(ToAddress(begin));
    }
}

} // namespace mini::memory