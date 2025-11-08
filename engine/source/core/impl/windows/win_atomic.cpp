module;

#define WIN32_LEAN_AND_MEAN
#define NOWINBASEINTERLOCK
#define NOSERVICE
#define NOMCX
#define NOIME
#include <windows.h>

module mini.core;

import :type;
import :memory;
import :atomic_base;
import :atomic_impl;

namespace mini::memory {

void AtomicWaitOnAddress(AtomicContention const volatile* addr, AtomicContention value, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WaitOnAddress(loc, static_cast<void*>(&value), size, INFINITE);
}

void AtomicNotifyOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WakeByAddressSingle(loc);
}

void AtomicNotifyAllOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WakeByAddressAll(loc);
}

} // namespace mini::memory