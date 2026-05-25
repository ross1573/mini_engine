#ifndef BUILTIN_MEMORY_H
#define BUILTIN_MEMORY_H

#include <memory>

#if MSVC
#  define PLACEMENT_NEW_CONSTEXPR [[msvc::constexpr]]
#else
#  define PLACEMENT_NEW_CONSTEXPR
#endif

// TODO: only compiler can do constexpr (de)allocate
#define CONSTEXPR_ALLOC(type, x)         std::allocator<type>{}.allocate(x)
#define CONSTEXPR_DEALLOC(type, x, size) std::allocator<type>{}.deallocate(x, size)

#if HAS_BUILTIN(__builtin_addressof)
#  define BUILTIN_ADDRESS_OF __builtin_addressof
#else
#  define BUILTIN_ADDRESS_OF std::addressof
#endif

#if HAS_BUILTIN(__builtin_operator_new) && HAS_BUILTIN(__builtin_operator_delete)
#  define BUILTIN_OPERATOR_NEW    __builtin_operator_new
#  define BUILTIN_OPERATOR_DELETE __builtin_operator_delete
#else
#  define BUILTIN_OPERATOR_NEW    ::operator new
#  define BUILTIN_OPERATOR_DELETE ::operator delete
#endif

#endif // BUILDIN_MEMORY_H