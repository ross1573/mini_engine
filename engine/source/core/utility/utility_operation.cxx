export module mini.core:utility_operation;

import :type_traits;
import :type_concepts;

namespace mini {

export template <typename T>
constexpr T&& forwardCast(RemoveRefT<T>& arg) noexcept
{
    return static_cast<T&&>(arg);
};

export template <typename T>
constexpr T&& forwardCast(RemoveRefT<T>&& arg) noexcept
{
    return static_cast<T&&>(arg);
};

export template <typename T>
constexpr RemoveRefT<T>&& moveCast(T&& arg) noexcept
{
    return static_cast<RemoveRefT<T>&&>(arg);
}

export template <typename T>
constexpr void swap(T& lhs, T& rhs) noexcept(NoThrowMovableT<T>)
{
    T tmp(moveCast(lhs));
    lhs = moveCast(rhs);
    rhs = moveCast(tmp);
}

export template <typename T, typename U = T>
constexpr T exchange(T& val, U&& newVal)
    requires ConvertibleToT<U, T>
{
    T old(moveCast(val));
    val = static_cast<T>(forwardCast<U>(newVal));
    return old;
}

export constexpr void unreachable()
{
    BUILTIN_UNREACHABLE();
}

} // namespace mini