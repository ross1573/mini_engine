export module mini.core:algorithm_memory;

import :type;
import :memory_operation;

namespace mini::memory {

export template <typename T, typename U>
inline constexpr void CopyRange(T dest, U begin, U end)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        MemCopy(dest, begin, static_cast<SizeT>(end - begin));
        return;
    }

    for (; begin != end; ++begin, ++dest) {
        *ToAddress(dest) = *ToAddress(begin);
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
        *ToAddress(--dest) = *ToAddress(--end);
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
        *ToAddress(dest) = MoveArg(*ToAddress(begin));
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
        *ToAddress(--dest) = MoveArg(*ToAddress(--end));
    }
}

export template <typename T, typename U>
inline constexpr bool EqualRange(T begin1, U begin2, U end2)
{
    if constexpr (IsTriviallyOperatable<T, U>()) {
        return MemCompare(begin1, begin2, static_cast<SizeT>(end2 - begin2));
    }

    for (; begin2 != end2; ++begin1, ++begin2) {
        if (*ToAddress(begin1) != *ToAddress(begin2)) {
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
        if (*ToAddress(begin1) != *ToAddress(begin2)) {
            return false;
        }
    }

    return begin1 == end1 && begin2 == end2;
}

export template <typename T, typename U>
inline constexpr void FillRange(T begin, T end, U const& value)
{
    for (; begin != end; ++begin) {
        *ToAddress(begin) = value;
    }
}

export template <typename T, typename U>
inline constexpr T Find(T first, T last, U const& value)
{
    for (; first != last; ++first) {
        if (*ToAddress(first) == value) {
            return first;
        }
    }

    return first;
}

export template <typename T, typename PredT>
inline constexpr T FindIf(T first, T last, PredT pred)
{
    for (; first != last; ++first) {
        if (pred(*ToAddress(first))) {
            return first;
        }
    }

    return first;
}

export template <typename T, typename PredT>
inline constexpr T FindIfNot(T first, T last, PredT pred)
{
    for (; first != last; ++first) {
        if (!pred(*ToAddress(first))) {
            return first;
        }
    }

    return first;
}

} // namespace mini::memory