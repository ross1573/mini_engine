module;

#include "memory.h"

export module mini.core:memory_operation;

import :type;
import :assert;
import :cmemory;
import :utility_operation;

namespace mini {

template <typename T>
concept IndirectAddressableT = requires(T ele) {
    { ele.address() } -> PointerT;
};

export template <typename T>
concept AddressableT = PointerT<T> || IndirectAddressableT<T>;

export template <typename T>
concept DereferencableT = requires(T ele) { *ele; };

export template <AddressableT T>
constexpr decltype(auto) address(T const& ele) noexcept
{
    if constexpr (PointerT<T>) {
        return ele;
    } else if constexpr (IndirectAddressableT<T>) {
        return ele.address();
    } else {
        UNSUPPORTED("unknown type for address conversion", T);
    }
}

export template <typename T>
constexpr decltype(auto) addressof(T& ele)
{
    return BUILTIN_ADDRESS_OF(ele);
}

export template <typename T>
constexpr decltype(auto) addressof(T const&& rval) = deleted_function("cannot get address of r-value");

export template <PointerT T, PointerT U>
constexpr bool pointerOverlapping(T ptr, U begin, U end)
{
    if consteval {
        return false;
    }

    return (begin <= ptr) && (end > ptr);
}

export template <PointerT T, PointerT U>
constexpr bool pointerOverlapping(T b1, T e1, U b2, U e2)
{
    if consteval {
        return false;
    }

    return pointerOverlapping(b1, b2, e2) || pointerOverlapping(e1, b2, e2);
}

} // namespace mini

namespace mini::memory {

template <typename T, typename U>
consteval bool triviallyOperatable()
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

// msvc won't evaluate placement new at compile time if there's no return.
// expression has to be decorated with [[msvc::constexpr]]
template <typename T, typename... Args>
constexpr T* constructAtImpl(T* ptr, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    MSVC_CONSTEXPR return ::new (static_cast<void*>(ptr)) T(forwardCast<Args>(args)...);
}

export template <NonArrayT T, typename... Args>
constexpr void constructAt(T* ptr, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
{
#if MSVC
    if consteval {
        constructAtImpl(ptr, forwardCast<Args>(args)...);
        return;
    }
#endif

    ASSERT(ptr != nullptr, "invalid location for object");
    ::new (static_cast<void*>(ptr)) T(forwardCast<Args>(args)...);
}

export template <NoThrowDefaultConstructibleT T>
constexpr void beginLifetime(T* begin, T* end) noexcept
{
    ASSERT(begin <= end, "invalid range");

    if !consteval {
        return;
    }

    for (; begin != end; ++begin) {
        constructAt(begin);
    }
}

export template <NoThrowDefaultConstructibleT T>
constexpr void beginLifetime(T* loc) noexcept
{
    if !consteval {
        return;
    }

    constructAt(loc);
}

export template <NonArrayT T>
constexpr void destructAt(T* ptr) noexcept(DestructibleT<T>)
{
    ASSERT(ptr != nullptr, "invalid location for object");

    if constexpr (TrivialT<T>) {
        return;
    }

    ptr->~T();
}

export template <typename T, typename... Args>
constexpr void constructRangeArgs(T begin, T end, Args&&... args)
{
    for (; begin != end; ++begin) {
        constructAt(address(begin), forwardCast<Args>(args)...);
    }
}

export template <typename T, typename U>
constexpr void constructRange(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        beginLifetime(dest, dest + size);
        memcopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        constructAt(address(dest), *begin);
    }
}

export template <typename T, typename U>
constexpr void constructBackward(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        beginLifetime(dest - size, dest);
        memcopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        constructAt(address(--dest), *(--end));
    }
}

export template <typename T, typename U>
constexpr void moveconstructRange(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        beginLifetime(dest, dest + size);
        memcopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        constructAt(address(dest), moveCast(*begin));
    }
}

export template <typename T, typename U>
constexpr void moveconstructBackward(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end - begin);
        beginLifetime(dest - size, dest);
        memcopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        constructAt(address(--dest), moveCast(*(--end)));
    }
}

export template <typename T>
constexpr void destructRange(T begin, T end)
{
    for (; begin != end; ++begin) {
        destructAt(address(begin));
    }
}

} // namespace mini::memory