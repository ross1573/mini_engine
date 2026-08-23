export module mini.core:assert_expr;

import :type;

namespace mini::debug {

export template <typename T>
inline constexpr bool EvaluateExpr(T) = deleted_function("unable to evaluate expression");

export template <ExplicitlyConvertibleToT<bool> T>
inline constexpr bool EvaluateExpr(T arg) noexcept
{
    return static_cast<bool>(arg);
}

} // namespace mini::debug