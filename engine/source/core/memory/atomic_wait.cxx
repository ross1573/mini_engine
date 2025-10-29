module;

#include "builtin.h"

#if ARCH_ARM64
#  if PLATFORM_MACOS
#    define ATOMIC_STATE_ALIGN 128
#  else
#    define ATOMIC_STATE_ALIGN 64
#  endif
#elif ARCH_X86_64
#  define ATOMIC_STATE_ALIGN 64
#elif ARCH_X86
#  define ATOMIC_STATE_ALIGN 32
#else
#  define ATOMIC_STATE_ALIGN 64
#endif

export module mini.core:atomic_wait;

import :type;
import :memory;
import :atomic_base;
import :thread_base;

namespace mini::memory {

static constexpr SizeT PauseAttempt = 8;
static constexpr SizeT YieldAttempt = 32;

struct CORE_API alignas(ATOMIC_STATE_ALIGN) AtomicStateEntry {
public:
    AtomicContentionT wait;
    AtomicContentionT platform;

    constexpr AtomicStateEntry()
        : wait(0)
        , platform(0)
    {
    }
};

// Most of the user-side processors have less than 32 cores.
// So, we're assuming that 32 is big enough for our use case.
// This implementation might get some perf issues with server station setup.
static constexpr SizeT StateTableSize = 1 << 5;
static CORE_API AtomicStateEntry g_AtomicStateTable[StateTableSize];

inline AtomicStateEntry* AtomicStateEntryByLocation(void const volatile* loc) noexcept
{
    // TODO: do we need better hashing..?
    SizeT ptr = reinterpret_cast<SizeT>(&loc);
    SizeT buket = (ptr << 2) & (StateTableSize - 1);
    return &g_AtomicStateTable[buket];
}

template <TrivialT T>
inline bool AtomicLoadCompare(T const volatile* loc, T val, MemoryOrder order) noexcept
{
    T current = AtomicLoad(const_cast<T volatile*>(loc), order);
    return BUILTIN_MEMCMP(&current, &val, sizeof(T)) == 0;
}

template <TrivialT T, typename YieldT>
inline bool AtomicLoadCompareWithSpin(T const volatile* loc, T val, MemoryOrder order, byte attempt,
                                      YieldT yield) noexcept
{
    for (int i = 0; i < attempt; ++i) {
        if (!AtomicLoadCompare(loc, val, order)) {
            return false;
        }

        yield();
    }

    return true;
}

template <TrivialT T>
inline bool AtomicSpinWait(T const volatile* loc, T val, MemoryOrder order) noexcept
{
    if constexpr (AtomicAlwaysLockFree(sizeof(T))) {
        if (!AtomicLoadCompareWithSpin(loc, val, order, PauseAttempt, &thread::ThreadPause)) {
            return false;
        }
    }

    if (!AtomicLoadCompareWithSpin(loc, val, order, YieldAttempt, &thread::ThreadYield)) {
        return false;
    }

    return true;
}

inline CORE_API void AtomicPlatformWait(AtomicContentionT volatile* wait,
                                        AtomicContentionT const volatile* loc,
                                        AtomicContentionT value, SizeT size) noexcept
{
    AtomicFetchAdd(wait, static_cast<AtomicContentionT>(1), MemoryOrder::relaxed);
    AtomicThreadFence(MemoryOrder::sequential);
    AtomicWaitOnAddress(static_cast<void const volatile*>(loc), value, size);
    AtomicFetchSub(wait, static_cast<AtomicContentionT>(1), MemoryOrder::release);
}

inline CORE_API void AtomicPlatformNotify(AtomicContentionT const volatile* wait,
                                          AtomicContentionT const volatile* loc,
                                          SizeT size) noexcept
{
    AtomicContentionT waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyOnAddress(static_cast<void const volatile*>(loc), size);
    }
}

inline CORE_API void AtomicPlatformNotifyAll(AtomicContentionT const volatile* wait,
                                             AtomicContentionT const volatile* loc,
                                             SizeT size) noexcept
{
    AtomicContentionT waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyAllOnAddress(static_cast<void const volatile*>(loc), size);
    }
}

template <TrivialT T>
struct AtomicWaitableState {
public:
    T const volatile* pointer;
    AtomicStateEntry* entry;
    SizeT size;

    AtomicWaitableState(T const volatile* pointer)
        : pointer(pointer)
        , entry(AtomicStateEntryByLocation(pointer))
        , size(sizeof(AtomicContentionT))
    {
    }
};

template <TrivialT T>
struct AtomicNotifiable : AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    AtomicNotifiable(T const volatile* pointer)
        : Base(pointer)
    {
    }

    void Notify()
    {
        AtomicFetchAdd(&Base::entry->platform, AtomicContentionT(1), MemoryOrder::sequential);
        AtomicPlatformNotify(&Base::entry->wait, &Base::entry->platform, Base::size);
    }

    void NotifyAll()
    {
        AtomicFetchAdd(&Base::entry->platform, AtomicContentionT(1), MemoryOrder::sequential);
        AtomicPlatformNotifyAll(&Base::entry->wait, &Base::entry->platform, Base::size);
    }
};

template <TrivialT T>
    requires AtomicWaitableT<T>::value
struct AtomicNotifiable<T> : AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    AtomicNotifiable(T const volatile* pointer)
        : Base(pointer)
    {
    }

    AtomicContentionT const volatile* ContentionAddress()
    {
        return reinterpret_cast<AtomicContentionT const volatile*>(Base::pointer);
    }

    void Notify() { AtomicPlatformNotify(&Base::entry->wait, ContentionAddress(), Base::size); }
    void NotifyAll() { AtomicPlatformNotify(&Base::entry->wait, ContentionAddress(), Base::size); }
};

template <TrivialT T>
struct AtomicWaitable : public AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    T old;
    MemoryOrder order;

    AtomicWaitable(T const volatile* pointer, T old, MemoryOrder order)
        : Base(pointer)
        , old(old)
        , order(order)
    {
    }

    void Wait()
    {
        for (; AtomicSpinWait(Base::pointer, old, order);) {
            AtomicContentionT* platform = &Base::entry->platform;
            AtomicContentionT contention = AtomicLoad(platform, MemoryOrder::acquire);

            // since spin-wait ends with yield, we should have this one last load compare
            // this is to avoid ABA problem as mush as possible
            if (!AtomicLoadCompare(Base::pointer, old, order)) {
                return;
            }

            AtomicPlatformWait(&Base::entry->wait, platform, contention, Base::size);
        }
    }
};

template <TrivialT T>
    requires AtomicWaitableT<T>::value
struct AtomicWaitable<T> : public AtomicWaitableState<T> {
public:
    typedef AtomicWaitableT<T>::Type WaitableT;
    typedef AtomicWaitableState<T> Base;

    T old;
    MemoryOrder order;

    AtomicWaitable(T const volatile* pointer, T old, MemoryOrder order)
        : Base(pointer)
        , old(old)
        , order(order)
    {
    }

    void Wait()
    {
        for (; AtomicSpinWait(Base::pointer, old, order);) {
            WaitableT waitable = BitCast<WaitableT>(old);
            AtomicContentionT const volatile* ptr =
                reinterpret_cast<AtomicContentionT const volatile*>(Base::pointer);

            AtomicPlatformWait(&Base::entry->wait, ptr, waitable, Base::size);
        }
    }
};

template <TrivialT T>
inline void AtomicWait(T const volatile* loc, T old, MemoryOrder order) noexcept
{
    AtomicWaitable(loc, old, order).Wait();
}

template <TrivialT T>
inline void AtomicNotify(T const volatile* loc) noexcept
{
    AtomicNotifiable(loc).Notify();
}

template <TrivialT T>
inline void AtomicNotifyAll(T const volatile* loc) noexcept
{
    AtomicNotifiable(loc).NotifyAll();
}

} // namespace mini::memory