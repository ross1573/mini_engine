module;

#include "win_include.h"

export module mini.core:mutex_platform;

import :type;

namespace mini {

using PlatformMutex = SRWLOCK;
using PlatformRecursiveMutex = CRITICAL_SECTION;

inline void mutexInitialize(PlatformMutex& mutex)
{
    InitializeSRWLock(&mutex);
}

inline void mutexLock(PlatformMutex& mutex)
{
    AcquireSRWLockExclusive(&mutex);
}

inline void mutexUnlock(PlatformMutex& mutex)
{
    ReleaseSRWLockExclusive(&mutex);
}

inline bool mutexTryLock(PlatformMutex& mutex)
{
    return TryAcquireSRWLockExclusive(&mutex);
}

inline void recursiveMutexInitialize(PlatformRecursiveMutex& mutex)
{
    InitializeCriticalSection(&mutex);
}

inline void recursiveMutexLock(PlatformRecursiveMutex& mutex)
{
    EnterCriticalSection(&mutex);
}

inline void recursiveMutexUnlock(PlatformRecursiveMutex& mutex)
{
    LeaveCriticalSection(&mutex);
}

inline bool recursiveMutexTryLock(PlatformRecursiveMutex& mutex)
{
    return TryEnterCriticalSection(&mutex);
}

inline void recursiveMutexDestroy(PlatformRecursiveMutex& mutex)
{
    DeleteCriticalSection(&mutex);
}

} // namespace mini