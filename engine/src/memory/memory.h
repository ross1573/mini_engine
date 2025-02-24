#pragma once

#include "core/utility.h"
#include "container/iterator/common.h"

namespace mini::detail
{

template <typename T>
constexpr void* MakeVoidPtr(T* ptr)
{
    return const_cast<void*>(static_cast<const volatile void*>(ptr));
}

} // namespace mini::detail

namespace mini::memory
{

template <typename T>
[[force_inline]] constexpr decltype(auto) AddressOf(T ele) noexcept
{
    if constexpr (PtrT<T>)
    {
        return ele;
    }
    else if constexpr (ForwardIteratorT<T>)
    {
        return ele.Address();
    }
    else
    {
        NEVER_CALLED("cannot extract pointer from the type", T);
    }
}

template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
constexpr void ConstructAt(T* ptr, Args&&... args) noexcept(NoThrowConstructibleFromT<T, Args...>)
{
    if (std::is_constant_evaluated())
    {
        // TODO: only compiler can do constexpr construct_at
        std::construct_at(ptr, ForwardArg<Args>(args)...);
        return;
    }

    ::new (detail::MakeVoidPtr(ptr)) T(ForwardArg<Args>(args)...);
}

template <typename T>
    requires (!std::is_array_v<T>)
constexpr void DestructAt(T* ptr) noexcept(DestructibleT<T>)
{
    if (std::is_constant_evaluated())
    {
        // TODO: only compiler can do constexpr destroy
        std::destroy_at(ptr);
        return;
    }

    ptr->~T();
}

template <typename T, typename... Args>
constexpr void ConstructRangeArgs(T begin, T end, Args&&... args)
{
    for (; begin != end; ++begin)
    {
        ConstructAt(AddressOf(begin), ForwardArg<Args>(args)...);
    }
}

template <typename T, typename U>
constexpr void ConstructRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest)
    {
        ConstructAt(AddressOf(dest), *begin);
    }
}

template <typename T, typename U>
constexpr void ConstructBackward(T dest, U begin, U end)
{
    for (; end != begin;)
    {
        ConstructAt(AddressOf(--dest), *(--end));
    }
}

template <typename T, typename U>
constexpr void MoveConstructRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest)
    {
        ConstructAt(AddressOf(dest), MoveArg(*begin));
    }
}

template <typename T, typename U>
constexpr void MoveConstructBackward(T dest, U begin, U end)
{
    for (; end != begin;)
    {
        ConstructAt(AddressOf(--dest), MoveArg(*(--end)));
    }
}

template <typename T>
constexpr void DestructRange(T begin, T end)
{
    for (; begin != end; ++begin)
    {
        DestructAt(AddressOf(begin));
    }
}

template <typename T, typename U>
constexpr void CopyRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest)
    {
        *dest = *begin;
    }
}

template <typename T, typename U>
constexpr void CopyBackward(T dest, U begin, U end)
{
    for (; end != begin;)
    {
        *(--dest) = *(--end);
    }
}

template <typename T, typename U>
constexpr void MoveRange(T dest, U begin, U end)
{
    for (; begin != end; ++begin, ++dest)
    {
        *dest = MoveArg(*begin);
    }
}

template <typename T, typename U>
constexpr void MoveBackward(T dest, U begin, U end)
{
    for (; end != begin;)
    {
        *(--dest) = MoveArg(*(--end));
    }
}

template <typename T, typename U>
constexpr bool EqualRange(T begin1, U begin2, U end2)
{
    for (; begin2 != end2; ++begin1, ++begin2)
    {
        if (*begin1 != *begin2)
        {
            return false;
        }
    }

    return true;
}

template <typename T, typename U>
constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
    {
        if (*begin1 != *begin2)
        {
            return false;
        }
    }

    return begin1 == end1 && begin2 == end2;
}

} // namespace mini::memory