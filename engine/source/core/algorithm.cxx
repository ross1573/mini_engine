export module mini.core:algorithm;

import :type;
import :iterator;
import :memory;

namespace mini
{

template <ForwardIteratorT T>
inline constexpr bool CheckDest(T dest)
{
    return dest.IsValid();
}

template <ForwardIteratorT T>
inline constexpr bool CheckRange(T begin, T end)
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

} // namespace mini

export namespace mini
{

template <typename T, typename U> 
    requires IteratorCopyableFromT<T, U>
inline constexpr void CopyRange(T dest, U begin, U end)
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyRange(dest, begin, end);
}

template <typename T, typename U> 
    requires IteratorCopyableFromT<T, U>
inline constexpr void CopyBackward(T dest, U begin, U end)
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyBackward(dest, begin, end);
}

template <typename T, typename U> 
    requires IteratorMovableFromT<T, U>
inline constexpr void MoveRange(T dest, U begin, U end)
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveRange(dest, begin, end);
}

template <typename T, typename U> 
    requires IteratorMovableFromT<T, U>
inline constexpr void MoveBackward(T dest, U begin, U end)
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveBackward(dest, begin, end);
}

template <ForwardIteratorT T, ForwardIteratorT U>
    requires ComparableWithT<typename T::Value, typename U::Value>
inline constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    ASSERT(CheckRange(begin1, end1));
    ASSERT(CheckRange(begin2, end2));

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
inline constexpr SizeT Distance(T first, T last)
{
    if (first == last)
    {
        return 0;
    }

    ASSERT(CheckRange(first, last));

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