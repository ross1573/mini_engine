export module mini.core:utility;

import :type;

export namespace mini
{

template <typename T>
inline constexpr T&& ForwardArg(RemoveRefT<T>& arg) noexcept
{
    return static_cast<T&&>(arg);
};

template <typename T>
inline constexpr T&& ForwardArg(RemoveRefT<T>&& arg) noexcept
{
    return static_cast<T&&>(arg);
};

template <typename T>
inline constexpr RemoveRefT<T>&& MoveArg(T&& arg) noexcept
{
    return static_cast<RemoveRefT<T>&&>(arg);
}

template <typename T>
inline constexpr void Swap(T& _1, T& _2) noexcept(NoThrowMovableT<T>)
{
    T tmp = MoveArg(_1);
    _1 = MoveArg(_2);
    _2 = MoveArg(tmp);
}

template <typename T, typename U = T>
inline constexpr T Exchange(T& v, U&& n)
{
    T old = MoveArg(v);
    v = ForwardArg<U>(n);
    return old;
}

} // namespace mini