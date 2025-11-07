module;

#pragma clang diagnostic ignored "-Watomic-alignment"
#include <os/os_sync_wait_on_address.h>

export module mini.core:atomic_impl;

import :type;
import :memory;
import :atomic_base;

namespace mini::memory {

export enum class MemoryOrder : int {
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acquireRelease = __ATOMIC_ACQ_REL,
    sequential = __ATOMIC_SEQ_CST
};

template <typename T>
inline T AtomicLoad(AtomicBase<T> const* ptr, MemoryOrder order)
{
    T result;
    __atomic_load(AddressOf(ptr->value), &result, static_cast<int>(order));
    return result;
}

template <typename T>
inline T AtomicLoad(AtomicBase<T> const volatile* ptr, MemoryOrder order)
{
    T result;
    __atomic_load(AddressOf(ptr->value), &result, static_cast<int>(order));
    return result;
}

template <typename T>
inline void AtomicStore(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    __atomic_store(AddressOf(ptr->value), &val, static_cast<int>(order));
}

template <typename T>
inline void AtomicStore(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    __atomic_store(AddressOf(ptr->value), &val, static_cast<int>(order));
}

template <typename T>
inline T AtomicExchange(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    T result;
    __atomic_exchange(AddressOf(ptr->value), &val, &result, static_cast<int>(order));
    return result;
}

template <typename T>
inline T AtomicExchange(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    T result;
    __atomic_exchange(AddressOf(ptr->value), &val, &result, static_cast<int>(order));
    return result;
}

template <typename T>
inline bool AtomicCompareExchangeWeak(AtomicBase<T>* ptr, T* expected, T desired,
                                      MemoryOrder success, MemoryOrder failure)
{
    return __atomic_compare_exchange(AddressOf(ptr->value), expected, &desired, true,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <typename T>
inline bool AtomicCompareExchangeWeak(AtomicBase<T> volatile* ptr, T* expected, T desired,
                                      MemoryOrder success, MemoryOrder failure)
{
    return __atomic_compare_exchange(AddressOf(ptr->value), expected, &desired, true,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <typename T>
inline bool AtomicCompareExchangeStrong(AtomicBase<T>* ptr, T* expected, T desired,
                                        MemoryOrder success, MemoryOrder failure)
{
    return __atomic_compare_exchange(AddressOf(ptr->value), expected, &desired, false,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <typename T>
inline bool AtomicCompareExchangeStrong(AtomicBase<T> volatile* ptr, T* expected, T desired,
                                        MemoryOrder success, MemoryOrder failure)
{
    return __atomic_compare_exchange(AddressOf(ptr->value), expected, &desired, false,
                                     static_cast<int>(success), static_cast<int>(failure));
}

template <typename T>
inline T AtomicFetchAdd(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_add(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchAdd(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_add(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchSub(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_sub(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchSub(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_sub(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchAnd(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_and(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchAnd(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_and(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchXor(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_xor(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchXor(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_xor(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchOr(AtomicBase<T>* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_or(AddressOf(ptr->value), val, static_cast<int>(order));
}

template <typename T>
inline T AtomicFetchOr(AtomicBase<T> volatile* ptr, T val, MemoryOrder order)
{
    return __atomic_fetch_or(AddressOf(ptr->value), val, static_cast<int>(order));
}

inline CORE_API void AtomicThreadFence(MemoryOrder order)
{
    __atomic_thread_fence(static_cast<int>(order));
}

inline CORE_API void AtomicSignalFence(MemoryOrder order)
{
    __atomic_signal_fence(static_cast<int>(order));
}

consteval bool AtomicAlwaysLockFree(SizeT size)
{
    return __atomic_always_lock_free(size, nullptr);
}

template <typename T>
inline constexpr bool AtomicIsLockFree(SizeT size, AtomicBase<T> const* ptr)
{
    return __atomic_is_lock_free(size, static_cast<void*>(AddressOf(ptr)));
}

template <typename T>
inline constexpr bool AtomicIsLockFree(SizeT size, AtomicBase<T> const volatile* ptr)
{
    return __atomic_is_lock_free(size, static_cast<void volatile*>(AddressOf(ptr)));
}

using AtomicContention = AtomicBase<uint64>;

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

inline void AtomicWaitOnAddress(AtomicContention const volatile* addr,
                                AtomicContention::Value value, SizeT size)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    os_sync_wait_on_address(const_cast<void*>(loc), value, size, OS_SYNC_WAIT_ON_ADDRESS_NONE);
}

inline void AtomicNotifyOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    os_sync_wake_by_address_any(const_cast<void*>(loc), size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

inline void AtomicNotifyAllOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    os_sync_wake_by_address_all(const_cast<void*>(loc), size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

} // namespace mini::memory