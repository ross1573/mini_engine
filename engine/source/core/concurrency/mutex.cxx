export module mini.core:mutex;

import :type;
import :mutex_platform;

namespace mini {

export class CORE_API Mutex {
private:
    typedef PlatformMutex MutexT;

    MutexT m_mutex;

public:
    Mutex() noexcept { mutexInitialize(m_mutex); }
    ~Mutex() noexcept = default;

    void lock() { mutexLock(m_mutex); }
    void unlock() { mutexUnlock(m_mutex); }
    bool tryLock() { return mutexTryLock(m_mutex); }

    MutexT* nativeHandle() noexcept { return &m_mutex; }

public:
    Mutex(Mutex const&) = delete;
    Mutex& operator=(Mutex const&) = delete;
};

export class CORE_API RecursiveMutex {
private:
    typedef PlatformRecursiveMutex MutexT;

    MutexT m_mutex;

public:
    RecursiveMutex() { recursiveMutexInitialize(m_mutex); }
    ~RecursiveMutex() noexcept = default;

    void lock() { recursiveMutexLock(m_mutex); }
    void unlock() { recursiveMutexUnlock(m_mutex); }
    bool tryLock() { return recursiveMutexTryLock(m_mutex); }

    MutexT* nativeHandle() noexcept { return &m_mutex; }

public:
    RecursiveMutex(RecursiveMutex const&) = delete;
    RecursiveMutex& operator=(RecursiveMutex const&) = delete;
};

} // namespace mini