module;

#include "internal/min_windows.h"

module mini.core;

import :type;
import :atomic_platform_wait;

namespace mini {

void WaitOnAddress(AtomicContention const volatile* addr, AtomicContention value, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WaitOnAddress(loc, static_cast<void*>(&value), size, INFINITE);
}

void NotifyOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WakeByAddressSingle(loc);
}

void NotifyAllOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WakeByAddressAll(loc);
}

} // namespace mini