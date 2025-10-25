module;

#if ARCH_ARM64
#  if PLATFORM_MACOS
#    define ATOMIC_STATE_ALIGN 128
#  else
#    define ATOMIC_STATE_ALIGN 64
#  endif
#elif ARCH_AMD64
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

static constexpr SizeT PauseAttempt = 2;
static constexpr SizeT YieldAttempt = 4;

template <TrivialT T>
inline bool AtomicLoadCompare(T const volatile* loc, T val, MemoryOrder order) noexcept
{
    T current = AtomicLoad(const_cast<T volatile*>(loc), order);
    return !MemCompare(&current, &val, 1);
}

template <TrivialT T, typename YieldFn>
inline bool AtomicLoadCompare(T const volatile* loc, T val, MemoryOrder order, byte attempt,
                              YieldFn yieldFn) noexcept
{
    for (int i = 0; i < attempt; ++i) {
        if (AtomicLoadCompare(loc, val, order)) {
            return true;
        }

        yieldFn();
    }

    return false;
}

template <TrivialT T>
inline bool AtomicSpinWait(T const volatile* loc, T val, MemoryOrder order) noexcept
{
    // hot section before entering kernal
    if (AtomicLoadCompare(loc, val, order, PauseAttempt, &thread::ThreadPause)) return true;
    if (AtomicLoadCompare(loc, val, order, YieldAttempt, &thread::ThreadYield)) return true;

    // one last compare before syscall
    if (AtomicLoadCompare(loc, val, order)) {
        return true;
    }

    return false;
}

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

inline CORE_API void AtomicPlatformWait(AtomicContentionT volatile* wait,
                                        AtomicContentionT const volatile* loc,
                                        AtomicContentionT value, SizeT size) noexcept
{
    AtomicFetchAdd(wait, static_cast<AtomicContentionT>(1), MemoryOrder::relaxed);
    AtomicThreadFence(MemoryOrder::sequential);
    AtomicWaitOnAddress(static_cast<void const volatile*>(loc), value, size);
    AtomicFetchSub(wait, static_cast<AtomicContentionT>(1), MemoryOrder::release);
}

inline CORE_API void AtomicPlatformNotify(AtomicContentionT volatile* wait,
                                          AtomicContentionT const volatile* loc,
                                          SizeT size) noexcept
{
    AtomicContentionT waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyOnAddress(static_cast<void const volatile*>(loc), size);
    }
}

inline CORE_API void AtomicPlatformNotifyAll(AtomicContentionT volatile* wait,
                                             AtomicContentionT const volatile* loc,
                                             SizeT size) noexcept
{
    AtomicContentionT waiterCount = AtomicLoad(wait, MemoryOrder::sequential);
    if (waiterCount != 0) {
        AtomicNotifyAllOnAddress(static_cast<void const volatile*>(loc), size);
    }
}

template <TrivialT T>
struct AtomicNotifiableState {
public:
    AtomicStateEntry* entry;
    AtomicContentionT const volatile* pointer;
    SizeT size;

    AtomicNotifiableState(T const volatile* pointer)
        : entry(AtomicStateEntryByLocation(pointer))
        , pointer(entry->platform)
        , size(sizeof(AtomicContentionT))
    {
    }
};

template <TrivialT T>
    requires AtomicWaitableT<T>::value
struct AtomicNotifiableState<T> {
public:
    AtomicStateEntry* entry;
    AtomicContentionT const volatile* pointer;
    SizeT size;

    AtomicNotifiableState(T const volatile* pointer)
        : entry(AtomicStateEntryByLocation(pointer))
        , pointer(reinterpret_cast<AtomicContentionT const volatile*>(pointer))
        , size(sizeof(T))
    {
    }
};

template <TrivialT T>
struct AtomicWaitableState : public AtomicNotifiableState<T> {
public:
    T old;

    AtomicWaitableState(T const volatile* pointer, T old)
        : AtomicNotifiableState<T>(pointer)
        , old(old)
    {
    }

    AtomicContentionT ContentionValue() const noexcept
    {
        return AtomicLoad(&this->entry->platform, MemoryOrder::acquire);
    }
};

template <TrivialT T>
    requires AtomicWaitableT<T>::value
struct AtomicWaitableState<T> : public AtomicNotifiableState<T> {
public:
    using WaitableT = AtomicWaitableT<T>::Type;

    WaitableT waitable;
    T old;

    AtomicWaitableState(T const volatile* pointer, T old)
        : AtomicNotifiableState<T>(pointer)
        , waitable(BitCast<WaitableT>(old))
        , old(old)
    {
    }

    AtomicContentionT ContentionValue() const noexcept { return waitable; }
};

template <TrivialT T>
inline void AtomicWait(T const volatile* loc, T old, MemoryOrder order) noexcept
{
    AtomicWaitableState waitable(loc, old);

    for (;;) {
        if (AtomicSpinWait(loc, old, order)) {
            return;
        }

        AtomicPlatformWait(&waitable.entry->wait, waitable.pointer, waitable.ContentionValue(),
                           waitable.size);

        if constexpr (AtomicWaitableT<T>::value) {
            return;
        }

        if (AtomicLoadCompare(loc, old, order)) {
            return;
        }
    }
}

template <TrivialT T>
inline void AtomicNotify(T const volatile* loc) noexcept
{
    AtomicNotifiableState notifiable(loc);
    if constexpr (!AtomicWaitableT<T>::value) {
        AtomicFetchAdd(notifiable.entry->platform, 1, MemoryOrder::sequential);
    }

    AtomicPlatformNotify(&notifiable.entry->wait, notifiable.pointer, notifiable.size);
}

template <TrivialT T>
inline void AtomicNotifyAll(T const volatile* loc) noexcept
{
    AtomicNotifiableState notifiable(loc);
    if constexpr (!AtomicWaitableT<T>::value) {
        AtomicFetchAdd(notifiable.entry->platform, 1, MemoryOrder::sequential);
    }

    AtomicPlatformNotifyAll(&notifiable.entry->wait, notifiable.pointer, notifiable.size);
}

} // namespace mini::memory