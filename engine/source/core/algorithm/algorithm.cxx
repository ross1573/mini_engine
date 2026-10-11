export module mini.core:algorithm;

import :type;
import :iterator;
import :algorithm_memory;

namespace mini {

template <ForwardIteratorT T>
constexpr bool checkDest(T dest)
{
    return dest.valid();
}

template <ForwardIteratorT T>
constexpr bool checkRange(T begin, T end)
{
    if (begin == end) [[unlikely]] {
        return true;
    }

    if (!begin.validWith(end)) [[unlikely]] {
        return false;
    }

    if constexpr (BidrectionalIteratorT<T>) {
        return (end - 1).valid();
    }

    return true;
}

export template <typename T, typename U>
constexpr void copyRange(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(checkDest(dest));
    ASSERT(checkRange(begin, end));
    memory::copyRange(dest, begin, end);
}

export template <typename T, typename U>
constexpr void copyBackward(T dest, U begin, U end)
    requires IteratorCopyableFromT<T, U>
{
    ASSERT(checkDest(dest));
    ASSERT(checkRange(begin, end));
    memory::copyBackward(dest, begin, end);
}

export template <typename T, typename U>
constexpr void moveRange(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(checkDest(dest));
    ASSERT(checkRange(begin, end));
    memory::moveRange(dest, begin, end);
}

export template <typename T, typename U>
constexpr void moveBackward(T dest, U begin, U end)
    requires IteratorMovableFromT<T, U>
{
    ASSERT(checkDest(dest));
    ASSERT(checkRange(begin, end));
    memory::moveBackward(dest, begin, end);
}

export template <ForwardIteratorT T, ForwardIteratorT U>
constexpr bool equalRange(T begin1, T end1, U begin2, U end2)
    requires EqualityComparableWithT<typename T::Value, typename U::Value>
{
    ASSERT(checkRange(begin1, end1));
    ASSERT(checkRange(begin2, end2));
    return memory::equalRange(begin1, end1, begin2, end2);
}

export template <RandomAccessIteratorT T, RandomAccessIteratorT U>
constexpr bool equalRange(T begin1, T end1, U begin2, U end2)
{
    ASSERT(checkRange(begin1, end1));
    ASSERT(checkRange(begin2, end2));

    offset_t diff1 = end1 - begin1;
    offset_t diff2 = end2 - begin2;
    ASSERT(diff1 >= 0 && diff2 >= 0, "distance cannot be negative value {}, {}", diff1, diff2);

    if (diff1 != diff2) {
        return false;
    }

    return memory::equalRange(begin1, begin2, end2);
}

export template <ForwardIteratorT T, typename U>
constexpr void fillRange(T begin, T end, U const& value)
    requires ConvertibleToT<U, typename T::Value>
{
    ASSERT(checkRange(begin, end));
    memory::fillRange(begin, end, value);
}

export template <ForwardIteratorT T>
constexpr size_t distance(T first, T last)
{
    ASSERT(checkRange(first, last));

    size_t count = 0;
    for (; first != last; ++count) { }
    return count;
}

export template <RandomAccessIteratorT T>
constexpr size_t distance(T first, T last)
{
    offset_t diff = last - first;

    ASSERT(checkRange(first, last));
    ASSERT(diff >= 0, "distance cannot be negative value {}", diff);

    return static_cast<size_t>(diff);
}

export template <ForwardIteratorT T, typename U = T::Value>
constexpr T find(T first, T last, U const& value)
{
    ASSERT(checkRange(first, last));
    return memory::find(first, last, value);
}

export template <ForwardIteratorT T, CallableWithReturnT<bool, typename T::Value> PredT>
constexpr T findIf(T first, T last, PredT pred)
{
    ASSERT(checkRange(first, last));
    return memory::findIf(first, last, pred);
}

export template <ForwardIteratorT T, CallableWithReturnT<bool, typename T::Value> PredT>
constexpr T findIfNot(T first, T last, PredT pred)
{
    ASSERT(checkRange(first, last));
    return memory::findIfNot(first, last, pred);
}

} // namespace mini