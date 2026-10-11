module;

#include "win_include.h"

module mini.core;

import :type;
import :atomic_platform_wait;

namespace mini {

void waitOnAddress(AtomicContention const volatile* addr, AtomicContention value, size_t size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WaitOnAddress(loc, static_cast<void*>(&value), size, INFINITE);
}

void notifyOnAddress(AtomicContention const volatile* addr, size_t)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WakeByAddressSingle(loc);
}

void notifyAllOnAddress(AtomicContention const volatile* addr, size_t)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    ::WakeByAddressAll(loc);
}

} // namespace mini