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

void AtomicWaitOnAddress(AtomicContention const volatile* addr, AtomicContention::Value value,
                         SizeT size)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    WaitOnAddress(const_cast<void*>(loc), static_cast<void*>(&value), size, INFINITE);
}

void AtomicNotifyOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    WakeByAddressSingle(const_cast<void*>(loc));
}

void AtomicNotifyAllOnAddress(AtomicContention const volatile* addr, SizeT)
{
    void const volatile* loc = static_cast<void const volatile*>(AddressOf(addr->value));
    WakeByAddressAll(const_cast<void*>(loc));
}

} // namespace mini::memory