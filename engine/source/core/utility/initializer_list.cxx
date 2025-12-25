module;

#include <initializer_list>

export module mini.core:initializer_list;

namespace mini {

// std::initializer_list is usally hard coded inside the compiler.
// its just impossible to mimic how compiler handles it
export template <typename T>
using InitializerList = std::initializer_list<T>;

} // namespace mini