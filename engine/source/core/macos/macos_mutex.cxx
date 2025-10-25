module;

#include <os/lock.h>
#include <pthread.h>

#if DEBUG
#  define DEBUG_UNFAIR_LOCK_OWNER(x)     os_unfair_lock_assert_owner(x)
#  define DEBUG_UNFAIR_LOCK_NOT_OWNER(x) os_unfair_lock_assert_not_owner(x);
#else
#  define DEBUG_UNFAIR_LOCK_OWNER(x)
#  define DEBUG_UNFAIR_LOCK_NOT_OWNER(x)
#endif

export module mini.core:mutex_base;

import :type;

namespace mini::thread {

using PlatformMutex = os_unfair_lock;
using PlatformRecursiveMutex = pthread_mutex_t;

inline void MutexInitialize(PlatformMutex& mutex)
{
    // this macro just zero initialize the mutex value
    mutex = OS_UNFAIR_LOCK_INIT;
}

inline void MutexLock(PlatformMutex& mutex)
{
    DEBUG_UNFAIR_LOCK_NOT_OWNER(&mutex);
    os_unfair_lock_lock(&mutex);
}

inline void MutexUnlock(PlatformMutex& mutex)
{
    DEBUG_UNFAIR_LOCK_OWNER(&mutex);
    os_unfair_lock_unlock(&mutex);
}

inline bool MutexTryLock(PlatformMutex& mutex)
{
    return os_unfair_lock_trylock(&mutex);
}

inline void RecursiveMutexInitialize(PlatformRecursiveMutex& mutex)
{
    pthread_mutexattr_t attr;
    int32 error = pthread_mutexattr_init(&attr);
    if (error) {
        goto init_error;
    }

    error = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (error) {
        pthread_mutexattr_destroy(&attr);
        goto init_error;
    }

    error = pthread_mutex_init(&mutex, &attr);
    if (error) {
        pthread_mutexattr_destroy(&attr);
        goto init_error;
    }

    error = pthread_mutexattr_destroy(&attr);
    if (error) {
        pthread_mutex_destroy(&mutex);
        goto init_error;
    }

    return;

init_error:
    ASSERT(error, "failed to initialize mutex");
}

inline void RecursiveMutexLock(PlatformRecursiveMutex& mutex)
{
    VERIFY(pthread_mutex_lock(&mutex) == 0, "failed to lock pthread_mutex");
}

inline void RecursiveMutexUnlock(PlatformRecursiveMutex& mutex)
{
    VERIFY(pthread_mutex_unlock(&mutex) == 0, "failed to unlock pthread_mutex");
}

inline bool RecursiveMutexTryLock(PlatformRecursiveMutex& mutex)
{
    return pthread_mutex_trylock(&mutex) == 0;
}

inline void RecursiveMutexDestroy(PlatformRecursiveMutex& mutex)
{
    VERIFY(pthread_mutex_destroy(&mutex) == 0, "failed to destroy pthread_mutex");
}

} // namespace mini::thread