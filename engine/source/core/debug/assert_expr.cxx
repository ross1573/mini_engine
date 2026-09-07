export module mini.core:assert_expr;

import :type;
import :numeric;

namespace mini::debug {

export template <typename T>
inline constexpr bool EvaluateExpr(T const&) = deleted_function("unable to evaluate expression");

export template <ValidatableT T>
inline constexpr bool EvaluateExpr(T const& arg) noexcept
{
    return arg.Valid();
}

export template <ExplicitlyConvertibleToT<bool> T>
inline constexpr bool EvaluateExpr(T const& arg) noexcept
    requires(!ValidatableT<T>)
{
    return static_cast<bool>(arg);
}

} // namespace mini::debug