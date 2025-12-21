module;

#define WIN32_LEAN_AND_MEAN
#define NOWINBASEINTERLOCK
#define NOSERVICE
#define NOMCX
#define NOIME
#include <windows.h>

module mini.atomic;

import mini.type;
import :platform_wait;

namespace mini {

void WaitOnAddress(AtomicContention const volatile* addr, AtomicContention value, SizeT size)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WaitOnAddress(loc, static_cast<void*>(&value), size, INFINITE);
}

void NotifyOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WakeByAddressSingle(loc);
}

void NotifyAllOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void* loc = const_cast<void*>(static_cast<void const volatile*>(addr));
    WakeByAddressAll(loc);
}

} // namespace mini