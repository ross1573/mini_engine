#ifndef BUILTIN_MEMORY_H
#define BUILTIN_MEMORY_H

#include <memory>

// TODO: constexpr placement new operator is coming soon!
#define CONSTEXPR_CONSTRUCT_AT std::construct_at
#define CONSTEXPR_DESTRUCT_AT  std::destroy_at

// TODO: only compiler can do constexpr (de)allocate
#define CONSTEXPR_ALLOC(type, x)         std::allocator<type>{}.allocate(x)
#define CONSTEXPR_DEALLOC(type, x, size) std::allocator<type>{}.deallocate(x, size)

// TODO: MSVC throws an error when compiling with nothrow tag
#if MSVC // clang-format off
#  define NOTHROW_T
#else
#  define NOTHROW_T ,std::nothrow_t{}
#endif // clang-format on

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