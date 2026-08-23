export module mini.core:assert_expr;

import :type;
import :numeric;

namespace mini::debug {

export template <typename T>
inline constexpr bool EvaluateExpr(T) = deleted_function("unable to evaluate expression");

export template <typename T>
inline constexpr bool EvaluateExpr(T arg) noexcept
    requires ExplicitlyConvertibleToT<T, bool> && !IntegralT<T>
{
    return static_cast<bool>(arg);
}

} // namespace mini::debug