module;

#include <os/os_sync_wait_on_address.h>

#if CLANG || GNUC
#  if ARCH_ARM64
#    define PAUSE() asm volatile("isb")
#  elif ARCH_X86_64 || ARCH_X86
#    define PAUSE() __builtin_ia32_pause();
#  else
#    define PAUSE() asm volatile("", , , "memory")
#  endif
#else
#  error "unsupported compiler"
#endif

export module mini.core:atomic_platform_wait;

import :type;

namespace mini {

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

inline void AtomicRelax()
{
    PAUSE();
}

inline void WaitOnAddress(AtomicContention const volatile* addr, AtomicContention value, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wait_on_address(loc, value, size, OS_SYNC_WAIT_ON_ADDRESS_NONE);
}

inline void NotifyOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wake_by_address_any(loc, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

inline void NotifyAllOnAddress(AtomicContention const volatile* addr, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    os_sync_wake_by_address_all(loc, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

} // namespace mini