module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

export module mini.core:thread_base;

import :type;

namespace mini::thread {

inline void ThreadYield()
{
    SwitchToThread();
}

inline void ThreadRelax()
{
    PAUSE();
}

} // namespace mini::thread