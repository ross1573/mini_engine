module;

#include <compare>
#include <concepts>
#include <type_traits>

export module mini.core:type_concepts;

import :type_traits;

template <typename T, typename... Args>
struct AnyOfImpl {
    static constexpr bool value = (std::is_same<T, Args>::value || ...);
};

template <typename T>
struct AnyOfImpl<T> {
    static constexpr bool value = false;
};

namespace mini {

export template <typename T>
concept PtrT = std::is_pointer_v<T>;

export template <typename T>
concept RefT = std::is_reference_v<T>;

export template <typename T>
concept ArrT = std::is_array_v<T>;

export template <typename T>
concept NonPtrT = !PtrT<T>;

export template <typename T>
concept NonRefT = !RefT<T>;

export template <typename T>
concept NonArrT = !ArrT<T>;

export template <typename T>
concept ValueT = NonPtrT<T> && NonRefT<T> && !std::is_array_v<T>;

export template <typename Base, typename Derived>
concept BaseOfT = std::is_base_of_v<Base, Derived>;

export template <typename Derived, typename Base>
concept DerivedFromT = std::is_base_of_v<Base, Derived>;

export template <typename T, typename U>
concept SameAsT = std::same_as<T, U>;

export template <typename T, typename... Args>
concept AnyOfT = AnyOfImpl<T, Args...>::value;

export template <typename From, typename To>
concept ConvertibleToT = std::convertible_to<From, To>;

export template <typename From, typename To>
concept PtrConvertibleToT = NonRefT<From> && NonRefT<To> && ConvertibleToT<From*, To*>;

export template <typename T, typename... Args>
concept ConstructibleFromT = std::constructible_from<T, Args...>;

export template <typename T, typename... Args>
concept NoThrowConstructibleFromT = std::is_nothrow_constructible_v<T, Args...>;

export template <typename T>
concept MoveConstructibleT = std::move_constructible<T>;

export template <typename T>
concept CopyConstructibleT = std::copy_constructible<T>;

export template <typename T, typename U>
concept AssignableFromT = std::assignable_from<T, U>;

export template <typename T, typename U>
concept NoThrowAssignableFromT = AssignableFromT<T, U> && std::is_nothrow_assignable_v<T, U>;

export template <typename T>
concept DestructibleT = std::destructible<T>;

export template <typename T>
concept MovableT = std::movable<T>;

export template <typename T>
concept CopyableT = std::copyable<T>;

export template <typename T>
concept NoThrowMovableT = MovableT<T> && std::is_nothrow_move_constructible_v<T> &&
                          std::is_nothrow_move_assignable_v<T>;

export template <typename T>
concept NoThrowCopyableT = NoThrowMovableT<T> && std::is_nothrow_copy_constructible_v<T> &&
                           std::is_nothrow_copy_assignable_v<T>;

export template <typename T>
concept TrivialT = std::is_nothrow_default_constructible_v<T> && std::is_trivially_copyable_v<T> &&
                   NoThrowCopyableT<T>;

export template <typename T, typename... Args>
concept CallableT = std::is_invocable_v<T, Args...>;

export template <typename T, typename... Args>
concept NoThrowCallableT = std::is_nothrow_invocable_v<T, Args...>;

export template <typename T>
concept EqualityComparableT = std::equality_comparable<T>;

export template <typename T, typename U>
concept EqualityComparableWithT = std::equality_comparable_with<T, U>;

export template <typename T>
concept ComparableT = std::totally_ordered<T>;

export template <typename T, typename U>
concept ComparableWithT = std::totally_ordered_with<T, U>;

export template <typename T>
concept ThreeWayComparableT = std::three_way_comparable<T>;

export template <typename T, typename U>
concept ThreeWayComparableWithT = std::three_way_comparable_with<T, U>;

} // namespace mini

namespace mini {

export template <typename T>
concept IntT = std::integral<T>;

export template <typename T>
concept FloatT = std::floating_point<T>;

export template <typename T>
concept ArithmeticT = std::is_arithmetic_v<T>;

export template <typename T>
concept CharT = AnyOfT<RemoveConstVolatileT<T>, char, wchar_t, char8_t, char16_t, char32_t>;

} // namespace mini