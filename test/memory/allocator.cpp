#include "test_macro.h"

import mini.test;

using mini::AllocationResult;
using mini::Allocator;
using mini::rebind;
using mini::UnboundAllocator;

struct IdentityAlloc {
    typedef mini::int32 Value;
    typedef mini::int32* Pointer;
    typedef mini::int32 const* ConstPointer;

    mini::int32 id = 0;

    constexpr AllocationResult<mini::int32> allocate(mini::size_t size) { return Allocator<mini::int32>{}.allocate(size); }
    constexpr void deallocate(Pointer ptr, mini::size_t size) { Allocator<mini::int32>{}.deallocate(ptr, size); }
};

template <typename T>
struct StatefulAlloc : Allocator<T> {
    mini::int32 id = 0;
};

struct RebindTag { };

template <typename U>
constexpr StatefulAlloc<U> rebind(Allocator<RebindTag> const&)
{
    return StatefulAlloc<U>{{}, 42};
}

constexpr mini::int32 testAllocate()
{
    constexpr mini::size_t size = 16;

    Allocator<mini::int32> const alloc;
    AllocationResult<mini::int32> result = alloc.allocate(size);
    AllocationResult<mini::int32> other = alloc.allocate(1);
    TEST_ENSURE(result.pointer != nullptr);
    TEST_ENSURE(result.capacity == size);
    TEST_ENSURE(other.pointer != nullptr);
    TEST_ENSURE(other.pointer != result.pointer);

    if !consteval {
        TEST_ENSURE(reinterpret_cast<mini::size_t>(result.pointer) % alignof(mini::int32) == 0);
    }

    for (mini::size_t i = 0; i < size; ++i) {
        mini::memory::constructAt(result.pointer + i, static_cast<mini::int32>(i * 2));
    }

    mini::int32 sum = 0;
    for (mini::size_t i = 0; i < size; ++i) {
        sum += result.pointer[i];
    }
    TEST_ENSURE(sum == 240);

    alloc.deallocate(result.pointer, result.capacity);
    alloc.deallocate(other.pointer, other.capacity);

    return 0;
}

constexpr mini::int32 testAllocateObject()
{
    if !consteval {
        initializeCounter();
    }

    Allocator<CopyableObject> alloc;
    AllocationResult<CopyableObject> result = alloc.allocate(4);

    TEST_ENSURE(result.pointer != nullptr);
    TEST_ENSURE(result.capacity == 4);
    if !consteval {
        TEST_ENSURE(reinterpret_cast<mini::size_t>(result.pointer) % alignof(CopyableObject) == 0);
        TEST_ENSURE(ctor == 0);
    }

    for (mini::size_t i = 0; i < result.capacity; ++i) {
        mini::memory::constructAt(result.pointer + i, static_cast<mini::int32>(i));
    }
    if !consteval {
        TEST_ENSURE(ctor == 4);
    }
    TEST_ENSURE(result.pointer[0].value() == 0);
    TEST_ENSURE(result.pointer[3].value() == 3);

    for (mini::size_t i = 0; i < result.capacity; ++i) {
        mini::memory::destructAt(result.pointer + i);
    }
    if !consteval {
        TEST_ENSURE(dtor == 4);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

mini::int32 testAllocateOveraligned()
{
    Allocator<OveralignedObject> alloc;
    AllocationResult<OveralignedObject> results[4] = {
        alloc.allocate(1),
        alloc.allocate(2),
        alloc.allocate(3),
        alloc.allocate(4),
    };

    for (AllocationResult<OveralignedObject> const& result : results) {
        TEST_ENSURE(result.pointer != nullptr);
        TEST_ENSURE(reinterpret_cast<mini::size_t>(result.pointer) % alignof(OveralignedObject) == 0);
    }

    for (AllocationResult<OveralignedObject> const& result : results) {
        alloc.deallocate(result.pointer, result.capacity);
    }

    return 0;
}

constexpr mini::int32 testRebindAllocator()
{
    Allocator<CopyableObject> rebounds[] = {
        rebind<CopyableObject>(Allocator<mini::int32>{}),
        rebind<CopyableObject>(UnboundAllocator{}),
        rebind<CopyableObject>(Allocator<CopyableObject>{}),
    };

    for (Allocator<CopyableObject> const& rebound : rebounds) {
        AllocationResult<CopyableObject> result = rebound.allocate(1);

        mini::memory::constructAt(result.pointer, 4);
        TEST_ENSURE(*result.pointer == CopyableObject(4));

        mini::memory::destructAt(result.pointer);
        rebound.deallocate(result.pointer, result.capacity);
    }

    return 0;
}

constexpr mini::int32 testRebindOverride()
{
    auto&& rebound = rebind<mini::int32>(Allocator<RebindTag>{});
    TEST_ENSURE(rebound.id == 42);

    return 0;
}

constexpr mini::int32 testRebindIdentity()
{
    IdentityAlloc alloc{.id = 7};
    IdentityAlloc const& constAlloc = alloc;

    IdentityAlloc copied = rebind<mini::int32>(alloc);
    IdentityAlloc constCopied = rebind<mini::int32>(constAlloc);
    IdentityAlloc moved = rebind<mini::int32>(mini::moveCast(alloc));
    TEST_ENSURE(copied.id == 7);
    TEST_ENSURE(constCopied.id == 7);
    TEST_ENSURE(moved.id == 7);

    return 0;
}

constexpr mini::int32 testEqual()
{
    TEST_ENSURE(Allocator<mini::int32>{} == Allocator<mini::int32>{});
    TEST_ENSURE(Allocator<CopyableObject>{} == Allocator<mini::int32>{});
    TEST_ENSURE(UnboundAllocator{} == UnboundAllocator{});

    return 0;
}

constexpr mini::int32 testInterchangeable()
{
    Allocator<mini::int32> const alloc;
    Allocator<mini::int32> const other = rebind<mini::int32>(Allocator<CopyableObject>{});
    Allocator<mini::int32> const unbound = rebind<mini::int32>(UnboundAllocator{});

    AllocationResult<mini::int32> first = alloc.allocate(3);
    AllocationResult<mini::int32> second = other.allocate(5);
    mini::memory::constructAt(first.pointer, 1);
    mini::memory::constructAt(second.pointer, 2);
    TEST_ENSURE(*first.pointer == 1);
    TEST_ENSURE(*second.pointer == 2);

    other.deallocate(first.pointer, first.capacity);
    unbound.deallocate(second.pointer, second.capacity);

    return 0;
}

constexpr mini::int32 testDeallocateNull()
{
    Allocator<mini::int32> const alloc;
    alloc.deallocate(nullptr, 0);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testAllocate);
    TEST_CONSTEXPR(testAllocateObject);
    TEST_FUNCTION(testAllocateOveraligned);

    TEST_CONSTEXPR(testRebindAllocator);
    TEST_CONSTEXPR(testRebindOverride);
    TEST_CONSTEXPR(testRebindIdentity);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testInterchangeable);

    TEST_CONSTEXPR(testDeallocateNull);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
