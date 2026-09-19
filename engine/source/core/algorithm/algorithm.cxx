export module mini.core:algorithm;

import :type;
import :iterator;
import :algorithm_memory;

namespace mini {

template <ForwardIteratorT T>
constexpr bool CheckDest(T dest)
{
    return dest.Valid();
}

template <ForwardIteratorT T>
constexpr bool CheckRange(T begin, T end)
{
    if (begin == end) [[unlikely]] {
        return true;
    }

    if (!begin.ValidWith(end)) [[unlikely]] {
        return false;
    }

    if constexpr (BidrectionalIteratorT<T>) {
        return (end - 1).Valid();
    }

    return true;
}

export template <typename T, typename U>
constexpr void CopyRange(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyRange(dest, begin, end);
}

export template <typename T, typename U>
constexpr void CopyBackward(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::CopyBackward(dest, begin, end);
}

export template <typename T, typename U>
constexpr void MoveRange(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveRange(dest, begin, end);
}

export template <typename T, typename U>
constexpr void MoveBackward(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(CheckDest(dest));
    ASSERT(CheckRange(begin, end));
    memory::MoveBackward(dest, begin, end);
}

export template <ForwardIteratorT T, ForwardIteratorT U>
constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
    requires EqualityComparableWithT<typename T::Value, typename U::Value>
{
    ASSERT(CheckRange(begin1, end1));
    ASSERT(CheckRange(begin2, end2));
    return memory::EqualRange(begin1, end1, begin2, end2);
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool EqualRange(T begin1, T end1, U begin2, U end2)
{
    ASSERT(CheckRange(begin1, end1));
    ASSERT(CheckRange(begin2, end2));

    offset_t diff1 = end1 - begin1;
    offset_t diff2 = end2 - begin2;
    ASSERT(diff1 >= 0 && diff2 >= 0, "distance cannot be negative value {}, {}", diff1, diff2);

    if (diff1 != diff2) {
        return false;
    }

    return memory::EqualRange(begin1, begin2, end2);
}

export template <ForwardIteratorT T, typename U>
constexpr void FillRange(T begin, T end, U const& value)
    requires ConvertibleToT<U, typename T::Value>
{
    ASSERT(CheckRange(begin, end));
    memory::FillRange(begin, end, value);
}

export template <ForwardIteratorT T>
constexpr size_t Distance(T first, T last)
{
    ASSERT(CheckRange(first, last));

    size_t count = 0;
    for (; first != last; ++count) { }
    return count;
}

export template <RandomAccessIteratorT T>
constexpr size_t Distance(T first, T last)
{
    offset_t diff = last - first;

    ASSERT(CheckRange(first, last));
    ASSERT(diff >= 0, "distance cannot be negative value {}", diff);

    return static_cast<size_t>(diff);
}

export template <ForwardIteratorT T, typename U = T::Value>
constexpr T Find(T first, T last, U const& value)
{
    ASSERT(CheckRange(first, last));
    return memory::Find(first, last, value);
}

export template <ForwardIteratorT T, CallableWithReturnT<bool, typename T::Value> PredT>
constexpr T FindIf(T first, T last, PredT pred)
{
    ASSERT(CheckRange(first, last));
    return memory::FindIf(first, last, pred);
}

export template <ForwardIteratorT T, CallableWithReturnT<bool, typename T::Value> PredT>
constexpr T FindIfNot(T first, T last, PredT pred)
{
    ASSERT(CheckRange(first, last));
    return memory::FindIfNot(first, last, pred);
}

} // namespace mini