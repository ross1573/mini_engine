export module mini.core:mutex;

import :type;
import :mutex_platform;

namespace mini {

export class CORE_API Mutex {
private:
    typedef PlatformMutex MutexT;

    MutexT m_mutex;

public:
    Mutex() noexcept { MutexInitialize(m_mutex); }
    ~Mutex() noexcept = default;

    void Lock() { MutexLock(m_mutex); }
    void Unlock() { MutexUnlock(m_mutex); }
    bool TryLock() { return MutexTryLock(m_mutex); }

    MutexT* GetNativeHandle() noexcept { return &m_mutex; }

private:
    Mutex(Mutex const&) = delete;
    Mutex& operator=(Mutex const&) = delete;
};

export class CORE_API RecursiveMutex {
private:
    typedef PlatformRecursiveMutex MutexT;

    MutexT m_mutex;

public:
    RecursiveMutex() { RecursiveMutexInitialize(m_mutex); }
    ~RecursiveMutex() noexcept = default;

    void Lock() { RecursiveMutexLock(m_mutex); }
    void Unlock() { RecursiveMutexUnlock(m_mutex); }
    bool TryLock() { return RecursiveMutexTryLock(m_mutex); }

    MutexT* GetNativeHandle() noexcept { return &m_mutex; }

private:
    RecursiveMutex(RecursiveMutex const&) = delete;
    RecursiveMutex& operator=(RecursiveMutex const&) = delete;
};

} // namespace mini