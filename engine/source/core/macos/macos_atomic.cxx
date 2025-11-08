module;

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

inline constexpr int32 FailureOrder(MemoryOrder order)
{
    switch (order) {
        case MemoryOrder::relaxed:
        case MemoryOrder::release:        return __ATOMIC_RELAXED;
        case MemoryOrder::consume:        return __ATOMIC_CONSUME;
        case MemoryOrder::acquire:
        case MemoryOrder::acquireRelease: return __ATOMIC_ACQUIRE;
        case MemoryOrder::sequential:
        default:                          return __ATOMIC_SEQ_CST;
    }
}

using AtomicContention = uint64;

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

inline void AtomicWaitOnAddress(AtomicContention const volatile* addr, AtomicContention value,
                                SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wait_on_address(loc, value, size, OS_SYNC_WAIT_ON_ADDRESS_NONE);
}

inline void AtomicNotifyOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wake_by_address_any(loc, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

inline void AtomicNotifyAllOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wake_by_address_all(loc, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

} // namespace mini::memory