module;

#include <os/os_sync_wait_on_address.h>

export module mini.core:atomic_base;

import :type;

namespace mini::memory {

export enum class MemoryOrder : int {
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acquireRelease = __ATOMIC_ACQ_REL,
    sequential = __ATOMIC_SEQ_CST
};

template <TrivialT T>
inline T AtomicLoad(T* ptr, MemoryOrder order)
{
    T result;
    __atomic_load(ptr, &result, static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline T AtomicLoad(T volatile* ptr, MemoryOrder order)
{
    T result;
    __atomic_load(ptr, &result, static_cast<int>(order));
    return result;
}

template <TrivialT T>
inline void AtomicStore(T* ptr, T* val, MemoryOrder order)
{
    __atomic_store(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline void AtomicStore(T volatile* ptr, T* val, MemoryOrder order)
{
    __atomic_store(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline void AtomicExchange(T* ptr, T* val, MemoryOrder order)
{
    __atomic_exchange(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline void AtomicExchange(T volatile* ptr, T* val, MemoryOrder order)
{
    __atomic_exchange(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline bool AtomicCompareExchangeWeak(T* ptr, T* expected, T* desired, MemoryOrder success,
                                      MemoryOrder failure)
{
    return __atomic_compare_exchange(ptr, expected, desired, true, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool AtomicCompareExchangeWeak(T volatile* ptr, T* expected, T* desired, MemoryOrder success,
                                      MemoryOrder failure)
{
    return __atomic_compare_exchange(ptr, expected, desired, true, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool AtomicCompareExchangeStrong(T* ptr, T* expected, T* desired, MemoryOrder success,
                                        MemoryOrder failure)
{
    return __atomic_compare_exchange(ptr, expected, desired, false, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline bool AtomicCompareExchangeStrong(T volatile* ptr, T* expected, T* desired,
                                        MemoryOrder success, MemoryOrder failure)
{
    return __atomic_compare_exchange(ptr, expected, desired, false, static_cast<int>(success),
                                     static_cast<int>(failure));
}

template <TrivialT T>
inline T AtomicFetchAdd(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_add(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchAdd(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_add(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchSub(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_sub(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchSub(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_sub(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchAnd(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_and(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchAnd(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_and(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchXor(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_xor(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchXor(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_xor(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchOr(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_or(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchOr(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_or(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchNand(T* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_Nand(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicFetchNand(T volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_Nand(ptr, val, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicTestAndSet(T* ptr, MemoryOrder order)
{
    return __atomic_test_and_set(ptr, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicTestAndSet(T volatile* ptr, MemoryOrder order)
{
    return __atomic_test_and_set(ptr, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicClear(T* ptr, MemoryOrder order)
{
    return __atomic_clear(ptr, static_cast<int>(order));
}

template <TrivialT T>
inline T AtomicClear(T volatile* ptr, MemoryOrder order)
{
    return __atomic_clear(ptr, static_cast<int>(order));
}

inline CORE_API void AtomicThreadFence(MemoryOrder order)
{
    __atomic_thread_fence(static_cast<int>(order));
}

inline CORE_API void AtomicSignalFence(MemoryOrder order)
{
    __atomic_signal_fence(static_cast<int>(order));
}

inline CORE_API constexpr bool AtomicAlwaysLockFree(SizeT size)
{
    // TODO: pass actual pointer to determine alignment..?
    return __atomic_always_lock_free(size, nullptr);
}

inline CORE_API constexpr bool AtomicIsLockFree(SizeT size)
{
    // TODO: pass actual pointer to determine alignment..?
    return __atomic_is_lock_free(size, nullptr);
}

using AtomicContentionT = uint64;

template <typename T>
struct AtomicWaitableT : FalseT {};

template <typename T>
    requires(sizeof(T) == 4)
struct AtomicWaitableT<T> : TrueT {
    typedef uint32 Type;
};

template <typename T>
    requires(sizeof(T) == 8)
struct AtomicWaitableT<T> : TrueT {
    typedef uint64 Type;
};

inline void AtomicWaitOnAddress(void const volatile* addr, AtomicContentionT value, SizeT size)
{
    os_sync_wait_on_address(const_cast<void*>(addr), value, size, OS_SYNC_WAIT_ON_ADDRESS_NONE);
}

inline void AtomicNotifyOnAddress(void const volatile* addr, SizeT size)
{
    os_sync_wake_by_address_any(const_cast<void*>(addr), size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

inline void AtomicNotifyAllOnAddress(void const volatile* addr, SizeT size)
{
    os_sync_wake_by_address_all(const_cast<void*>(addr), size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

} // namespace mini::memory