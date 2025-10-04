module;

#include <initializer_list>

export module mini.core:utility;

import :type;

namespace mini {

export template <typename T>
using InitializerList = std::initializer_list<T>;

export template <typename T>
inline constexpr T&& ForwardArg(RemoveRefT<T>& arg) noexcept
{
    return static_cast<T&&>(arg);
};

export template <typename T>
inline constexpr T&& ForwardArg(RemoveRefT<T>&& arg) noexcept
{
    return static_cast<T&&>(arg);
};

export template <typename T>
inline constexpr RemoveRefT<T>&& MoveArg(T&& arg) noexcept
{
    return static_cast<RemoveRefT<T>&&>(arg);
}

export template <typename T>
inline constexpr void Swap(T& _1, T& _2) noexcept(NoThrowMovableT<T>)
{
    T tmp = MoveArg(_1);
    _1 = MoveArg(_2);
    _2 = MoveArg(tmp);
}

export template <typename T, typename U = T>
inline constexpr T Exchange(T& v, U&& n)
    requires ConvertibleToT<U, T>
{
    T old = MoveArg(v);
    v = static_cast<T>(ForwardArg<U>(n));
    return old;
}

} // namespace mini