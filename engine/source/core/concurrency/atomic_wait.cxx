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

constexpr size_t spinWaitCount = 64;

template <TrivialT T>
inline bool atomicLoadCompare(T const volatile* loc, T val, int32 order) noexcept
{
    T current;
    __atomic_load(loc, addressof(current), order);
    return memory::memcomp(addressof(current), addressof(val), sizeof(T)) == 0;
}

template <TrivialT T>
inline bool atomicSpinWaitLoop(T const volatile* loc, T val, int32 order) noexcept
{
    Clock::TimePoint start = Clock::now();
    for (;;) {
        for (size_t i = 0; i < spinWaitCount; ++i) {
            atomicRelax();

            if (!atomicLoadCompare(loc, val, order)) {
                return false;
            }
        }

        Clock::TimePoint tp = Clock::now();
        if ((tp - start) > Microseconds(4)) {
            break;
        }
    }

    return true;
}

template <TrivialT T>
inline bool atomicSpinWait(T const volatile* loc, T val, int32 order) noexcept
{
    if (!atomicLoadCompare(loc, val, order)) {
        return false;
    }

    return atomicSpinWaitLoop(loc, val, order);
}

inline CORE_API void atomicPlatformWait(AtomicContention volatile* waiter, // NOLINT
                                        AtomicContention const volatile* platform,
                                        AtomicContention value,
                                        size_t size) noexcept
{
    __atomic_fetch_add(waiter, static_cast<AtomicContention>(1), __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    waitOnAddress(platform, value, size);
    __atomic_fetch_sub(waiter, static_cast<AtomicContention>(1), __ATOMIC_RELEASE);
}

inline CORE_API void atomicPlatformNotify(AtomicContention const volatile* waiter,
                                          AtomicContention const volatile* platform,
                                          size_t size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        notifyOnAddress(platform, size);
    }
}

inline CORE_API void atomicPlatformNotifyAll(AtomicContention const volatile* waiter,
                                             AtomicContention const volatile* platform,
                                             size_t size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        notifyAllOnAddress(platform, size);
    }
}

struct CORE_API alignas(__ATOMIC_INTERFERENCE_SIZE) AtomicEntry {
public:
    AtomicContention waiter;
    AtomicContention platform;

    constexpr AtomicEntry() noexcept
        : waiter(0)
        , platform(0)
    {
    }
};

// Most of the user-side processors have less than 32 cores.
// So, we're assuming that 64 is big enough for our use case.
// This implementation might get some perf issues with server station setup.
constexpr size_t contentionTableSize = 1 << 6;
CORE_API AtomicEntry atomicContentionTable[contentionTableSize];

template <typename T>
struct AtomicWaitableContext {
public:
    T const volatile* pointer;
    AtomicEntry* entry;
    size_t size;

    AtomicWaitableContext(T const volatile* pointer)
        : pointer(pointer)
    {
        size_t intptr = reinterpret_cast<size_t>(pointer);
        size_t hash = intptr >> 6;
        hash ^= hash >> 16;
        hash &= (contentionTableSize - 1);
        entry = &atomicContentionTable[hash];

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
    mini::atomicPlatformNotify(&context.entry->waiter, &context.entry->platform, context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
    requires(!mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    constexpr mini::AtomicContention contention = 1;

    __atomic_fetch_add(&context.entry->platform, contention, __ATOMIC_SEQ_CST);
    mini::atomicPlatformNotifyAll(&context.entry->waiter, &context.entry->platform, context.size);
}

export template <typename T>
inline void __atomic_notify_one(T const volatile* pointer) noexcept
    requires(mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention const volatile* loc =
        reinterpret_cast<mini::AtomicContention const volatile*>(context.pointer);

    mini::atomicPlatformNotify(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention const volatile* loc =
        reinterpret_cast<mini::AtomicContention const volatile*>(context.pointer);

    mini::atomicPlatformNotifyAll(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_wait(T const volatile* pointer, T old, int order) noexcept
    requires(!mini::AtomicWaitableT<T>::value)
{
    mini::AtomicWaitableContext context(pointer);
    mini::AtomicContention contention;
    __atomic_load(&context.entry->platform, &contention, __ATOMIC_ACQUIRE);

    while (mini::atomicLoadCompare(context.pointer, old, static_cast<mini::int32>(order))) {
        // since load compare of non atomic values cannot be fetched atomically,
        // notify can occur while load comparing the value.
        // so we should compare the monitor value to check it has been changed.
        // if so, we should not enter wait or else it will cause a dead lock.
        if (mini::atomicLoadCompare(&context.entry->platform, contention, __ATOMIC_ACQUIRE)) {
            mini::atomicPlatformWait(&context.entry->waiter, &context.entry->platform, contention, context.size);
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
        reinterpret_cast<mini::AtomicWaitableT<T>::Type*>(mini::addressof(old));

    if (mini::atomicSpinWait(context.pointer, old, static_cast<mini::int32>(order))) {
        mini::atomicPlatformWait(&context.entry->waiter, loc, *waitable, context.size);
    }
}