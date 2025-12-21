module;

#include <type_traits>

export module mini.type:traits;

namespace mini {

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

export template <bool Cond, typename T, typename F>
using ConditionalT = std::conditional<Cond, T, F>::type;

export template <typename... Args>
using CommonT = std::common_type_t<Args...>;

} // namespace mini