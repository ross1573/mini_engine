export module mini.core:algorithm;

import :type;
import :iterator;
import :memory;

namespace mini {

template <ForwardIteratorT T>
inline constexpr bool CheckDest(T dest)
{
    return dest.IsValid();
}

template <ForwardIteratorT T>
inline constexpr bool CheckRange(T begin, T end)
{
    if (begin == end) [[unlikely]] {
        return true;
    }
    else if (!begin.IsValidWith(end)) [[unlikely]] {
        return false;
    }

    if constexpr (BidrectionalIteratorT<T>) {
        return (end - 1).IsValid();
    }

    return true;
}

export template <typename T, typename U>
inline constexpr void CopyRange(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyRange(dest, begin, end);
}

export template <typename T, typename U>
inline constexpr void CopyBackward(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyBackward(dest, begin, end);
}

export template <typename T, typename U>
inline constexpr void MoveRange(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveRange(dest, begin, end);
}

export template <typename T, typename U>
inline constexpr void MoveBackward(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveBackward(dest, begin, end);
}

export template <ForwardIteratorT T, ForwardIteratorT U>
inline constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
    requires EqualityComparableWithT<typename T::Value, typename U::Value>
{
    ASSERT(CheckRange(begin1, end1));
    ASSERT(CheckRange(begin2, end2));
    return memory::EqualRange(begin1, end1, begin2, end2);
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
inline constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    ASSERT(CheckRange(begin1, end1));
    ASSERT(CheckRange(begin2, end2));

    OffsetT diff1 = end1 - begin1;
    OffsetT diff2 = end2 - begin2;
    ASSERT(diff1 >= 0 && diff2 >= 0, "distance cannot be negative value");

    if (diff1 != diff2) {
        return false;
    }

    return memory::EqualRange(begin1, begin2, end2);
}

export template <ForwardIteratorT T, typename U>
inline constexpr void FillRange(T begin, T end, U const& value)
{
    ASSERT(CheckRange(begin, end));
    memory::FillRange(begin, end, value);
}

export template <ForwardIteratorT T>
inline constexpr SizeT Distance(T first, T last)
{
    ASSERT(CheckRange(first, last));

    SizeT count = 0;
    for (; first != last; ++count);
    return count;
}

export template <RandomAccessIteratorT T>
inline constexpr SizeT Distance(T first, T last)
{
    OffsetT diff = last - first;

    ASSERT(CheckRange(first, last));
    ASSERT(diff >= 0, "distance cannot be negative value");

    return static_cast<SizeT>(diff);
}

} // namespace mini