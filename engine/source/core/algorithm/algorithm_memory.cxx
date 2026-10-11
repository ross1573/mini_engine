export module mini.core:algorithm_memory;

import :type;
import :cmemory;
import :memory_operation;

namespace mini::memory {

export template <typename T, typename U>
constexpr void copyRange(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        memcopy(dest, begin, static_cast<size_t>(end - begin));
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        *address(dest) = *address(begin);
    }
}

export template <typename T, typename U>
constexpr void copyBackward(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        memcopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        *address(--dest) = *address(--end);
    }
}

export template <typename T, typename U>
constexpr void moveRange(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        memcopy(dest, begin, static_cast<size_t>(end - begin));
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        *address(dest) = moveCast(*address(begin));
    }
}

export template <typename T, typename U>
constexpr void moveBackward(T dest, U begin, U end)
{
    if constexpr (triviallyOperatable<T, U>()) {
        memcopyBackward(dest, end, static_cast<size_t>(end - begin));
        return;
    }

    for (; end != begin;) {
        *address(--dest) = moveCast(*address(--end));
    }
}

export template <typename T, typename U>
constexpr bool equalRange(T begin1, U begin2, U end2)
{
    if constexpr (triviallyOperatable<T, U>()) {
        return memcomp(begin1, begin2, static_cast<size_t>(end2 - begin2));
    }

    for (; begin2 != end2; ++begin1, ++begin2) {
        if (*address(begin1) != *address(begin2)) {
            return false;
        }
    }

    return true;
}

export template <typename T, typename U>
constexpr bool equalRange(T begin1, T end1, U begin2, U end2)
{
    if constexpr (triviallyOperatable<T, U>()) {
        size_t size = static_cast<size_t>(end2 - begin2);
        if (size != (end1 - begin1)) {
            return false;
        }

        return memcomp(begin1, begin2, size);
    }

    for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2) {
        if (*address(begin1) != *address(begin2)) {
            return false;
        }
    }

    return begin1 == end1 && begin2 == end2;
}

export template <typename T, typename U>
constexpr void fillRange(T begin, T end, U const& value)
{
    for (; begin != end; ++begin) {
        *address(begin) = value;
    }
}

export template <typename T, typename U>
constexpr T find(T first, T last, U const& value)
{
    for (; first != last; ++first) {
        if (*address(first) == value) {
            return first;
        }
    }

    return first;
}

export template <typename T, typename PredT>
constexpr T findIf(T first, T last, PredT pred)
{
    for (; first != last; ++first) {
        if (pred(*address(first))) {
            return first;
        }
    }

    return first;
}

export template <typename T, typename PredT>
constexpr T findIfNot(T first, T last, PredT pred)
{
    for (; first != last; ++first) {
        if (!pred(*address(first))) {
            return first;
        }
    }

    return first;
}

} // namespace mini::memory