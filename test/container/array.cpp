#include <vector>

#include "test_macro.h"

import mini.test;

using mini::Array;
using mini::ArrayView;
using mini::InitializerList;

template <typename T, typename AllocT, typename StdAllocT>
[[no_inline]] constexpr mini::int32 testArray(Array<T, AllocT> const& arr, std::vector<T, StdAllocT> const& vec)
{
    TEST_ENSURE(arr.size() == vec.size());

    if (!arr.empty()) {
        TEST_ENSURE(arr.first() == vec.front());
        TEST_ENSURE(arr.last() == vec.back());
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        TEST_ENSURE(arr[i] == vec[i]);
        TEST_ENSURE(arr.at(i) == vec.at(i));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
constexpr void fill(Array<T>& arr, std::vector<T>& vec, mini::int32 count)
{
    for (mini::int32 i = 0; i < count; ++i) {
        arr.pushBack(makeObject<T>(i + 34));
        vec.push_back(makeObject<T>(i + 34));
    }
}

constexpr Array<mini::int32> makeSequence(mini::int32 count)
{
    Array<mini::int32> arr;
    for (mini::int32 i = 0; i < count; ++i) {
        arr.pushBack(i);
    }

    return arr;
}

inline mini::size_t liveBufferCount = 0;

template <typename T>
struct BufferCountingAlloc {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    constexpr mini::AllocationResult<T> allocate(mini::size_t size) const noexcept
    {
        mini::AllocationResult<T> result = mini::Allocator<T>{}.allocate(size);
        if !consteval {
            if (result.pointer != nullptr) {
                ++liveBufferCount;
            }
        }

        return result;
    }

    constexpr void deallocate(Pointer ptr, mini::size_t size) const noexcept
    {
        if !consteval {
            if (ptr != nullptr) {
                --liveBufferCount;
            }
        }

        mini::Allocator<T>{}.deallocate(ptr, size);
    }

    template <typename U>
    constexpr bool operator==(BufferCountingAlloc<U> const&) const noexcept
    {
        return true;
    }
};

template <typename T>
[[no_inline]] constexpr mini::int32 testCtor()
{
    Array<T> arr;
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAllocCtor()
{
    mini::Allocator<T> alloc;

    Array<T> copied(alloc);
    TEST_ENSURE(copied.empty());
    TEST_ENSURE(copied.capacity() == 0);

    Array<T> moved(mini::moveCast(alloc));
    TEST_ENSURE(moved.empty());
    TEST_ENSURE(moved.capacity() == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCapacityCtor()
{
    Array<T> arr(16);
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == 16);

    Array<T> allocated(16, mini::Allocator<T>{});
    TEST_ENSURE(allocated.empty());
    TEST_ENSURE(allocated.capacity() == 16);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyCtor()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 20);

    Array<T> copied(arr);
    TEST_ENSURE(copied == arr);
    TEST_ENSURE(testArray(copied, vec) == 0);

    Array<T> allocated(arr, mini::Allocator<T>{});
    TEST_ENSURE(allocated == arr);
    TEST_ENSURE(testArray(allocated, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveCtor()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 20);

    Array<T> moved(mini::moveCast(arr));
    TEST_ENSURE(testArray(moved, vec) == 0);
    TEST_ENSURE(arr.empty());

    Array<T> allocated(mini::moveCast(moved), mini::Allocator<T>{});
    TEST_ENSURE(testArray(allocated, vec) == 0);
    TEST_ENSURE(moved.empty());

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testRangeCtor()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 20);

    Array<T> ranged(arr.begin(), arr.end());
    TEST_ENSURE(testArray(ranged, vec) == 0);

    Array<T> allocated(arr.begin(), arr.end(), mini::Allocator<T>{});
    TEST_ENSURE(testArray(allocated, vec) == 0);

    Array<T> partial(arr.begin() + 1, arr.end() - 1);
    TEST_ENSURE(testArray(partial, std::vector<T>(vec.begin() + 1, vec.end() - 1)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInitializerListCtor()
{
    InitializerList list = {
        makeObject<T>(101),
        makeObject<T>(102),
        makeObject<T>(103),
        makeObject<T>(104),
    };

    TEST_ENSURE(testArray(Array<T>(list), std::vector<T>(list)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPushBack()
{
    Array<T> arr;
    std::vector<T> vec;

    arr.pushBack(makeObject<T>(34));
    vec.push_back(makeObject<T>(34));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.pushBack(makeArgument<T>(35));
    vec.emplace_back(makeArgument<T>(35));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAppendList()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 2);

    InitializerList list = {
        makeObject<T>(101),
        makeObject<T>(102),
        makeObject<T>(103),
        makeObject<T>(104),
    };

    arr.append(list);
    vec.append_range(list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertIndex()
{
    Array<T> arr;
    std::vector<T> vec;

    arr.insert(0, makeObject<T>(34));
    vec.insert(vec.begin(), makeObject<T>(34));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(0, makeArgument<T>(35));
    vec.emplace(vec.begin(), makeArgument<T>(35));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(1, makeObject<T>(36));
    vec.insert(vec.begin() + 1, makeObject<T>(36));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertIterator()
{
    Array<T> arr;
    std::vector<T> vec;

    arr.insert(arr.begin(), makeObject<T>(34));
    vec.insert(vec.begin(), makeObject<T>(34));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), makeArgument<T>(35));
    vec.emplace(vec.begin(), makeArgument<T>(35));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.end(), makeObject<T>(36));
    vec.insert(vec.end(), makeObject<T>(36));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeIterator()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    Array<T> source(arr);

    arr.insertRange(0, source.end() - 2, source.end());
    vec.insert(vec.begin(), source.end() - 2, source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(1, source.begin(), source.begin() + 2);
    vec.insert(vec.begin() + 1, source.begin(), source.begin() + 2);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeList()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    InitializerList list = {
        makeObject<T>(101),
        makeObject<T>(102),
        makeObject<T>(103),
        makeObject<T>(104),
    };

    arr.insertRange(2, list);
    vec.insert(vec.begin() + 2, list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeIndexArray()
{
    Array<T> source;
    std::vector<T> vec;
    fill<T>(source, vec, 4);

    Array<T> arr;
    arr.pushBack(makeObject<T>(1));
    arr.pushBack(makeObject<T>(2));
    std::vector<T> expected;
    expected.push_back(makeObject<T>(1));
    expected.push_back(makeObject<T>(2));

    arr.insertRange(1, source);
    expected.insert(expected.begin() + 1, vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, expected) == 0);
    TEST_ENSURE(testArray(source, vec) == 0);

    arr.insertRange(arr.size(), source);
    expected.insert(expected.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, expected) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopBack()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 32);

    arr.popBack();
    vec.pop_back();
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopLast()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    T popped = arr.popLast();
    TEST_ENSURE(popped == vec.back());

    vec.pop_back();
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopBackCount()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 32);

    arr.popBack(5);
    vec.erase(vec.end() - 5, vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testRemoveIndex()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 32);

    arr.remove(0);
    vec.erase(vec.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.remove(5);
    vec.erase(vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testRemoveIterator()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 32);

    arr.remove(arr.begin() + 5);
    vec.erase(vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.remove(arr.end() - 1);
    vec.erase(vec.end() - 1);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testRemoveRange()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 32);

    arr.removeRange(arr.begin() + 2, arr.begin() + 5);
    vec.erase(vec.begin() + 2, vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAssignRange()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    Array<T> assigned;
    assigned.assign(arr.begin(), arr.end());
    TEST_ENSURE(assigned == arr);
    TEST_ENSURE(testArray(assigned, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testResize()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr.resize(20, makeArgument<T>(50));
    vec.resize(20, makeObject<T>(50));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.resize(24, makeObject<T>(51));
    vec.resize(24, makeObject<T>(51));
    TEST_ENSURE(testArray(arr, vec) == 0);

    if constexpr (mini::DefaultConstructibleT<T>) {
        arr.resize(4);
        vec.resize(4);
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testReserve()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr.reserve(24);
    vec.reserve(24);
    TEST_ENSURE(arr.capacity() >= 24);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testShrink()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr.reserve(24);
    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testSwap()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    Array<T> other;
    arr.swap(other);
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(testArray(other, vec) == 0);

    arr.swap(other);
    TEST_ENSURE(other.empty());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testClear()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr.clear();
    vec.clear();
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyAssign()
{
    Array<T> source;
    std::vector<T> vec;
    fill<T>(source, vec, 8);

    Array<T> copied;
    copied.pushBack(makeObject<T>(9));
    copied = source;
    TEST_ENSURE(testArray(copied, vec) == 0);
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(copied.data() != source.data());

    Array<T> larger;
    std::vector<T> unused;
    fill<T>(larger, unused, 20);
    larger = source;
    TEST_ENSURE(testArray(larger, vec) == 0);

    Array<T> empty;
    larger = empty;
    TEST_ENSURE(larger.empty());

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveAssign()
{
    Array<T> source;
    std::vector<T> vec;
    fill<T>(source, vec, 8);

    T const* data = source.data();
    Array<T> moved;
    moved.pushBack(makeObject<T>(42));
    moved = mini::moveCast(source);

    TEST_ENSURE(testArray(moved, vec) == 0);
    TEST_ENSURE(moved.data() == data);
    TEST_ENSURE(source.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testCapacity()
{
    Array<mini::int32> arr;
    TEST_ENSURE(arr.capacity() == 0);

    arr = makeSequence(100);
    TEST_ENSURE(arr.capacity() == 128);

    Array<mini::int32> reserved(100);
    reserved.append(makeSequence(100));
    TEST_ENSURE(reserved.size() == 100);
    TEST_ENSURE(reserved.capacity() == 100);

    reserved.pushBack(100);
    TEST_ENSURE(reserved.size() == 101);
    TEST_ENSURE(reserved.capacity() == 200);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFirst()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    TEST_ENSURE(arr.first() == 0);
    TEST_ENSURE(carr.first() == 0);

    arr.first() = -1;
    TEST_ENSURE(carr[0] == -1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testLast()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    TEST_ENSURE(arr.last() == 99);
    TEST_ENSURE(carr.last() == 99);

    arr.last() = -2;
    TEST_ENSURE(carr[99] == -2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testAt()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    TEST_ENSURE(arr.at(42) == 42);
    TEST_ENSURE(carr.at(42) == 42);

    arr.at(1) = -3;
    TEST_ENSURE(carr[1] == -3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubscript()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    TEST_ENSURE(arr[57] == 57);
    TEST_ENSURE(carr[57] == 57);

    arr[2] = -4;
    TEST_ENSURE(carr.at(2) == -4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testData()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    TEST_ENSURE(arr.data() == carr.data());
    TEST_ENSURE(carr.data()[13] == 13);

    arr.data()[50] = 100;
    TEST_ENSURE(carr[50] == 100);

    return 0;
}

[[no_inline]] constexpr mini::int32 testBeginEnd()
{
    Array<mini::int32> empty;
    Array<mini::int32> const& cempty = empty;
    TEST_ENSURE(empty.begin() == empty.end());
    TEST_ENSURE(cempty.begin() == cempty.end());

    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;
    TEST_ENSURE(*arr.begin() == 0);
    TEST_ENSURE(*(carr.end() - 1) == 99);
    TEST_ENSURE(static_cast<mini::size_t>(arr.end() - arr.begin()) == arr.size());
    TEST_ENSURE(static_cast<mini::size_t>(carr.end() - carr.begin()) == carr.size());

    return 0;
}

[[no_inline]] constexpr mini::int32 testRangeFor()
{
    Array<mini::int32> arr = makeSequence(100);
    Array<mini::int32> const& carr = arr;

    mini::int32 sum = 0;
    for (mini::int32 value : carr) {
        sum += value;
    }
    TEST_ENSURE(sum == 4950);

    for (mini::int32& value : arr) {
        value *= 2;
    }
    TEST_ENSURE(arr[50] == 100);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidIndex()
{
    Array<mini::int32> empty;
    TEST_ENSURE(!empty.validIndex(0));

    Array<mini::int32> arr = makeSequence(100);
    TEST_ENSURE(arr.validIndex(0));
    TEST_ENSURE(arr.validIndex(99));
    TEST_ENSURE(!arr.validIndex(100));
    TEST_ENSURE(!arr.validIndex(110));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidIterator()
{
    Array<mini::int32> empty;
    TEST_ENSURE(!empty.validIterator(empty.begin()));

    Array<mini::int32> arr = makeSequence(100);
    TEST_ENSURE(arr.validIterator(arr.begin()));
    TEST_ENSURE(arr.validIterator(arr.end() - 1));
    TEST_ENSURE(!arr.validIterator(arr.end()));
    TEST_ENSURE(!arr.validIterator(arr.end() + 4));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidRange()
{
    Array<mini::int32> arr = makeSequence(100);
    TEST_ENSURE(arr.validRange(arr.begin(), arr.end()));
    TEST_ENSURE(arr.validRange(arr.begin() + 10, arr.begin() + 20));
    TEST_ENSURE(arr.validRange(arr.begin() + 10, arr.begin() + 10));

    return 0;
}

[[no_inline]] constexpr mini::int32 testEqual()
{
    Array<mini::int32> lhs;
    Array<mini::int32> rhs;
    TEST_ENSURE(lhs == rhs);

    lhs.pushBack(1);
    TEST_ENSURE(lhs != rhs);

    rhs.pushBack(2);
    TEST_ENSURE(lhs != rhs);

    rhs[0] = 1;
    TEST_ENSURE(lhs == rhs);
    TEST_ENSURE(lhs == lhs);

    return 0;
}

[[no_inline]] constexpr mini::int32 testAssignList()
{
    Array<mini::int32> arr;
    arr.pushBack(9);
    arr.assign(InitializerList<mini::int32>{1, 2, 3, 4});

    TEST_ENSURE(arr.size() == 4);
    TEST_ENSURE(arr.first() == 1);
    TEST_ENSURE(arr.last() == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFreeSwap()
{
    Array<mini::int32> lhs(InitializerList<mini::int32>{1, 2});
    Array<mini::int32> rhs(InitializerList<mini::int32>{3, 4, 5, 6});
    mini::int32 const* lhsData = lhs.data();
    mini::int32 const* rhsData = rhs.data();

    swap(lhs, rhs);
    TEST_ENSURE(lhs.size() == 4 && lhs.first() == 3);
    TEST_ENSURE(rhs.size() == 2 && rhs.first() == 1);
    TEST_ENSURE(lhs.data() == rhsData);
    TEST_ENSURE(rhs.data() == lhsData);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConvertToView()
{
    Array<mini::int32> arr(InitializerList<mini::int32>{1, 2, 3, 4});
    ArrayView<mini::int32> view = arr;

    TEST_ENSURE(view.size() == 4);
    TEST_ENSURE(view.data() == arr.data());

    return 0;
}

[[no_inline]] constexpr mini::int32 testViewCtor()
{
    Array<mini::int32> arr(InitializerList<mini::int32>{1, 2, 3, 4});
    ArrayView<mini::int32> view = arr;

    Array<mini::int32> fromView(view);
    TEST_ENSURE(fromView == arr);

    Array<mini::int32> allocated(view, mini::Allocator<mini::int32>{});
    TEST_ENSURE(allocated == arr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testViewCountCtor()
{
    Array<mini::int32> arr(InitializerList<mini::int32>{1, 2, 3, 4});
    ArrayView<mini::int32> view = arr;

    Array<mini::int32> counted(view, 2);
    TEST_ENSURE(counted.size() == 2);
    TEST_ENSURE(counted.last() == 2);

    Array<mini::int32> allocated(view, 3, mini::Allocator<mini::int32>{});
    TEST_ENSURE(allocated.size() == 3);
    TEST_ENSURE(allocated.last() == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConvertPointerView()
{
    mini::int32 values[] = {1, 2, 3};
    Array<mini::int32*> arr;
    arr.pushBack(values);
    arr.pushBack(values + 1);
    arr.pushBack(values + 2);

    ArrayView<mini::int32 const*> view = arr;
    TEST_ENSURE(view.size() == 3);
    TEST_ENSURE(static_cast<void const*>(view.data()) == static_cast<void const*>(arr.data()));
    TEST_ENSURE(view.first() == values);
    TEST_ENSURE(*view.last() == 3);

    arr[1] = values + 2;
    TEST_ENSURE(*view[1] == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testInsertRangeIteratorView()
{
    Array<mini::int32> source(InitializerList<mini::int32>{1, 2, 3, 4});
    Array<mini::int32> arr(InitializerList<mini::int32>{9, 9});
    ArrayView<mini::int32> view = source;

    arr.insertRange(arr.end(), view);
    TEST_ENSURE(arr.size() == 6);
    TEST_ENSURE(arr[2] == 1 && arr.last() == 4);

    arr.insertRange(arr.begin(), view);
    TEST_ENSURE(arr.size() == 10);
    TEST_ENSURE(arr.first() == 1 && arr[4] == 9);

    return 0;
}

[[no_inline]] constexpr mini::int32 testInsertRangeIteratorPosition()
{
    Array<mini::int32> source(InitializerList<mini::int32>{1, 2, 3, 4});
    Array<mini::int32> arr(InitializerList<mini::int32>{9, 9});

    arr.insertRange(arr.begin() + 1, source.begin(), source.begin() + 2);
    TEST_ENSURE(arr.size() == 4);
    TEST_ENSURE(arr[0] == 9 && arr[1] == 1 && arr[2] == 2 && arr[3] == 9);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAppendAliased()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.reserve(32);
    vec.reserve(32);

    arr.append(arr.begin(), arr.end());
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.append(arr);
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.append(arr.begin(), arr.end());
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.append(arr);
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertAliased()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 2);

    arr.reserve(4);
    vec.reserve(4);

    arr.insert(0, arr.first());
    vec.insert(vec.begin(), vec.front());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), arr[1]);
    vec.insert(vec.begin(), vec[1]);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertAliasedGrowth()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.insert(1, arr[3]);
    vec.insert(vec.begin() + 1, T(vec[3]));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.insert(arr.begin(), arr.last());
    vec.insert(vec.begin(), T(vec.back()));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.insert(arr.end() - 1, arr.first());
    vec.insert(vec.end() - 1, T(vec.front()));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.pushBack(arr[0]);
    vec.push_back(T(vec[0]));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.pushBack(arr.last());
    vec.push_back(T(vec.back()));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.insertRange(2, arr.begin() + 1, arr.begin() + 2);
    vec.insert(vec.begin() + 2, T(vec[1]));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeAliased()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.reserve(16);
    vec.reserve(16);

    arr.insertRange(arr.size(), arr.begin(), arr.end());
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.insertRange(arr.end(), arr.begin(), arr.end());
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.insertRange(0, arr.begin(), arr.begin() + 3);
    vec.insert(vec.begin(), vec.begin(), vec.begin() + 3);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.insertRange(arr.begin() + 5, arr.begin() + 2, arr.end() - 2);
    vec.insert(vec.begin() + 5, vec.begin() + 2, vec.end() - 2);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(arr.capacity() == arr.size());

    arr.insertRange(arr.begin() + 1, arr);
    vec.insert(vec.begin() + 1, vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeAliasedInPlace()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.reserve(32);
    TEST_ENSURE(arr.capacity() == 32);

    std::vector<T> source(vec.begin(), vec.begin() + 2);
    arr.insertRange(1, arr.begin(), arr.begin() + 2);
    vec.insert(vec.begin() + 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.begin(), vec.begin() + 3);
    arr.insertRange(arr.size() - 1, arr.begin(), arr.begin() + 3);
    vec.insert(vec.end() - 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.end() - 3, vec.end());
    arr.insertRange(arr.begin() + 2, arr.end() - 3, arr.end());
    vec.insert(vec.begin() + 2, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.begin() + 3, vec.begin() + 4);
    arr.insertRange(0, arr.begin() + 3, arr.begin() + 4);
    vec.insert(vec.begin(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.begin() + 1, vec.begin() + 5);
    arr.insertRange(3, ArrayView<T>(arr).subView(1, 4));
    vec.insert(vec.begin() + 3, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.end() - 2, vec.end());
    arr.insertRange(arr.end() - 1, ArrayView<T>(arr).subBack(2));
    vec.insert(vec.end() - 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    source.assign(vec.begin(), vec.end());
    arr.insertRange(1, arr);
    vec.insert(vec.begin() + 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.capacity() == 32);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAssignAliased()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr.reserve(16);

    arr.assign(arr.begin() + 2, arr.end() - 1);
    vec = std::vector<T>(vec.begin() + 2, vec.end() - 1);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.assign(ArrayView<T>(arr).subView(1, 3));
    vec = std::vector<T>(vec.begin() + 1, vec.begin() + 4);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.assign(arr);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    TEST_ENSURE(arr.size() == arr.capacity());
    arr.assign(arr.begin(), arr.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.assign(arr.begin() + 1, arr.end());
    vec = std::vector<T>(vec.begin() + 1, vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertInPlace()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.reserve(16);
    T const* data = arr.data();

    arr.insert(1, makeObject<T>(70));
    vec.insert(vec.begin() + 1, makeObject<T>(70));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin() + 3, makeArgument<T>(71));
    vec.emplace(vec.begin() + 3, makeArgument<T>(71));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.end() - 1, makeObject<T>(72));
    vec.insert(vec.end() - 1, makeObject<T>(72));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(0, makeArgument<T>(73));
    vec.emplace(vec.begin(), makeArgument<T>(73));
    TEST_ENSURE(testArray(arr, vec) == 0);

    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(arr.capacity() == 16);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeInPlace()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 6);

    Array<T> source;
    for (mini::int32 i = 0; i < 3; ++i) {
        source.pushBack(makeObject<T>(80 + i));
    }

    arr.reserve(32);
    T const* data = arr.data();

    arr.insertRange(1, source.begin(), source.end());
    vec.insert(vec.begin() + 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.end() - 1, source.begin(), source.end());
    vec.insert(vec.end() - 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.size() - 3, ArrayView<T>(source));
    vec.insert(vec.end() - 3, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin() + 2, ArrayView<T>(source).subFront(2));
    vec.insert(vec.begin() + 2, source.begin(), source.begin() + 2);
    TEST_ENSURE(testArray(arr, vec) == 0);

    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(arr.capacity() == 32);
    TEST_ENSURE(source.size() == 3);
    TEST_ENSURE(source.first() == makeObject<T>(80));
    TEST_ENSURE(source.last() == makeObject<T>(82));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAssignInPlace()
{
    Array<T> source;
    std::vector<T> expected;
    for (mini::int32 i = 0; i < 8; ++i) {
        source.pushBack(makeObject<T>(50 + i));
        expected.push_back(makeObject<T>(50 + i));
    }

    Array<T> arr;
    std::vector<T> unused;
    fill<T>(arr, unused, 2);
    arr.reserve(16);
    T const* data = arr.data();

    arr.assign(source.begin(), source.end());
    TEST_ENSURE(testArray(arr, expected) == 0);
    TEST_ENSURE(arr.data() == data);

    arr.assign(ArrayView<T>(source).subFront(3));
    TEST_ENSURE(testArray(arr, std::vector<T>(expected.begin(), expected.begin() + 3)) == 0);
    TEST_ENSURE(arr.data() == data);

    arr.assign(source);
    TEST_ENSURE(testArray(arr, expected) == 0);
    TEST_ENSURE(arr.data() == data);

    arr.assign(ArrayView<T>{});
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == 16);

    arr.assign(source.begin() + 2, source.begin() + 5);
    TEST_ENSURE(testArray(arr, std::vector<T>(expected.begin() + 2, expected.begin() + 5)) == 0);

    arr.assign(source.begin(), source.begin());
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == 16);

    TEST_ENSURE(testArray(source, expected) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyAssignInPlace()
{
    Array<T> source;
    std::vector<T> expected;
    for (mini::int32 i = 0; i < 8; ++i) {
        source.pushBack(makeObject<T>(50 + i));
        expected.push_back(makeObject<T>(50 + i));
    }

    Array<T> arr;
    std::vector<T> unused;
    fill<T>(arr, unused, 3);
    arr.reserve(16);
    T const* data = arr.data();

    arr = source;
    TEST_ENSURE(testArray(arr, expected) == 0);
    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(arr.data() != source.data());

    arr = ArrayView<T>(source).subBack(2);
    TEST_ENSURE(testArray(arr, std::vector<T>(expected.end() - 2, expected.end())) == 0);
    TEST_ENSURE(arr.data() == data);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testResizeInPlace()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    arr.reserve(16);
    T const* data = arr.data();

    arr.resize(10, makeArgument<T>(60));
    vec.resize(10, makeObject<T>(60));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.data() == data);

    arr.resize(12, arr.first());
    vec.resize(12, T(vec.front()));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.data() == data);

    arr.resize(3, makeArgument<T>(61));
    vec.resize(3, makeObject<T>(61));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.capacity() == 16);

    arr.shrink();
    arr.resize(6, arr.last());
    vec.resize(6, T(vec.back()));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeLargeGrowth()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 2);
    arr.shrink();

    Array<T> source;
    for (mini::int32 i = 0; i < 20; ++i) {
        source.pushBack(makeObject<T>(60 + i));
    }

    arr.insertRange(1, source.begin(), source.end());
    vec.insert(vec.begin() + 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.capacity() >= arr.size());

    arr.shrink();
    arr.insertRange(arr.begin() + 1, ArrayView<T>(source));
    vec.insert(vec.begin() + 1, source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.capacity() >= arr.size());

    arr.shrink();
    arr.append(source.begin(), source.end());
    vec.insert(vec.end(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.capacity() >= arr.size());

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInsertRangeViewEdge()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    Array<T> source;
    source.pushBack(makeObject<T>(90));
    source.pushBack(makeObject<T>(91));

    arr.insertRange(1, ArrayView<T>{});
    arr.insertRange(arr.begin() + 2, ArrayView<T>{});
    arr.insertRange(arr.size(), ArrayView<T>{});
    arr.insertRange(arr.end(), ArrayView<T>{});
    arr.insertRange(1, source.begin(), source.begin());
    arr.insertRange(arr.begin() + 1, source.end(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(1, ArrayView<T>(source).subFront(1));
    vec.insert(vec.begin() + 1, source.first());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin() + 3, ArrayView<T>(source).subBack(1));
    vec.insert(vec.begin() + 3, source.last());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.size(), ArrayView<T>(source).subBack(1));
    vec.insert(vec.end(), source.last());
    TEST_ENSURE(testArray(arr, vec) == 0);

    TEST_ENSURE(source.size() == 2);
    TEST_ENSURE(source.first() == makeObject<T>(90));
    TEST_ENSURE(source.last() == makeObject<T>(91));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAppendEdge()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 4);

    Array<T> source;
    source.pushBack(makeObject<T>(90));
    source.pushBack(makeObject<T>(91));

    arr.append(ArrayView<T>{});
    arr.append(Array<T>{});
    arr.append(source.begin(), source.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.reserve(16);
    T const* data = arr.data();

    arr.append(ArrayView<T>(source).subFront(1));
    vec.push_back(source.first());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.append(source);
    vec.insert(vec.end(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.append(source.begin(), source.end());
    vec.insert(vec.end(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(source.size() == 2);
    TEST_ENSURE(source.first() == makeObject<T>(90));
    TEST_ENSURE(source.last() == makeObject<T>(91));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyAssignSelf()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    arr = makeLvalueReference(arr);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveAssignSelf()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    T const* data = arr.data();
    arr = mini::moveCast(makeLvalueReference(arr));
    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(testArray(arr, vec) == 0);

    Array<T> empty;
    empty = mini::moveCast(makeLvalueReference(empty));
    TEST_ENSURE(empty.empty());
    TEST_ENSURE(empty.capacity() == 0);

    empty.pushBack(makeObject<T>(7));
    TEST_ENSURE(empty.size() == 1);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveIterator()
{
    Array<T> source;
    std::vector<T> vec;
    fill<T>(source, vec, 6);

    Array<T> arr(mini::MoveIterator(source.begin()), mini::MoveIterator(source.end()));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(source.size() == arr.size());

    Array<T> appended;
    appended.pushBack(makeObject<T>(5));
    appended.append(mini::MoveIterator(arr.begin() + 4), mini::MoveIterator(arr.end()));
    TEST_ENSURE(appended.size() == 3);
    TEST_ENSURE(appended.first() == makeObject<T>(5));
    TEST_ENSURE(appended[1] == vec[4]);
    TEST_ENSURE(appended.last() == vec.back());

    Array<T> inserted;
    inserted.pushBack(makeObject<T>(6));
    inserted.pushBack(makeObject<T>(7));
    inserted.insertRange(1, mini::MoveIterator(arr.begin()), mini::MoveIterator(arr.begin() + 2));
    inserted.insertRange(inserted.begin(), mini::MoveIterator(arr.begin() + 2), mini::MoveIterator(arr.begin() + 3));
    TEST_ENSURE(inserted.size() == 5);
    TEST_ENSURE(inserted[0] == vec[2]);
    TEST_ENSURE(inserted[1] == makeObject<T>(6));
    TEST_ENSURE(inserted[2] == vec[0]);
    TEST_ENSURE(inserted[3] == vec[1]);
    TEST_ENSURE(inserted[4] == makeObject<T>(7));

    Array<T> assigned;
    assigned.pushBack(makeObject<T>(8));
    assigned.assign(mini::MoveIterator(appended.begin()), mini::MoveIterator(appended.end()));
    TEST_ENSURE(assigned.size() == 3);
    TEST_ENSURE(assigned.first() == makeObject<T>(5));
    TEST_ENSURE(assigned[1] == vec[4]);
    TEST_ENSURE(assigned.last() == vec.back());

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testSingleElementRangeKeepsSource()
{
    Array<T> source;
    std::vector<T> vec;
    fill<T>(source, vec, 4);

    Array<T> appended;
    appended.append(source.begin() + 1, source.begin() + 2);
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(appended.size() == 1 && appended.first() == vec[1]);

    Array<T> indexed;
    indexed.pushBack(makeObject<T>(9));
    indexed.insertRange(0, source.begin() + 2, source.begin() + 3);
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(indexed.size() == 2 && indexed.first() == vec[2]);

    Array<T> iterated;
    iterated.pushBack(makeObject<T>(9));
    iterated.insertRange(iterated.begin(), source.begin() + 3, source.end());
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(iterated.size() == 2 && iterated.first() == vec[3]);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMovedFromReuse()
{
    Array<T> arr;
    std::vector<T> vec;
    fill<T>(arr, vec, 8);

    Array<T> moved(mini::moveCast(arr));
    TEST_ENSURE(arr.empty());
    arr.pushBack(makeObject<T>(1));
    arr.pushBack(makeObject<T>(2));
    TEST_ENSURE(arr.size() == 2);
    TEST_ENSURE(arr.first() == makeObject<T>(1));
    TEST_ENSURE(arr.last() == makeObject<T>(2));
    TEST_ENSURE(testArray(moved, vec) == 0);

    Array<T> assigned;
    assigned.pushBack(makeObject<T>(3));
    assigned = mini::moveCast(moved);
    TEST_ENSURE(moved.empty());
    TEST_ENSURE(testArray(assigned, vec) == 0);

    moved.pushBack(makeObject<T>(4));
    TEST_ENSURE(moved.size() == 1);
    TEST_ENSURE(moved.first() == makeObject<T>(4));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyEmpty()
{
    Array<T> empty;

    Array<T> copied(empty);
    TEST_ENSURE(copied.empty());
    TEST_ENSURE(copied == empty);

    Array<T> allocated(empty, mini::Allocator<T>{});
    TEST_ENSURE(allocated.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopBackEmpty()
{
    Array<mini::int32> arr;
    arr.popBack();
    arr.popBack(3);
    TEST_ENSURE(arr.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopBackOverflow()
{
    Array<mini::int32> arr = makeSequence(100);
    arr.popBack(1000);
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() >= 100);

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopBackCountEdge()
{
    Array<mini::int32> arr = makeSequence(10);

    arr.popBack(0);
    TEST_ENSURE(arr == makeSequence(10));

    arr.popBack(10);
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() >= 10);

    arr.pushBack(7);
    TEST_ENSURE(arr.size() == 1 && arr.first() == 7);

    return 0;
}

[[no_inline]] constexpr mini::int32 testRemoveRangeBoundary()
{
    Array<mini::int32> arr = makeSequence(100);

    arr.removeRange(arr.begin() + 3, arr.begin() + 3);
    TEST_ENSURE(arr.size() == 100);

    arr.removeRange(arr.begin() + 3, arr.begin() + 4);
    TEST_ENSURE(arr.size() == 99);
    TEST_ENSURE(arr[3] == 4);

    arr.removeRange(arr.end() - 3, arr.end());
    TEST_ENSURE(arr.size() == 96);
    TEST_ENSURE(arr.last() == 96);

    arr.removeRange(arr.begin(), arr.end());
    TEST_ENSURE(arr.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidRangeInvalid()
{
    Array<mini::int32> arr = makeSequence(8);
    arr.reserve(16);

    TEST_ENSURE(!arr.validRange(arr.begin(), arr.end() + 1));
    TEST_ENSURE(!arr.validRange(arr.end() + 1, arr.end() + 2));
    TEST_ENSURE(!arr.validRange(arr.begin() + 5, arr.begin() + 2));

    return 0;
}

[[no_inline]] constexpr mini::int32 testResizeDefault()
{
    Array<mini::int32> arr;

    arr.resize(10, 7);
    TEST_ENSURE(arr.size() == 10);
    TEST_ENSURE(arr.first() == 7 && arr.last() == 7);

    arr.resize(4);
    TEST_ENSURE(arr.size() == 4);

    arr.resize(6);
    TEST_ENSURE(arr.size() == 6);
    TEST_ENSURE(arr[3] == 7);
    TEST_ENSURE(arr[5] == 0);

    arr.resize(0);
    TEST_ENSURE(arr.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testResizeSame()
{
    Array<mini::int32> arr = makeSequence(8);
    mini::int32 const* data = arr.data();
    mini::size_t capacity = arr.capacity();

    arr.resize(8, 99);
    TEST_ENSURE(arr.size() == 8);
    TEST_ENSURE(arr.capacity() == capacity);
    TEST_ENSURE(arr.data() == data);
    TEST_ENSURE(arr == makeSequence(8));

    arr.resize(8);
    TEST_ENSURE(arr == makeSequence(8));

    return 0;
}

[[no_inline]] constexpr mini::int32 testResizeMoveOnly()
{
    Array<MoveOnlyObject> arr;

    arr.resize(4);
    TEST_ENSURE(arr.size() == 4);

    arr.resize(8, 5);
    TEST_ENSURE(arr.size() == 8);
    TEST_ENSURE(arr[3] == MoveOnlyObject());
    TEST_ENSURE(arr[4] == MoveOnlyObject(5));
    TEST_ENSURE(arr.last() == MoveOnlyObject(5));

    arr.resize(2, 6);
    TEST_ENSURE(arr.size() == 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testReserveSmaller()
{
    Array<mini::int32> arr;

    arr.reserve(256);
    TEST_ENSURE(arr.capacity() == 256);

    arr.reserve(8);
    TEST_ENSURE(arr.capacity() == 256);

    return 0;
}

[[no_inline]] constexpr mini::int32 testShrinkEmpty()
{
    Array<mini::int32> empty;
    empty.shrink();
    TEST_ENSURE(empty.empty());
    TEST_ENSURE(empty.capacity() == 0);

    Array<mini::int32> arr = makeSequence(10);
    arr.clear();
    arr.shrink();
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == 0);

    arr.pushBack(3);
    arr.pushBack(4);
    TEST_ENSURE(arr.size() == 2);
    TEST_ENSURE(arr.first() == 3 && arr.last() == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testClearCapacity()
{
    Array<mini::int32> empty;
    empty.clear();
    TEST_ENSURE(empty.empty());

    Array<mini::int32> arr = makeSequence(100);
    mini::size_t capacity = arr.capacity();
    arr.clear();
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.capacity() == capacity);

    return 0;
}

[[no_inline]] constexpr mini::int32 testViewCountCtorOverflow()
{
    Array<mini::int32> arr(InitializerList<mini::int32>{1, 2, 3, 4});
    ArrayView<mini::int32> view = arr;

    Array<mini::int32> counted(view, 10);
    TEST_ENSURE(counted.size() == 4);
    TEST_ENSURE(counted.capacity() == 10);
    TEST_ENSURE(counted == arr);

    counted.pushBack(5);
    TEST_ENSURE(counted.capacity() == 10);
    TEST_ENSURE(counted.last() == 5);

    Array<mini::int32> zero(view, 0);
    TEST_ENSURE(zero.empty());
    TEST_ENSURE(zero.capacity() == 0);

    return 0;
}

[[no_inline]] constexpr mini::int32 testEmptySourceCtor()
{
    Array<mini::int32> fromView(ArrayView<mini::int32>{});
    TEST_ENSURE(fromView.empty());
    TEST_ENSURE(fromView.capacity() == 0);

    Array<mini::int32> fromCountedView(ArrayView<mini::int32>{}, 4);
    TEST_ENSURE(fromCountedView.empty());
    TEST_ENSURE(fromCountedView.capacity() == 4);

    Array<mini::int32> source = makeSequence(4);
    Array<mini::int32> fromRange(source.begin() + 2, source.begin() + 2);
    TEST_ENSURE(fromRange.empty());
    TEST_ENSURE(fromRange.capacity() == 0);

    return 0;
}

mini::int32 testLifetimePush()
{
    initializeCounter();
    {
        Array<CopyableObject> arr;
        CopyableObject object(1);

        arr.pushBack(object);
        TEST_ENSURE(copyCtor == 1);

        arr.pushBack(CopyableObject(2));
        TEST_ENSURE(copyCtor == 1);

        arr.pushBack(6);
        TEST_ENSURE(copyCtor == 1);

        for (mini::int32 i = 0; i < 64; ++i) {
            arr.pushBack(4);
        }

        TEST_ENSURE(copyCtor == 1);
        TEST_ENSURE(arr.size() == 67);
        TEST_ENSURE(arr[0] == object);
        TEST_ENSURE(arr[1] == CopyableObject(2));
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

mini::int32 testLifetimeBalance()
{
    initializeCounter();
    {
        Array<CopyableObject> arr;
        CopyableObject object(1);

        for (mini::int32 i = 0; i < 64; ++i) {
            arr.pushBack(4);
        }

        arr.insert(0, object);
        arr.insert(arr.begin() + 10, 3);
        TEST_ENSURE(arr.first() == object);
        TEST_ENSURE(arr[10] == CopyableObject(3));

        CopyableObject last = arr.popLast();
        TEST_ENSURE(last == CopyableObject(4));

        arr.remove(5);
        arr.removeRange(arr.begin(), arr.begin() + 10);
        arr.popBack(3);
        arr.resize(80, 5);
        TEST_ENSURE(arr.last() == CopyableObject(5));
        arr.resize(5);
        arr.shrink();
        TEST_ENSURE(arr.capacity() == 5);

        Array<CopyableObject> copied(arr);
        TEST_ENSURE(copied == arr);

        Array<CopyableObject> moved(mini::moveCast(copied));
        TEST_ENSURE(moved == arr);

        moved.clear();
        TEST_ENSURE(moved.empty());

        Array<CopyableObject> assigned;
        assigned = arr;
        assigned = mini::moveCast(arr);
        TEST_ENSURE(arr.empty());
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

mini::int32 testLifetimeGrowth()
{
    initializeCounter();
    {
        Array<CopyableObject> arr(4);
        arr.pushBack(7);
        arr.pushBack(8);
        arr.pushBack(9);
        TEST_ENSURE(ctor == 3);
        TEST_ENSURE(copyCtor == 0);
        TEST_ENSURE(moveCtor == 0);

        arr.pushBack(10);
        arr.pushBack(11);
        TEST_ENSURE(ctor == 5);
        TEST_ENSURE(copyCtor == 0);
        TEST_ENSURE(moveCtor == 4);
        TEST_ENSURE(arr.capacity() == 8);
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

mini::int32 testLifetimeRange()
{
    initializeCounter();
    {
        Array<CopyableObject> source;
        for (mini::int32 i = 0; i < 4; ++i) {
            source.pushBack(10 + i);
        }

        Array<CopyableObject> arr(32);
        for (mini::int32 i = 0; i < 6; ++i) {
            arr.pushBack(i);
        }

        mini::size_t copies = copyCtor;
        arr.insertRange(1, source.begin(), source.end());
        TEST_ENSURE(copyCtor == copies + 4);

        copies = copyCtor;
        arr.insertRange(arr.end() - 1, ArrayView<CopyableObject>(source));
        TEST_ENSURE(copyCtor == copies + 4);

        copies = copyCtor;
        arr.append(source);
        TEST_ENSURE(copyCtor == copies + 4);
        TEST_ENSURE(arr.size() == 18);
        TEST_ENSURE(arr.capacity() == 32);

        arr.removeRange(arr.begin() + 2, arr.begin() + 7);
        arr.remove(0);
        TEST_ENSURE(arr.size() == 12);

        arr.shrink();
        arr.insertRange(3, source.begin(), source.end());
        arr.append(source.begin(), source.end());
        TEST_ENSURE(arr.size() == 20);

        arr.assign(source.begin(), source.begin() + 2);
        TEST_ENSURE(arr.size() == 2);

        arr.assign(source);
        TEST_ENSURE(arr == source);

        arr.popBack(10);
        TEST_ENSURE(arr.empty());

        arr.assign(source.begin(), source.end());
        TEST_ENSURE(arr == source);
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

mini::int32 testCountingAlloc()
{
    using CountingArray = Array<CopyableObject, BufferCountingAlloc<CopyableObject>>;

    liveBufferCount = 0;
    {
        CountingArray arr(8);
        for (mini::int32 i = 0; i < 8; ++i) {
            arr.pushBack(i);
        }
        TEST_ENSURE(liveBufferCount == 1);

        CountingArray copied(arr);
        TEST_ENSURE(copied == arr);
        TEST_ENSURE(liveBufferCount == 2);

        CountingArray copiedAlloc(arr, BufferCountingAlloc<CopyableObject>{});
        TEST_ENSURE(copiedAlloc == arr);
        TEST_ENSURE(liveBufferCount == 3);

        CountingArray moved(mini::moveCast(copied));
        TEST_ENSURE(moved == arr);
        TEST_ENSURE(copied.empty());
        TEST_ENSURE(copied.capacity() == 0);
        TEST_ENSURE(liveBufferCount == 3);

        CountingArray movedAlloc(mini::moveCast(copiedAlloc), BufferCountingAlloc<CopyableObject>{});
        TEST_ENSURE(movedAlloc == arr);
        TEST_ENSURE(copiedAlloc.empty());
        TEST_ENSURE(liveBufferCount == 3);

        CountingArray assigned(2);
        assigned.pushBack(100);
        TEST_ENSURE(liveBufferCount == 4);

        assigned = mini::moveCast(moved);
        TEST_ENSURE(assigned == arr);
        TEST_ENSURE(moved.empty());
        TEST_ENSURE(liveBufferCount == 4);

        CountingArray swapped;
        swapped.swap(assigned);
        TEST_ENSURE(swapped == arr);
        TEST_ENSURE(assigned.empty());
        TEST_ENSURE(liveBufferCount == 4);

        Array<CopyableObject> plain(arr.begin(), arr.end());
        TEST_ENSURE(plain == arr);
        TEST_ENSURE(arr == plain);
        TEST_ENSURE(liveBufferCount == 4);

        plain.pushBack(8);
        TEST_ENSURE(plain != arr);
        TEST_ENSURE(arr != plain);
    }
    TEST_ENSURE(liveBufferCount == 0);

    return 0;
}

mini::int32 testCountingAllocBuffer()
{
    using CountingArray = Array<CopyableObject, BufferCountingAlloc<CopyableObject>>;

    liveBufferCount = 0;
    {
        CountingArray arr;
        TEST_ENSURE(liveBufferCount == 0);

        for (mini::int32 i = 0; i < 20; ++i) {
            arr.pushBack(i);
        }
        TEST_ENSURE(liveBufferCount == 1);

        arr.reserve(64);
        TEST_ENSURE(liveBufferCount == 1);

        arr.shrink();
        TEST_ENSURE(liveBufferCount == 1);
        TEST_ENSURE(arr.capacity() == 20);

        arr.insert(3, 100);
        TEST_ENSURE(liveBufferCount == 1);

        CountingArray source(arr);
        TEST_ENSURE(liveBufferCount == 2);

        arr.shrink();
        arr.insertRange(1, source.begin(), source.end());
        TEST_ENSURE(liveBufferCount == 2);

        arr.shrink();
        arr.append(source);
        TEST_ENSURE(liveBufferCount == 2);

        arr.resize(arr.capacity() + 10, 7);
        TEST_ENSURE(liveBufferCount == 2);

        arr.assign(source.begin(), source.end());
        TEST_ENSURE(liveBufferCount == 2);
        TEST_ENSURE(arr == source);

        arr.clear();
        TEST_ENSURE(liveBufferCount == 2);

        source = mini::moveCast(arr);
        TEST_ENSURE(liveBufferCount == 2);
    }
    TEST_ENSURE(liveBufferCount == 0);

    return 0;
}

mini::int32 main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAllocCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAllocCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAllocCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testAllocCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAllocCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCapacityCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCapacityCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCapacityCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCapacityCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCapacityCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPushBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAppendList, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAppendList, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendList, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendList, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertIndex, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertIndex, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIndex, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIndex, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIndex, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertIterator, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertIterator, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIterator, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIterator, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertIterator, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIterator, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIterator, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIterator, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIterator, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeList, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeList, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeList, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeList, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIndexArray, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIndexArray, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIndexArray, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeIndexArray, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopLast, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopLast, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopLast, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopLast, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopLast, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRemoveIndex, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIndex, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIndex, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIndex, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIndex, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRemoveIterator, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIterator, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIterator, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIterator, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveIterator, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignRange, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignRange, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignRange, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testResize, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testResize, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testResize, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testResize, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testReserve, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testReserve, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testReserve, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testReserve, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testReserve, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testShrink, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testShrink, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testShrink, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testShrink, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testShrink, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSwap, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSwap, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSwap, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testSwap, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testSwap, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testClear, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testClear, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testClear, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testClear, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testClear, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, OveralignedObject);

    TEST_CONSTEXPR(testCapacity);
    TEST_CONSTEXPR(testFirst);
    TEST_CONSTEXPR(testLast);
    TEST_CONSTEXPR(testAt);
    TEST_CONSTEXPR(testSubscript);
    TEST_CONSTEXPR(testData);
    TEST_CONSTEXPR(testBeginEnd);
    TEST_CONSTEXPR(testRangeFor);
    TEST_CONSTEXPR(testValidIndex);
    TEST_CONSTEXPR(testValidIterator);
    TEST_CONSTEXPR(testValidRange);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testAssignList);
    TEST_CONSTEXPR(testFreeSwap);

    TEST_CONSTEXPR(testConvertToView);
    TEST_CONSTEXPR(testViewCtor);
    TEST_CONSTEXPR(testViewCountCtor);
    TEST_CONSTEXPR(testConvertPointerView);
    TEST_CONSTEXPR(testInsertRangeIteratorView);
    TEST_CONSTEXPR(testInsertRangeIteratorPosition);

    TEST_CONSTEXPR_TEMPLATE(testAppendAliased, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAppendAliased, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendAliased, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendAliased, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertAliasedGrowth, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliasedGrowth, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliasedGrowth, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliasedGrowth, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliased, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliased, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliased, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliased, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliasedInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliasedInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliasedInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAliasedInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignAliased, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignAliased, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignAliased, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignAliased, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertInPlace, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssignInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testResizeInPlace, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testResizeInPlace, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testResizeInPlace, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testResizeInPlace, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeLargeGrowth, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeLargeGrowth, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeLargeGrowth, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeLargeGrowth, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeViewEdge, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeViewEdge, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeViewEdge, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeViewEdge, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAppendEdge, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAppendEdge, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendEdge, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAppendEdge, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveIterator, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveIterator, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveIterator, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveIterator, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveIterator, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMovedFromReuse, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMovedFromReuse, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMovedFromReuse, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMovedFromReuse, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testMovedFromReuse, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyEmpty, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyEmpty, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyEmpty, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyEmpty, OveralignedObject);

    TEST_CONSTEXPR(testPopBackEmpty);
    TEST_CONSTEXPR(testPopBackOverflow);
    TEST_CONSTEXPR(testPopBackCountEdge);

    TEST_CONSTEXPR(testRemoveRangeBoundary);
    TEST_CONSTEXPR(testValidRangeInvalid);

    TEST_CONSTEXPR(testResizeDefault);
    TEST_CONSTEXPR(testResizeSame);
    TEST_CONSTEXPR(testResizeMoveOnly);
    TEST_CONSTEXPR(testReserveSmaller);
    TEST_CONSTEXPR(testShrinkEmpty);
    TEST_CONSTEXPR(testClearCapacity);

    TEST_CONSTEXPR(testViewCountCtorOverflow);
    TEST_CONSTEXPR(testEmptySourceCtor);

    TEST_FUNCTION(testLifetimePush);
    TEST_FUNCTION(testLifetimeBalance);
    TEST_FUNCTION(testLifetimeGrowth);
    TEST_FUNCTION(testLifetimeRange);
    TEST_FUNCTION(testCountingAlloc);
    TEST_FUNCTION(testCountingAllocBuffer);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
