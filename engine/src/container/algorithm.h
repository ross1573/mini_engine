#pragma once
#pragma once

#include "memory/memory.h"
#include "container/iterator/common.h"

namespace mini::detail
{

template <ForwardIteratorT T>
[[force_inline]] constexpr bool CheckDest(T dest)
{
    return dest.IsValid();
}

template <ForwardIteratorT T>
constexpr bool CheckRange(T begin, T end)
{
    if (!begin.IsValid() || !begin.IsValidWith(end)) [[unlikely]]
    {
        return false;
    }

    if constexpr (BidrectionalIteratorT<T>)
    {
        return (end - 1).IsValid();
    }

    return true;
}

} // namespace mini::detail

namespace mini
{

template <ForwardIteratorT T, ForwardIteratorT U>
    requires CopyableWithT<T, U>
constexpr void CopyRange(T dest, U begin, U end)
{
    ASSERT(detail::CheckDest(dest));
    ASSERT(detail::CheckRange(begin, end));
    memory::CopyRange(dest, begin, end);
}

template <ForwardIteratorT T, ForwardIteratorT U>
    requires CopyableWithT<T, U>
constexpr void CopyBackward(T dest, U begin, U end)
{
    ASSERT(detail::CheckDest(dest));
    ASSERT(detail::CheckRange(begin, end));
    memory::CopyBackward(dest, begin, end);
}

template <ForwardIteratorT T, ForwardIteratorT U>
    requires MovableWithT<T, U>
constexpr void MoveRange(T dest, U begin, U end)
{
    ASSERT(detail::CheckDest(dest));
    ASSERT(detail::CheckRange(begin, end));
    memory::MoveRange(dest, begin, end);
}

template <ForwardIteratorT T, ForwardIteratorT U>
    requires MovableWithT<T, U>
constexpr void MoveBackward(T dest, U begin, U end)
{
    ASSERT(detail::CheckDest(dest));
    ASSERT(detail::CheckRange(begin, end));
    memory::MoveBackward(dest, begin, end);
}

template <ForwardIteratorT T, ForwardIteratorT U>
    requires ComparableWithT<typename T::Value, typename U::Value>
constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    ASSERT(detail::CheckRange(begin1, end1));
    ASSERT(detail::CheckRange(begin2, end2));

    if constexpr (RandomAccessIteratorT<T> && RandomAccessIteratorT<U>)
    {
        OffsetT diff1 = end1 - begin1;
        OffsetT diff2 = end2 - begin2;
        ASSERT(diff1 >= 0 && diff2 >= 0, "distance cannot be negative value");

        if (diff1 != diff2)
        {
            return false;
        }

        return memory::EqualRange(begin1, begin2, end2);
    }

    return memory::EqualRange(begin1, end1, begin2, end2);
}

template <ForwardIteratorT T>
constexpr SizeT Distance(T first, T last)
{
    if (first == last)
    {
        return 0;
    }

    ASSERT(detail::CheckRange(first, last));

    if constexpr (RandomAccessIteratorT<T>)
    {
        OffsetT diff = last - first;
        ASSERT(diff >= 0, "distance cannot be negative value");
        return static_cast<SizeT>(diff);
    }

    SizeT count = 0;
    for (; first != last; ++count);
    return count;
}

} // namespace mini