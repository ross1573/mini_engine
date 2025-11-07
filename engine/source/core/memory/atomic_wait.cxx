module;

#include "cstring.h"

export module mini.core:atomic_wait;

import :type;
import :memory;
import :atomic_base;
import :atomic_impl;
import :thread_base;

namespace mini::memory {

constexpr SizeT spinWaitCount = 64;

template <TrivialT T>
inline bool AtomicLoadCompare(AtomicBase<T> const volatile* loc, T val, MemoryOrder order) noexcept
{
    T current = AtomicLoad(const_cast<AtomicBase<T> volatile*>(loc), order);
    return BUILTIN_MEMCMP(&current, &val, sizeof(T)) == 0;
}

template <TrivialT T>
inline bool AtomicSpinWait(AtomicBase<T> const volatile* loc, T val, MemoryOrder order) noexcept
{
    // TODO: fix it with duration, just like libc++
    for (SizeT i = 0; i < spinWaitCount; ++i) {
        if (!AtomicLoadCompare(loc, val, order)) {
            return false;
        }

        mini::thread::ThreadRelax();
    }

    return true;
}

inline CORE_API void AtomicPlatformWait(AtomicContention volatile* wait,
                                        AtomicContention const volatile* loc,
                                        AtomicContention::Value value, SizeT size) noexcept
{
    AtomicFetchAdd(wait, AtomicContention::Value(1), MemoryOrder::relaxed);
    AtomicThreadFence(MemoryOrder::sequential);
    AtomicWaitOnAddress(loc, value, size);
    AtomicFetchSub(wait, AtomicContention::Value(1), MemoryOrder::release);
}

inline CORE_API void AtomicPlatformNotify(AtomicContention const volatile* wait,
                                          AtomicContention const volatile* loc, SizeT size) noexcept
{
    AtomicContention::Value waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyOnAddress(loc, size);
    }
}

inline CORE_API void AtomicPlatformNotifyAll(AtomicContention const volatile* wait,
                                             AtomicContention const volatile* loc,
                                             SizeT size) noexcept
{
    AtomicContention::Value waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyAllOnAddress(loc, size);
    }
}

struct CORE_API alignas(AtomicInterferenceSize) AtomicEntry {
public:
    AtomicContention wait;
    AtomicContention platform;

    constexpr AtomicEntry()
        : wait(0)
        , platform(0)
    {
    }
};

// Most of the user-side processors have less than 32 cores.
// So, we're assuming that 32 is big enough for our use case.
// This implementation might get some perf issues with server station setup.
constexpr SizeT contentionTableSize = 1 << 6;
CORE_API AtomicEntry g_AtomicContentionTable[contentionTableSize];

template <typename T>
struct AtomicWaitableState {
public:
    AtomicBase<T> const volatile* pointer;
    AtomicEntry* entry;
    SizeT size;

    AtomicWaitableState(AtomicBase<T> const volatile* pointer)
        : pointer(pointer)
        , size(sizeof(AtomicContention))
    {
        SizeT bucket = AtomicAddressHash<T, contentionTableSize>(&pointer->value);
        entry = reinterpret_cast<AtomicEntry*>(&g_AtomicContentionTable[bucket]);
    }
};

template <typename T>
struct AtomicNotifiable : AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    AtomicNotifiable(AtomicBase<T> const volatile* pointer)
        : Base(pointer)
    {
    }

    void Notify()
    {
        AtomicContention::Value val(1);
        AtomicFetchAdd(&Base::entry->platform, val, MemoryOrder::sequential);
        AtomicPlatformNotify(&Base::entry->wait, &Base::entry->platform, Base::size);
    }

    void NotifyAll()
    {
        AtomicContention::Value val(1);
        AtomicFetchAdd(&Base::entry->platform, val, MemoryOrder::sequential);
        AtomicPlatformNotifyAll(&Base::entry->wait, &Base::entry->platform, Base::size);
    }
};

template <typename T>
    requires AtomicWaitableT<T>::value
struct AtomicNotifiable<T> : AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    AtomicNotifiable(AtomicBase<T> const volatile* pointer)
        : Base(pointer)
    {
    }

    AtomicContention const volatile* ContentionAddress()
    {
        return reinterpret_cast<AtomicContention const volatile*>(Base::pointer);
    }

    void Notify()
    {
        AtomicContention const volatile* loc = ContentionAddress();
        AtomicPlatformNotify(&Base::entry->wait, loc, Base::size);
    }

    void NotifyAll()
    {
        AtomicContention const volatile* loc = ContentionAddress();
        AtomicPlatformNotify(&Base::entry->wait, loc, Base::size);
    }
};

template <typename T>
struct AtomicWaitable : public AtomicWaitableState<T> {
public:
    typedef AtomicWaitableState<T> Base;

    T old;
    MemoryOrder order;

    AtomicWaitable(AtomicBase<T> const volatile* pointer, T old, MemoryOrder order)
        : Base(pointer)
        , old(old)
        , order(order)
    {
    }

    void Wait()
    {
        for (; AtomicSpinWait(Base::pointer, old, order);) {
            AtomicContention* platform = &Base::entry->platform;
            AtomicContention::Value contention = AtomicLoad(platform, MemoryOrder::acquire);

            if (AtomicLoadCompare(Base::pointer, old, order)) {
                AtomicPlatformWait(&Base::entry->wait, platform, contention, Base::size);
            }
        }
    }
};

template <typename T>
    requires AtomicWaitableT<T>::value
struct AtomicWaitable<T> : public AtomicWaitableState<T> {
public:
    typedef AtomicWaitableT<T>::Type WaitableT;
    typedef AtomicWaitableState<T> Base;

    T old;
    MemoryOrder order;

    AtomicWaitable(AtomicBase<T> const volatile* pointer, T old, MemoryOrder order)
        : Base(pointer)
        , old(old)
        , order(order)
    {
    }

    void Wait()
    {
        if (!AtomicSpinWait(Base::pointer, old, order)) {
            return;
        }

        WaitableT waitable = BitCast<WaitableT>(old);
        AtomicContention const volatile* ptr =
            reinterpret_cast<AtomicContention const volatile*>(Base::pointer);

        if (AtomicLoadCompare(Base::pointer, old, order)) {
            AtomicPlatformWait(&Base::entry->wait, ptr, waitable, Base::size);
        }
    }
};

template <typename T>
inline void AtomicWait(AtomicBase<T> const volatile* loc, T old, MemoryOrder order) noexcept
{
    AtomicWaitable(loc, old, order).Wait();
}

template <typename T>
inline void AtomicNotify(AtomicBase<T> const volatile* loc) noexcept
{
    AtomicNotifiable(loc).Notify();
}

template <typename T>
inline void AtomicNotifyAll(AtomicBase<T> const volatile* loc) noexcept
{
    AtomicNotifiable(loc).NotifyAll();
}

} // namespace mini::memory