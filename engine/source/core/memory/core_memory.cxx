module;

#include <memory>

#include "assertion.h"
#include "builtin.h"

// TODO: constexpr placement new operator is coming soon!
#define CONSTEXPR_CONSTRUCT_AT std::construct_at
#define CONSTEXPR_DESTRUCT_AT  std::destroy_at

export module mini.core:memory;

import :type;
import :utility;
import :iterator;

namespace mini::memory {

export template <typename T>
concept DereferencableT = requires(T ele) { *ele; };

export template <typename T>
concept AddressableT = PtrT<T> || ForwardIteratorT<T>;

template <typename T>
inline constexpr void* MakeVoidPtr(T* ptr)
{
    return const_cast<void*>(static_cast<const volatile void*>(ptr));
}

export template <AddressableT T>
inline constexpr decltype(auto) AddressOf(T const& ele) noexcept
{
    if constexpr (PtrT<T>) {
        return ele;
    }
    else if constexpr (ForwardIteratorT<T>) {
        return ele.Address();
    }
    else {
        NEVER_CALLED("unknown type for address conversion", T);
    }
}

template <typename T, typename U>
consteval bool IsTriviallyOperatable()
{
    if constexpr (PtrT<T> && PtrT<U>) {
        using ValueT = RemoveConstT<RemovePtrT<T>>;
        using ValueU = RemoveConstT<RemovePtrT<U>>;

        if constexpr (SameAsT<ValueT, ValueU>) {
            return TrivialT<ValueT> && TrivialT<ValueU>;
        }
    }

    // TODO: contiguous iterator case
    return false;
}

template <PtrT T, PtrT U>
inline constexpr bool IsPtrOverlapping(T ptr, U begin, U end)
{
    if consteval {
        return false;
    }

    return (begin <= ptr) && (end > ptr);
}

template <PtrT T, PtrT U>
inline constexpr bool IsPtrOverlapping(T b1, T e1, U b2, U e2)
{
    if consteval {
        return false;
    }

    return IsPtrOverlapping(b1, b2, e2) || IsPtrOverlapping(e1, b2, e2);
}

export template <NonArrT T, typename... Args>
inline constexpr void ConstructAt(T* ptr, Args&&... args)
    noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    ASSERT(ptr, "invalid location for object");

    if consteval {
        CONSTEXPR_CONSTRUCT_AT(ptr, ForwardArg<Args>(args)...);
        return;
    }

    ::new (MakeVoidPtr(ptr)) T(ForwardArg<Args>(args)...);
}

template <TrivialT T>
inline constexpr void BeginLifetime(T* begin, T* end) noexcept
{
    ASSERT(begin <= end, "invalid range");

    if consteval {
        for (; begin != end; ++begin) {
            ConstructAt(AddressOf(begin));
        }
    }
}

template <TrivialT T>
inline constexpr void BeginLifetime(T* loc) noexcept
{
    ASSERT(loc, "invalid location for object");

    if consteval {
        ConstructAt(AddressOf(loc));
    }
}

export template <NonArrT T>
inline constexpr void DestructAt(T* ptr) noexcept(DestructibleT<T>)
{
    ASSERT(ptr, "invalid location for object");

    if consteval {
        CONSTEXPR_DESTRUCT_AT(ptr);
        return;
    }

    ptr->~T();
}

template <TrivialT T>
inline constexpr void EndLifetime(T* begin, T* end) noexcept
{
    ASSERT(begin <= end, "invalid range");

    if consteval {
        for (; begin != end; ++begin) {
            DestructAt(AddressOf(begin));
        }
    }
}

template <TrivialT T>
inline constexpr void EndLifetime(T* loc) noexcept
{
    ASSERT(loc, "invalid location for object");

    if consteval {
        DestructAt(AddressOf(loc));
    }
}

export template <TrivialT T>
inline constexpr void MemCopy(T* dst, T const* src, SizeT len) noexcept
{
    if !consteval {
        BUILTIN_MEMCPY(dst, src, len * sizeof(T));
        return;
    }

    for (; len; --len) {
        *dst++ = *src++;
    }
}

export template <TrivialT T>
inline constexpr void MemCopyBackward(T* dst, T const* src, SizeT len) noexcept
{
    if !consteval {
        BUILTIN_MEMMOVE(dst - len, src - len, len * sizeof(T));
        return;
    }

    for (; len; --len) {
        *(--dst) = *(--src);
    }
}

export template <TrivialT T>
inline constexpr void MemMove(T* dst, T const* src, SizeT len) noexcept
{
    if !consteval {
        BUILTIN_MEMMOVE(dst, src, len * sizeof(T));
        return;
    }

    if (static_cast<T const*>(dst) < src) {
        MemCopy(dst, src, len);
    }
    else {
        MemCopyBackward(dst + len, src + len, len);
    }
}

export template <TrivialT T>
inline constexpr bool MemCompare(T const* dst, T const* src, SizeT len) noexcept
{
    if !consteval {
        return BUILTIN_MEMCMP(dst, src, len * sizeof(T));
    }

    for (; len; --len) {
        if (*dst++ != *src++) {
            return false;
        }
    }

    return true;
}

export template <typename T, typename... Args>
inline constexpr void ConstructRangeArgs(T begin, T end, Args&&... args)
{
    for (; begin != end; ++begin) {
        ConstructAt(AddressOf(begin), ForwardArg<Args>(args)...);
    }
}

export template <typename T, typename U>
inline constexpr void ConstructRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        SizeT size = static_cast<SizeT>(end - begin);
        BeginLifetime(dest, dest + size);
        MemCopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        ConstructAt(AddressOf(dest), *begin);
    }
}

export template <typename T, typename U>
inline constexpr void ConstructBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        SizeT size = static_cast<SizeT>(end - begin);
        BeginLifetime(dest - size, dest);
        MemCopyBackward(dest, end, static_cast<SizeT>(end - begin));
        return;
    }

    for (; end != begin;) {
        ConstructAt(AddressOf(--dest), *(--end));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        SizeT size = static_cast<SizeT>(end - begin);
        BeginLifetime(dest, dest + size);
        MemCopy(dest, begin, size);
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        ConstructAt(AddressOf(dest), MoveArg(*begin));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        SizeT size = static_cast<SizeT>(end - begin);
        BeginLifetime(dest - size, dest);
        MemCopyBackward(dest, end, static_cast<SizeT>(end - begin));
        return;
    }

    for (; end != begin;) {
        ConstructAt(AddressOf(--dest), MoveArg(*(--end)));
    }
}

export template <typename T>
inline constexpr void DestructRange(T begin, T end)
{
    for (; begin != end; ++begin) {
        DestructAt(AddressOf(begin));
    }
}

export template <typename T, typename U>
inline constexpr void CopyRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        MemCopy(dest, begin, static_cast<SizeT>(end - begin));
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        *AddressOf(dest) = *AddressOf(begin);
    }
}

export template <typename T, typename U>
inline constexpr void CopyBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        MemCopyBackward(dest, end, static_cast<SizeT>(end - begin));
        return;
    }

    for (; end != begin;) {
        *AddressOf(--dest) = *AddressOf(--end);
    }
}

export template <typename T, typename U>
inline constexpr void MoveRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        MemCopy(dest, begin, static_cast<SizeT>(end - begin));
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        *AddressOf(dest) = MoveArg(*AddressOf(begin));
    }
}

export template <typename T, typename U>
inline constexpr void MoveBackward(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        MemCopyBackward(dest, end, static_cast<SizeT>(end - begin));
        return;
    }

    for (; end != begin;) {
        *AddressOf(--dest) = MoveArg(*AddressOf(--end));
    }
}

export template <typename T, typename U>
inline constexpr bool EqualRange(T begin1, U begin2, U end2)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        return MemCompare(begin1, begin2, static_cast<SizeT>(end2 - begin2));
    }

    for (; begin2 != end2; ++begin1, ++begin2) {
        if (*AddressOf(begin1) != *AddressOf(begin2)) {
            return false;
        }
    }

    return true;
}

export template <typename T, typename U>
inline constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        SizeT size = static_cast<SizeT>(end2 - begin2);
        if (size != (end1 - begin1)) {
            return false;
        }

        return MemCompare(begin1, begin2, size);
    }

    for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2) {
        if (*AddressOf(begin1) != *AddressOf(begin2)) {
            return false;
        }
    }

    return begin1 == end1 && begin2 == end2;
}

export template <typename T, typename U>
inline constexpr void FillRange(T begin, T end, U const& value)
{
    for (; begin != end; ++begin) {
        *AddressOf(begin) = value;
    }
}

} // namespace mini::memory