#include "test_macro.h"

import mini.test;

#if MSVC
#  undef TEST_CONSTEXPR
#  define TEST_CONSTEXPR(func) TEST_FUNCTION(func)
#endif

struct IndirectPointer {
    mini::int32* ptr;

    constexpr mini::int32* address() const noexcept { return ptr; }
};

struct AddressOverloaded {
    mini::int32 value = 0;

    constexpr AddressOverloaded* operator&() noexcept { return nullptr; }
};

struct IndirectIterator {
    CopyableObject* ptr;

    constexpr CopyableObject* address() const noexcept { return ptr; }
    constexpr IndirectIterator& operator++() noexcept
    {
        ++ptr;
        return *this;
    }
    constexpr bool operator==(IndirectIterator const& other) const noexcept { return ptr == other.ptr; }
};

template <typename T>
constexpr mini::int32 sumOf(T const* ptr, mini::size_t len)
{
    mini::int32 sum = 0;
    for (mini::size_t i = 0; i < len; ++i) {
        sum += static_cast<mini::int32>(ptr[i]);
    }
    return sum;
}

constexpr mini::int32 testAddress()
{
    mini::int32 values[2] = {1, 2};
    IndirectPointer indirect{values + 1};

    TEST_ENSURE(mini::address(values + 0) == values);
    TEST_ENSURE(mini::address(indirect) == values + 1);
    TEST_ENSURE(*mini::address(indirect) == 2);

    return 0;
}

constexpr mini::int32 testAddressOf()
{
    mini::int32 value = 0;
    AddressOverloaded overloaded;

    TEST_ENSURE(mini::addressof(value) == &value);
    TEST_ENSURE(mini::addressof(overloaded) != nullptr);

    return 0;
}

mini::int32 testPointerOverlapping()
{
    mini::int32 values[8] = {};
    mini::int32* begin = values;
    mini::int32* end = values + 4;

    TEST_ENSURE(mini::pointerOverlapping(values + 0, begin, end));
    TEST_ENSURE(mini::pointerOverlapping(values + 3, begin, end));
    TEST_ENSURE(!mini::pointerOverlapping(values + 4, begin, end));
    TEST_ENSURE(mini::pointerOverlapping(static_cast<mini::int32 const*>(values + 2), begin, end));
    TEST_ENSURE(!mini::pointerOverlapping(values + 1, values + 2, values + 4));
    TEST_ENSURE(!mini::pointerOverlapping(values + 2, values + 2, values + 2));

    TEST_ENSURE(mini::pointerOverlapping(values + 2, values + 6, begin, end));
    TEST_ENSURE(mini::pointerOverlapping(values + 0, values + 3, values + 2, values + 6));
    TEST_ENSURE(!mini::pointerOverlapping(values + 4, values + 8, begin, end));
    TEST_ENSURE(mini::pointerOverlapping(values + 0, values + 8, values + 2, values + 4));
    TEST_ENSURE(mini::pointerOverlapping(values + 2, values + 4, values + 2, values + 4));
    TEST_ENSURE(!mini::pointerOverlapping(values + 0, values + 4, values + 4, values + 8));

    return 0;
}

constexpr mini::int32 testConstructAt()
{
    if !consteval {
        initializeCounter();
    }

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(2);

    mini::memory::constructAt(result.pointer, 3);
    mini::memory::constructAt(result.pointer + 1, mini::moveCast(result.pointer[0]));
    TEST_ENSURE(result.pointer[1].value() == 3);
    TEST_ENSURE(result.pointer[0].resource() == nullptr);
    if !consteval {
        TEST_ENSURE(ctor == 1 && moveCtor == 1 && dtor == 0);
    }

    mini::memory::destructAt(result.pointer);
    mini::memory::destructAt(result.pointer + 1);
    if !consteval {
        TEST_ENSURE(dtor == 2);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructAtCopy()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject source(9);
    CopyableObject const constSource(10);

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(3);

    mini::memory::constructAt(result.pointer, source);
    mini::memory::constructAt(result.pointer + 1, constSource);
    mini::memory::constructAt(result.pointer + 2);
    TEST_ENSURE(result.pointer[0].value() == 9);
    TEST_ENSURE(result.pointer[1].value() == 10);
    TEST_ENSURE(result.pointer[2].value() == 0);
    TEST_ENSURE(source.value() == 9);
    if !consteval {
        TEST_ENSURE(ctor == 3 && copyCtor == 2 && moveCtor == 0);
    }

    mini::memory::destructRange(result.pointer, result.pointer + 3);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructAtTrivial()
{
    mini::Allocator<TrivialObject> alloc;
    mini::AllocationResult<TrivialObject> result = alloc.allocate(2);

    mini::memory::constructAt(result.pointer, 1, 2);
    mini::memory::constructAt(result.pointer + 1);
    TEST_ENSURE((result.pointer[0] == TrivialObject{1, 2}));
    TEST_ENSURE((result.pointer[1] == TrivialObject{0, 0}));

    mini::memory::destructAt(result.pointer);
    mini::memory::destructAt(result.pointer + 1);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testBeginLifetime()
{
    mini::Allocator<mini::int32> alloc;
    mini::AllocationResult<mini::int32> result = alloc.allocate(4);

    mini::memory::beginLifetime(result.pointer, result.pointer + 3);
    mini::memory::beginLifetime(result.pointer + 3);
    for (mini::int32 i = 0; i < 4; ++i) {
        result.pointer[i] = i + 1;
    }
    TEST_ENSURE(sumOf(result.pointer, 4) == 10);

    mini::memory::beginLifetime(result.pointer, result.pointer);

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructRange()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject source[3] = {CopyableObject(1), CopyableObject(2), CopyableObject(3)};

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(3);

    mini::memory::constructRange(result.pointer, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i].value() == i + 1);
        TEST_ENSURE(source[i].value() == i + 1);
    }
    if !consteval {
        TEST_ENSURE(copyCtor == 3 && moveCtor == 0);
    }

    mini::memory::destructRange(result.pointer, result.pointer + 3);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructRangeArgs()
{
    if !consteval {
        initializeCounter();
    }

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(3);

    mini::memory::constructRangeArgs(result.pointer, result.pointer + 3, 7);
    for (mini::size_t i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i].value() == 7);
    }
    if !consteval {
        TEST_ENSURE(ctor == 3);
    }

    mini::memory::constructRangeArgs(result.pointer, result.pointer);

    mini::memory::destructRange(result.pointer, result.pointer + 3);
    if !consteval {
        TEST_ENSURE(dtor == 3);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructRangeTrivial()
{
    mini::int32 const source[4] = {1, 2, 3, 4};

    mini::Allocator<mini::int32> alloc;
    mini::AllocationResult<mini::int32> result = alloc.allocate(4);

    mini::memory::constructRange(result.pointer, source + 0, source + 4);
    for (mini::int32 i = 0; i < 4; ++i) {
        TEST_ENSURE(result.pointer[i] == i + 1);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructRangeConvert()
{
    if !consteval {
        initializeCounter();
    }

    mini::int32 const source[3] = {4, 5, 6};

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(6);

    mini::memory::constructRange(result.pointer, source + 0, source + 3);
    mini::memory::constructBackward(result.pointer + 6, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i].value() == i + 4);
        TEST_ENSURE(result.pointer[i + 3].value() == i + 4);
    }
    if !consteval {
        TEST_ENSURE(ctor == 6 && copyCtor == 0 && moveCtor == 0);
    }

    mini::memory::destructRange(result.pointer, result.pointer + 6);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructBackward()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject source[3] = {CopyableObject(1), CopyableObject(2), CopyableObject(3)};

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(4);

    mini::memory::constructBackward(result.pointer + 4, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i + 1].value() == i + 1);
        TEST_ENSURE(source[i].value() == i + 1);
    }
    if !consteval {
        TEST_ENSURE(copyCtor == 3 && moveCtor == 0);
    }

    mini::memory::destructRange(result.pointer + 1, result.pointer + 4);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testConstructBackwardTrivial()
{
    mini::int32 const source[3] = {1, 2, 3};

    mini::Allocator<mini::int32> alloc;
    mini::AllocationResult<mini::int32> result = alloc.allocate(4);

    mini::memory::beginLifetime(result.pointer);
    result.pointer[0] = 0;
    mini::memory::constructBackward(result.pointer + 4, source + 0, source + 3);
    for (mini::int32 i = 0; i < 4; ++i) {
        TEST_ENSURE(result.pointer[i] == i);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testMoveConstructRange()
{
    if !consteval {
        initializeCounter();
    }

    MoveOnlyObject source[3] = {MoveOnlyObject(1), MoveOnlyObject(2), MoveOnlyObject(3)};

    mini::Allocator<MoveOnlyObject> alloc;
    mini::AllocationResult<MoveOnlyObject> result = alloc.allocate(3);

    mini::memory::moveconstructRange(result.pointer, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i].value() == i + 1);
        TEST_ENSURE(source[i].resource() == nullptr);
    }
    if !consteval {
        TEST_ENSURE(moveCtor == 3);
    }

    mini::memory::destructRange(result.pointer, result.pointer + 3);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testMoveConstructRangeTrivial()
{
    mini::int32 source[3] = {4, 5, 6};

    mini::Allocator<mini::int32> alloc;
    mini::AllocationResult<mini::int32> result = alloc.allocate(3);

    mini::memory::moveconstructRange(result.pointer, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i] == i + 4);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testMoveConstructBackward()
{
    if !consteval {
        initializeCounter();
    }

    MoveOnlyObject source[3] = {MoveOnlyObject(1), MoveOnlyObject(2), MoveOnlyObject(3)};

    mini::Allocator<MoveOnlyObject> alloc;
    mini::AllocationResult<MoveOnlyObject> result = alloc.allocate(5);

    mini::memory::moveconstructBackward(result.pointer + 5, source + 0, source + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i + 2].value() == i + 1);
        TEST_ENSURE(source[i].resource() == nullptr);
    }
    if !consteval {
        TEST_ENSURE(moveCtor == 3);
    }

    mini::memory::destructRange(result.pointer + 2, result.pointer + 5);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testMoveConstructBackwardTrivial()
{
    mini::int32 source[2] = {7, 8};

    mini::Allocator<mini::int32> alloc;
    mini::AllocationResult<mini::int32> result = alloc.allocate(3);

    mini::memory::moveconstructBackward(result.pointer + 3, source + 0, source + 2);
    TEST_ENSURE(result.pointer[1] == 7);
    TEST_ENSURE(result.pointer[2] == 8);

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testDestructRange()
{
    if !consteval {
        initializeCounter();
    }

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(4);

    mini::memory::constructRangeArgs(result.pointer, result.pointer + 4, 1);
    mini::memory::destructRange(result.pointer, result.pointer);
    if !consteval {
        TEST_ENSURE(dtor == 0);
    }

    mini::memory::destructRange(result.pointer, result.pointer + 4);
    if !consteval {
        TEST_ENSURE(ctor == 4 && dtor == 4);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testEmptyRange()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject source[1] = {CopyableObject(1)};
    mini::int32 trivialSource[1] = {1};

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(1);
    mini::Allocator<mini::int32> trivialAlloc;
    mini::AllocationResult<mini::int32> trivialResult = trivialAlloc.allocate(1);

    mini::memory::constructRange(result.pointer, source + 0, source + 0);
    mini::memory::constructBackward(result.pointer + 1, source + 0, source + 0);
    mini::memory::moveconstructRange(result.pointer, source + 0, source + 0);
    mini::memory::moveconstructBackward(result.pointer + 1, source + 0, source + 0);
    mini::memory::constructRange(trivialResult.pointer, trivialSource + 0, trivialSource + 0);
    mini::memory::constructBackward(trivialResult.pointer + 1, trivialSource + 0, trivialSource + 0);
    mini::memory::moveconstructRange(trivialResult.pointer, trivialSource + 0, trivialSource + 0);
    mini::memory::moveconstructBackward(trivialResult.pointer + 1, trivialSource + 0, trivialSource + 0);

    TEST_ENSURE(source[0].value() == 1);
    TEST_ENSURE(trivialSource[0] == 1);
    if !consteval {
        TEST_ENSURE(ctor == 1 && copyCtor == 0 && moveCtor == 0 && dtor == 0);
    }

    trivialAlloc.deallocate(trivialResult.pointer, trivialResult.capacity);
    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

constexpr mini::int32 testIndirectRange()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject source[2] = {CopyableObject(1), CopyableObject(2)};

    mini::Allocator<CopyableObject> alloc;
    mini::AllocationResult<CopyableObject> result = alloc.allocate(5);
    IndirectIterator begin{result.pointer};
    IndirectIterator middle{result.pointer + 3};
    IndirectIterator end{result.pointer + 5};

    mini::memory::constructRangeArgs(begin, middle, 8);
    mini::memory::constructRange(middle, source + 0, source + 2);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(result.pointer[i].value() == 8);
    }
    TEST_ENSURE(result.pointer[3].value() == 1);
    TEST_ENSURE(result.pointer[4].value() == 2);
    if !consteval {
        TEST_ENSURE(ctor == 5 && copyCtor == 2);
    }

    mini::memory::destructRange(begin, end);
    if !consteval {
        TEST_ENSURE(dtor == 5);
    }

    alloc.deallocate(result.pointer, result.capacity);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testAddress);
    TEST_CONSTEXPR(testAddressOf);
    TEST_FUNCTION(testPointerOverlapping);

    TEST_CONSTEXPR(testConstructAt);
    TEST_CONSTEXPR(testConstructAtCopy);
    TEST_CONSTEXPR(testConstructAtTrivial);

    TEST_CONSTEXPR(testBeginLifetime);

    TEST_CONSTEXPR(testConstructRange);
    TEST_CONSTEXPR(testConstructRangeArgs);
    TEST_CONSTEXPR(testConstructRangeTrivial);
    TEST_CONSTEXPR(testConstructRangeConvert);

    TEST_CONSTEXPR(testConstructBackward);
    TEST_CONSTEXPR(testConstructBackwardTrivial);

    TEST_CONSTEXPR(testMoveConstructRange);
    TEST_CONSTEXPR(testMoveConstructRangeTrivial);

    TEST_CONSTEXPR(testMoveConstructBackward);
    TEST_CONSTEXPR(testMoveConstructBackwardTrivial);

    TEST_CONSTEXPR(testDestructRange);

    TEST_CONSTEXPR(testEmptyRange);
    TEST_CONSTEXPR(testIndirectRange);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
