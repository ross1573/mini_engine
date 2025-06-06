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

template <typename T>
inline constexpr void* MakeVoidPtr(T* ptr)
{
    return const_cast<void*>(static_cast<const volatile void*>(ptr));
}

namespace mini::memory {

export template <typename T>
concept DereferencableT = requires(T ele) { *ele; };

export template <typename T>
concept AddressableT = PtrT<T> || ForwardIteratorT<T>;

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

template <PtrT T, PtrT U>
inline constexpr bool IsPtrOverlapping(T ptr, U begin, U end)
{
    if (IsConstantEvaluated()) {
        return false;
    }

    return (begin <= ptr) && (end > ptr);
}

template <PtrT T, PtrT U>
inline constexpr bool IsPtrOverlapping(T b1, T e1, U b2, U e2)
{
    if (IsConstantEvaluated()) {
        return false;
    }

    return IsPtrOverlapping(b1, b2, e2) || IsPtrOverlapping(e1, b2, e2);
}

export template <NonArrT T, typename... Args>
inline constexpr void ConstructAt(T* ptr, Args&&... args)
    noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    if (IsConstantEvaluated()) {
        CONSTEXPR_CONSTRUCT_AT(ptr, ForwardArg<Args>(args)...);
        return;
    }

    ::new (MakeVoidPtr(ptr)) T(ForwardArg<Args>(args)...);
}

export template <NonArrT T>
inline constexpr void DestructAt(T* ptr) noexcept(DestructibleT<T>)
{
    if (IsConstantEvaluated()) {
        CONSTEXPR_DESTRUCT_AT(ptr);
        return;
    }

    ptr->~T();
}

export template <TrivialT T>
inline constexpr void MemCopy(T* dst, T const* src, SizeT len) noexcept
{
    if (!IsConstantEvaluated()) {
        BUILTIN_MEMCPY(dst, src, len * sizeof(T));
        return;
    }

    for (; len; --len) {
        *dst++ = *src++;
    }
}

export template <TrivialT T>
inline constexpr void MemMove(T* dst, T const* src, SizeT len) noexcept
{
    if (!IsConstantEvaluated()) {
        BUILTIN_MEMMOVE(dst, src, len * sizeof(T));
        return;
    }

    if (static_cast<T const*>(dst) < src) {
        for (; len; --len) {
            *dst++ = *src++;
        }
    }
    else {
        T const* s = src + len;
        T* d = dst + len;

        for (; len != 0; --len) {
            *(--d) = *(--s);
        }
    }
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
    for (; begin != end; ++begin, ++dest) {
        ConstructAt(AddressOf(dest), *begin);
    }
}

export template <typename T, typename U>
inline constexpr void ConstructBackward(T dest, U begin, U end)
{
    for (; end != begin;) {
        ConstructAt(AddressOf(--dest), *(--end));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest) {
        ConstructAt(AddressOf(dest), MoveArg(*begin));
    }
}

export template <typename T, typename U>
inline constexpr void MoveConstructBackward(T dest, U begin, U end)
{
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
    for (; begin != end; ++begin, ++dest) {
        *AddressOf(dest) = *AddressOf(begin);
    }
}

export template <typename T, typename U>
inline constexpr void CopyBackward(T dest, U begin, U end)
{
    for (; end != begin;) {
        *AddressOf(--dest) = *AddressOf(--end);
    }
}

export template <typename T, typename U>
inline constexpr void MoveRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest) {
        *AddressOf(dest) = MoveArg(*AddressOf(begin));
    }
}

export template <typename T, typename U>
inline constexpr void MoveBackward(T dest, U begin, U end)
{
    for (; end != begin;) {
        *AddressOf(--dest) = MoveArg(*AddressOf(--end));
    }
}

export template <typename T, typename U>
inline constexpr bool EqualRange(T begin1, U begin2, U end2)
{
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