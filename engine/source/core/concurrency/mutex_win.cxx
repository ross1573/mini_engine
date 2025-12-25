module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

export module mini.core:mutex_platform;

import :type;

namespace mini {

using PlatformMutex = SRWLOCK;
using PlatformRecursiveMutex = CRITICAL_SECTION;

inline void MutexInitialize(PlatformMutex& mutex)
{
    InitializeSRWLock(&mutex);
}

inline void MutexLock(PlatformMutex& mutex)
{
    AcquireSRWLockExclusive(&mutex);
}

inline void MutexUnlock(PlatformMutex& mutex)
{
    ReleaseSRWLockExclusive(&mutex);
}

inline bool MutexTryLock(PlatformMutex& mutex)
{
    return TryAcquireSRWLockExclusive(&mutex);
}

inline void RecursiveMutexInitialize(PlatformRecursiveMutex& mutex)
{
    InitializeCriticalSection(&mutex);
}

inline void RecursiveMutexLock(PlatformRecursiveMutex& mutex)
{
    EnterCriticalSection(&mutex);
}

inline void RecursiveMutexUnlock(PlatformRecursiveMutex& mutex)
{
    LeaveCriticalSection(&mutex);
}

inline bool RecursiveMutexTryLock(PlatformRecursiveMutex& mutex)
{
    return TryEnterCriticalSection(&mutex);
}

inline void RecursiveMutexDestroy(PlatformRecursiveMutex& mutex)
{
    DeleteCriticalSection(&mutex);
}

} // namespace mini