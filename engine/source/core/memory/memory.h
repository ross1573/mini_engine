#ifndef BUILTIN_MEMORY_H
#define BUILTIN_MEMORY_H

#include <memory>

// TODO: constexpr placement new operator is coming soon!
#define CONSTEXPR_CONSTRUCT_AT std::construct_at
#define CONSTEXPR_DESTRUCT_AT  std::destroy_at

#if HAS_BUILTIN(__builtin_addressof)
#  define BUILTIN_ADDRESS_OF(x) __builtin_addressof(x)
#else
#  define BUILTIN_ADDRESS_OF(x) std::addressof(x)
#endif

#endif // BUILDIN_MEMORY_H