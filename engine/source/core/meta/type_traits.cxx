module;

#include <type_traits>

export module mini.core:type_traits;

template <typename T, T N>
struct IntegralConstant {
    using ValueT = T;
    static constexpr ValueT value = N;
    constexpr operator ValueT() const noexcept { return N; }
    constexpr ValueT operator()() const noexcept { return N; }
};

namespace mini {

export using TrueT = IntegralConstant<bool, true>;
export using FalseT = IntegralConstant<bool, false>;

export template <typename... Args>
struct TrueArgT : TrueT { };

export template <typename... Args>
struct FalseArgT : FalseT { };

export template <typename T>
using RemoveRefT = std::remove_reference<T>::type;

export template <typename T>
using RemovePtrT = std::remove_pointer<T>::type;

export template <typename T>
using RemoveConstT = std::remove_const<T>::type;

export template <typename T>
using RemoveVolatileT = std::remove_volatile<T>::type;

export template <typename T>
using RemoveConstVolatileT = std::remove_cv<T>::type;

export template <typename T>
using RemoveConstVolatileRefT = std::remove_cvref<T>::type;

export template <typename T>
using DecayT = std::decay<T>::type;

export template <bool ConditionN, typename T, typename F>
using ConditionalT = std::conditional<ConditionN, T, F>::type;

export template <typename... Args>
using CommonT = std::common_type<Args...>::type;

} // namespace mini