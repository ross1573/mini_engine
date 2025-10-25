export module mini.core:mutex;

import :type;
import :mutex_base;

namespace mini::thread {

export class CORE_API Mutex {
private:
    typedef PlatformMutex MutexT;

    MutexT m_Mutex;

public:
    constexpr Mutex() noexcept { MutexInitialize(m_Mutex); }
    constexpr ~Mutex() noexcept = default;

    void Lock() { MutexLock(m_Mutex); }
    void Unlock() { MutexUnlock(m_Mutex); }
    bool TryLock() { return MutexTryLock(m_Mutex); }

    MutexT* GetNativeHandle() noexcept { return &m_Mutex; }

private:
    Mutex(Mutex const&) = delete;
    Mutex& operator=(Mutex const&) = delete;
};

export class CORE_API RecursiveMutex {
private:
    typedef PlatformRecursiveMutex MutexT;

    MutexT m_Mutex;

public:
    RecursiveMutex() { RecursiveMutexInitialize(m_Mutex); }
    ~RecursiveMutex() noexcept = default;

    void Lock() { RecursiveMutexLock(m_Mutex); }
    void Unlock() { RecursiveMutexUnlock(m_Mutex); }
    bool TryLock() { return RecursiveMutexTryLock(m_Mutex); }

    MutexT* GetNativeHandle() noexcept { return &m_Mutex; }

private:
    RecursiveMutex(RecursiveMutex const&) = delete;
    RecursiveMutex& operator=(RecursiveMutex const&) = delete;
};

} // namespace mini::thread