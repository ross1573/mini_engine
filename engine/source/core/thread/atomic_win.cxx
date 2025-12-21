module;

#include <intrin.h>

#if MSVC
#  if ARCH_ARM64
#    define PAUSE() __isb(_ARM64_BARRIER_SY)
#  elif ARCH_X86_64 || ARCH_X86
#    define PAUSE() _mm_pause()
#  else
#    define PAUSE() __nop()
#  endif
#elif CLANG || GNUC
#  if ARCH_ARM64
#    define PAUSE() asm volatile("isb")
#  elif ARCH_X86_64 || ARCH_X86
#    define PAUSE() __builtin_ia32_pause()
#  else
#    define PAUSE() asm volatile("", , , "memory")
#  endif
#else
#  error "unsupported compiler"
#endif

export module mini.thread:atomic_platform_wait;

import mini.type;

namespace mini {

using AtomicContention = AtomicContentionValueT;

template <typename T>
struct AtomicWaitableT : FalseT {};

template <typename T>
    requires(sizeof(T) == 1)
struct AtomicWaitableT<T> : TrueT {
    typedef uint8 Type;
};

template <typename T>
    requires(sizeof(T) == 2)
struct AtomicWaitableT<T> : TrueT {
    typedef uint16 Type;
};

template <typename T>
    requires(sizeof(T) == 4)
struct AtomicWaitableT<T> : TrueT {
    typedef uint32 Type;
};

template <typename T>
    requires(sizeof(T) == 8)
struct AtomicWaitableT<T> : TrueT {
    typedef uint64 Type;
};

CORE_API inline void AtomicRelax()
{
    PAUSE();
}

CORE_API void WaitOnAddress(AtomicContention const volatile*, AtomicContention, SizeT);
CORE_API void NotifyOnAddress(AtomicContention const volatile*, SizeT);
CORE_API void NotifyAllOnAddress(AtomicContention const volatile*, SizeT);

} // namespace mini