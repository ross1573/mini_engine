#ifndef BUILTIN_MEMORY_H
#define BUILTIN_MEMORY_H

#include <memory>

#if MSVC
#  define MSVC_CONSTEXPR [[msvc::constexpr]]
#else
#  define MSVC_CONSTEXPR
#endif

// TODO: only msvc compiler can do constexpr (de)allocate on windows
//       msvc rejects the code if the function signature do not match (including namespace)
//       thoses assholes won't give a fuck about constant evaluation
//       hopefully c++26 might solve this issue, but until than we have to rely on std
#define CONSTEXPR_ALLOC(type, x)         std::allocator<type>{ }.allocate(x)
#define CONSTEXPR_DEALLOC(type, x, size) std::allocator<type>{ }.deallocate(x, size)

#if HAS_BUILTIN(__builtin_addressof) || MSVC
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