module;

#if CLANG // TODO: clang some how manages to skip this diagnostic
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Watomic-alignment"
#endif

export module mini.core:atomic_wait;

import :type;
import :memory_operation;
import :duration;
import :clock;
import :atomic_platform;
import :atomic_platform_wait;

namespace mini {

constexpr SizeT spinWaitCount = 64;

template <TrivialT T>
inline bool AtomicLoadCompare(T const volatile* loc, T val, int32 order) noexcept
{
    T current;
    __atomic_load(loc, memory::AddressOf(current), order);
    return memory::MemCompare(memory::AddressOf(current), memory::AddressOf(val), sizeof(T)) == 0;
}

template <TrivialT T>
inline bool AtomicSpinWaitLoop(T const volatile* loc, T val, int32 order) noexcept
{
    Clock::TimePoint start = Clock::Now();
    for (;;) {
        for (SizeT i = 0; i < spinWaitCount; ++i) {
            AtomicRelax();

            if (!AtomicLoadCompare(loc, val, order)) {
                return false;
            }
        }

        Clock::TimePoint tp = Clock::Now();
        if ((tp - start) > MicroSeconds(4)) {
            break;
        }
    }

    return true;
}

template <TrivialT T>
inline bool AtomicSpinWait(T const volatile* loc, T val, int32 order) noexcept
{
    if (!AtomicLoadCompare(loc, val, order)) {
        return false;
    }

    return AtomicSpinWaitLoop(loc, val, order);
}

inline CORE_API void AtomicPlatformWait(AtomicContention volatile* waiter,
                                        AtomicContention const volatile* platform,
                                        AtomicContention value, SizeT size) noexcept
{
    __atomic_fetch_add(waiter, AtomicContention(1), __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    WaitOnAddress(platform, value, size);
    __atomic_fetch_sub(waiter, AtomicContention(1), __ATOMIC_RELEASE);
}

inline CORE_API void AtomicPlatformNotify(AtomicContention const volatile* waiter,
                                          AtomicContention const volatile* platform,
                                          SizeT size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        NotifyOnAddress(platform, size);
    }
}

inline CORE_API void AtomicPlatformNotifyAll(AtomicContention const volatile* waiter,
                                             AtomicContention const volatile* platform,
                                             SizeT size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        NotifyAllOnAddress(platform, size);
    }
}

struct CORE_API alignas(__ATOMIC_INTERFERENCE_SIZE) AtomicEntry {
public:
    AtomicContention waiter;
    AtomicContention platform;

    constexpr AtomicEntry()
        : waiter(0)
        , platform(0)
    {
    }
};

// Most of the user-side processors have less than 32 cores.
// So, we're assuming that 64 is big enough for our use case.
// This implementation might get some perf issues with server station setup.
constexpr SizeT contentionTableSize = 1 << 6;
CORE_API AtomicEntry g_AtomicContentionTable[contentionTableSize];

template <typename T>
struct AtomicWaitableContext {
public:
    T const volatile* pointer;
    AtomicEntry* entry;
    SizeT size;

    AtomicWaitableContext(T const volatile* pointer)
        : pointer(pointer)
    {
        SizeT intptr = reinterpret_cast<SizeT>(pointer);
        SizeT hash = intptr >> 6;
        hash ^= hash >> 16;
        hash &= (contentionTableSize - 1);
        entry = &g_AtomicContentionTable[hash];

        if constexpr (AtomicWaitableT<T>::value) {
            size = sizeof(T);
        } else {
            size = sizeof(AtomicContention);
        }
    }
};

} // namespace mini

export template <typename T>
inline void __atomic_notify_one(T const volatile* pointer) noexcept
    requires(!mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    constexpr mini::AtomicContention contention = 1;

    __atomic_fetch_add(&context.entry->platform, contention, __ATOMIC_SEQ_CST);
    mini::AtomicPlatformNotify(&context.entry->waiter, &context.entry->platform, context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
    requires(!mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    constexpr mini::AtomicContention contention = 1;

    __atomic_fetch_add(&context.entry->platform, contention, __ATOMIC_SEQ_CST);
    mini::AtomicPlatformNotifyAll(&context.entry->waiter, &context.entry->platform, context.size);
}

export template <typename T>
inline void __atomic_notify_one(T const volatile* pointer) noexcept
    requires(mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention const volatile* loc =
        reinterpret_cast<mini::AtomicContention const volatile*>(context.pointer);

    mini::AtomicPlatformNotify(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention const volatile* loc =
        reinterpret_cast<mini::AtomicContention const volatile*>(context.pointer);

    mini::AtomicPlatformNotifyAll(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_wait(T const volatile* pointer, T old, int order) noexcept
    requires(!mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention contention;
    __atomic_load(&context.entry->platform, &contention, __ATOMIC_ACQUIRE);

    while (mini::AtomicLoadCompare(context.pointer, old, static_cast<mini::int32>(order))) {
        // since load compare of non atomic values cannot be fetched atomically,
        // notify can occur while load comparing the value.
        // so we should compare the monitor value to check it has been changed.
        // if so, we should not enter wait or else it will cause a dead lock.
        if (mini::AtomicLoadCompare(&context.entry->platform, contention, __ATOMIC_ACQUIRE)) {
            mini::AtomicPlatformWait(&context.entry->waiter, &context.entry->platform, contention,
                                     context.size);
        }
    }
}

export template <typename T>
inline void __atomic_wait(T const volatile* pointer, T old, int order) noexcept
    requires(mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention const volatile* loc =
        reinterpret_cast<mini::AtomicContention const volatile*>(context.pointer);
    typename mini::AtomicWaitableT<T>::Type* waitable =
        reinterpret_cast<mini::AtomicWaitableT<T>::Type*>(mini::memory::AddressOf(old));

    if (mini::AtomicSpinWait(context.pointer, old, static_cast<mini::int32>(order))) {
        mini::AtomicPlatformWait(&context.entry->waiter, loc, *waitable, context.size);
    }
}