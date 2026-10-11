export module mini.core:assert_expr;

import :type;
import :numeric;

namespace mini::debug {

export template <typename T>
constexpr bool evaluate(T const&) = deleted_function("unable to evaluate expression");

export template <ValidatableT T>
constexpr bool evaluate(T const& arg) noexcept
{
    return arg.valid();
}

export template <ValidatableT T>
constexpr bool evaluate(T* arg) noexcept
{
    return arg != nullptr && arg->valid();
}

export template <ExplicitlyConvertibleToT<bool> T>
constexpr bool evaluate(T const& arg) noexcept
    requires(!ValidatableT<T>)
{
    return static_cast<bool>(arg);
}

} // namespace mini::debug