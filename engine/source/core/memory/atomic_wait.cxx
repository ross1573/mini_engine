module;

#if CLANG // TODO: clang some how manages to skip this diagnostic
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Watomic-alignment"
#endif

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
inline bool AtomicLoadCompare(T const volatile* loc, T val, int32 order) noexcept
{
    T current;
    __atomic_load(loc, &current, order);
    return BUILTIN_MEMCMP(AddressOf(current), AddressOf(val), sizeof(T)) == 0;
}

template <TrivialT T>
inline bool AtomicSpinWait(T const volatile* loc, T val, int32 order) noexcept
{
    // TODO: fix it with duration, just like libc++
    for (SizeT i = 0; i < spinWaitCount; ++i) {
        if (!AtomicLoadCompare(loc, val, order)) {
            return false;
        }

        mini::thread::ThreadRelax();
    }

    return AtomicLoadCompare(loc, val, order);
}

inline CORE_API void AtomicPlatformWait(AtomicContention volatile* waiter,
                                        AtomicContention const volatile* platform,
                                        AtomicContention value, SizeT size) noexcept
{
    __atomic_fetch_add(waiter, AtomicContention(1), __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    AtomicWaitOnAddress(platform, value, size);
    __atomic_fetch_sub(waiter, AtomicContention(1), __ATOMIC_RELEASE);
}

inline CORE_API void AtomicPlatformNotify(AtomicContention const volatile* waiter,
                                          AtomicContention const volatile* platform,
                                          SizeT size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        AtomicNotifyOnAddress(platform, size);
    }
}

inline CORE_API void AtomicPlatformNotifyAll(AtomicContention const volatile* waiter,
                                             AtomicContention const volatile* platform,
                                             SizeT size) noexcept
{
    AtomicContention waiterCount;
    __atomic_load(waiter, &waiterCount, __ATOMIC_SEQ_CST);
    if (waiterCount != 0) {
        AtomicNotifyAllOnAddress(platform, size);
    }
}

struct CORE_API alignas(AtomicInterferenceSize) AtomicEntry {
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
        SizeT bucket = AtomicAddressHash<T, contentionTableSize>(pointer);
        entry = &g_AtomicContentionTable[bucket];

        if constexpr (AtomicWaitableT<T>::value) {
            size = sizeof(T);
        }
        else {
            size = sizeof(AtomicContention);
        }
    }
};

} // namespace mini::memory

export template <typename T>
inline void __atomic_notify_one(T const volatile* pointer) noexcept
    requires(!mini::memory::AtomicWaitableT<T>::value)
{
    mini::memory::AtomicWaitableContext context(pointer);
    constexpr mini::memory::AtomicContention contention = 1;

    __atomic_fetch_add(&context.entry->platform, contention, __ATOMIC_SEQ_CST);
    mini::memory::AtomicPlatformNotify(&context.entry->waiter, &context.entry->platform,
                                       context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
    requires(!mini::memory::AtomicWaitableT<T>::value)
{
    mini::memory::AtomicWaitableContext context(pointer);
    constexpr mini::memory::AtomicContention contention = 1;

    __atomic_fetch_add(&context.entry->platform, contention, __ATOMIC_SEQ_CST);
    mini::memory::AtomicPlatformNotifyAll(&context.entry->waiter, &context.entry->platform,
                                          context.size);
}

export template <typename T>
inline void __atomic_notify_one(T const volatile* pointer) noexcept
    requires(mini::memory::AtomicWaitableT<T>::value)
{
    mini::memory::AtomicWaitableContext context(pointer);
    mini::memory::AtomicContention const volatile* loc =
        reinterpret_cast<mini::memory::AtomicContention const volatile*>(context.pointer);

    mini::memory::AtomicPlatformNotify(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_notify_all(T const volatile* pointer) noexcept
{
    mini::memory::AtomicWaitableContext context(pointer);
    mini::memory::AtomicContention const volatile* loc =
        reinterpret_cast<mini::memory::AtomicContention const volatile*>(context.pointer);

    mini::memory::AtomicPlatformNotifyAll(&context.entry->waiter, loc, context.size);
}

export template <typename T>
inline void __atomic_wait(T const volatile* pointer, T old, int order) noexcept
    requires(!mini::memory::AtomicWaitableT<T>::value)
{
    mini::memory::AtomicWaitableContext context(pointer);
    mini::memory::AtomicContention contention;

    while (mini::memory::AtomicLoadCompare(context.pointer, old, static_cast<mini::int32>(order))) {
        __atomic_load(&context.entry->platform, &contention, static_cast<mini::int32>(order));
        mini::memory::AtomicPlatformWait(&context.entry->waiter, &context.entry->platform,
                                         contention, context.size);
    }
}

export template <typename T>
inline void __atomic_wait(T const volatile* pointer, T old, int order) noexcept
    requires(mini::memory::AtomicWaitableT<T>::value)
{
    mini::memory::AtomicWaitableContext context(pointer);
    mini::memory::AtomicContention const volatile* loc =
        reinterpret_cast<mini::memory::AtomicContention const volatile*>(context.pointer);
    typename mini::memory::AtomicWaitableT<T>::Type* waitable =
        reinterpret_cast<mini::memory::AtomicWaitableT<T>::Type*>(mini::memory::AddressOf(old));

    if (mini::memory::AtomicSpinWait(context.pointer, old, static_cast<mini::int32>(order))) {
        mini::memory::AtomicPlatformWait(&context.entry->waiter, loc, *waitable, context.size);
    }
}