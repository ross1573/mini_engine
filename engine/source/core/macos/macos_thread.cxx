module;

#include <pthread.h>
#include <sched.h>

#if CLANG || APPLE_CLANG
#  if ARCH_ARM64
#    define PAUSE() asm volatile("isb")
#  elif ARCH_X86_64 || ARCH_X86
#    define PAUSE() __builtin_ia32_pause();
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
    VERIFY(sched_yield() == 0, "failed to yield this thread");
}

inline void ThreadPause()
{
    PAUSE();
}

} // namespace mini::thread