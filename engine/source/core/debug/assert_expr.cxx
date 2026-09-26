export module mini.core:assert_expr;

import :type;
import :numeric;

namespace mini::debug {

export template <typename T>
constexpr bool Evaluate(T const&) = deleted_function("unable to evaluate expression");

export template <ValidatableT T>
constexpr bool Evaluate(T const& arg) noexcept
{
    return arg.Valid();
}

export template <ValidatableT T>
constexpr bool Evaluate(T* arg) noexcept
{
    return arg != nullptr && arg->Valid();
}

export template <ExplicitlyConvertibleToT<bool> T>
constexpr bool Evaluate(T const& arg) noexcept
    requires(!ValidatableT<T>)
{
    return static_cast<bool>(arg);
}

} // namespace mini::debug