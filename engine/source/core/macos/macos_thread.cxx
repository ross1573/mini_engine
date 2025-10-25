module;

#include <pthread.h>
#include <sched.h>

export module mini.core:thread_base;

import :type;

namespace mini::thread {

inline void ThreadYield()
{
    VERIFY(sched_yield() == 0, "failed to yield this thread");
}

inline void ThreadPause()
{
    // TODO: implement with proper pause instruction
    ThreadYield();
}

} // namespace mini::thread