module;

#if HAS_BUILTIN(__builtin_unreachable)
#  define BUILTIN_UNREACHABLE() __builtin_unreachable()
#elif MSVC
#  define BUILTIN_UNREACHABLE() __assume(false)
#else
#  define BUILTIN_UNREACHABLE()
#endif // HAS_BUILTIN(__builtin_unreachable)

export module mini.core:utility_operation;

import :type_traits;
import :type_concepts;

namespace mini {

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

export inline constexpr void Unreachable()
{
    BUILTIN_UNREACHABLE();
}

} // namespace mini