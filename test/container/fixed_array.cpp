#include <vector>

#include "test_macro.h"

import mini.test;

using mini::ArrayView;
using mini::FixedArray;
using mini::InitializerList;

template <typename T, size_t CapN, typename StdAllocT>
constexpr int testArray(FixedArray<T, CapN> const& arr, std::vector<T, StdAllocT> const& vec)
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

template <typename T, size_t CapN>
constexpr void fill(FixedArray<T, CapN>& arr, std::vector<T>& vec, size_t count)
{
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        arr.pushBack(makeObject<T>(++arrcount));
        vec.push_back(makeObject<T>(++veccount));
    }
}

template <typename T>
constexpr int testCtor()
{
    TEST_ENSURE((FixedArray<T, 1>{}.empty()));
    TEST_ENSURE((FixedArray<T, 1>{}.capacity() == 1));
    TEST_ENSURE((FixedArray<T, 16>().empty()));
    TEST_ENSURE((FixedArray<T, 16>().capacity() == 16));

    return 0;
}

template <typename T>
constexpr int testCopyCtor()
{
    FixedArray<T, 20> arr;
    std::vector<T> vec;
    fill<T, 20>(arr, vec, 20);

    FixedArray<T, 20> copied(arr);
    TEST_ENSURE(copied == arr);
    TEST_ENSURE(testArray(copied, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testMoveCtor()
{
    FixedArray<T, 20> arr;
    std::vector<T> vec;
    fill<T, 20>(arr, vec, 20);

    FixedArray<T, 20> moved(mini::moveCast(arr));
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(testArray(moved, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testMoveIteratorCtor()
{
    FixedArray<T, 8> source;
    std::vector<T> vec;
    fill<T, 8>(source, vec, 6);

    FixedArray<T, 8> arr(mini::MoveIterator(source.begin()), mini::MoveIterator(source.end()));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(source.size() == arr.size());

    FixedArray<T, 8> appended;
    appended.pushBack(makeObject<T>(5));
    appended.append(mini::MoveIterator(arr.begin() + 4), mini::MoveIterator(arr.end()));
    TEST_ENSURE(appended.size() == 3);
    TEST_ENSURE(appended.first() == makeObject<T>(5));
    TEST_ENSURE(appended[1] == vec[4]);
    TEST_ENSURE(appended.last() == vec.back());

    FixedArray<T, 8> assigned;
    assigned.pushBack(makeObject<T>(7));
    assigned.assign(mini::MoveIterator(appended.begin()), mini::MoveIterator(appended.end()));
    TEST_ENSURE(assigned.size() == 3);
    TEST_ENSURE(assigned.first() == makeObject<T>(5));
    TEST_ENSURE(assigned[1] == vec[4]);
    TEST_ENSURE(assigned.last() == vec.back());

    return 0;
}

template <typename T>
constexpr int testRangeCtor()
{
    FixedArray<T, 20> arr;
    std::vector<T> vec;
    fill<T, 20>(arr, vec, 20);

    TEST_ENSURE((FixedArray<T, 20>(arr.begin(), arr.end()) == arr));
    TEST_ENSURE((FixedArray<T, 20>(arr.begin(), arr.begin()).empty()));

    return 0;
}

template <typename T>
constexpr int testInitializerListCtor()
{
    int count = 0;
    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    TEST_ENSURE(testArray(FixedArray<T, 20>(list), std::vector<T>(list)) == 0);

    FixedArray<T, 8> larger(list, 6);
    TEST_ENSURE(testArray(larger, std::vector<T>(list)) == 0);

    FixedArray<T, 8> smaller(list, 2);
    TEST_ENSURE(testArray(smaller, std::vector<T>(list.begin(), list.begin() + 2)) == 0);

    return 0;
}

template <typename T>
constexpr int testPushBack()
{
    FixedArray<T, 8> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.pushBack(makeObject<T>(++arrcount));
    vec.push_back(makeObject<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.pushBack(makeArgument<T>(++arrcount));
    vec.emplace_back(makeArgument<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testAppend()
{
    FixedArray<T, 8> arr;
    std::vector<T> vec;
    fill<T, 8>(arr, vec, 2);
    int count = 50;

    arr.append(arr.begin(), arr.end());
    vec.insert(vec.end(), vec.begin(), vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.append(arr.begin(), arr.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.append(ArrayView<T>());
    TEST_ENSURE(testArray(arr, vec) == 0);

    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    arr.append(list);
    vec.append_range(list);
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.full());

    return 0;
}

template <typename T>
constexpr int testInsert()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.insert(0, makeObject<T>(++arrcount));
    vec.insert(vec.begin(), makeObject<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(0, makeArgument<T>(++arrcount));
    vec.emplace(vec.begin(), makeArgument<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), makeObject<T>(++arrcount));
    vec.insert(vec.begin(), makeObject<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), makeArgument<T>(++arrcount));
    vec.emplace(vec.begin(), makeArgument<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testInsertRange()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    fill<T, 16>(arr, vec, 4);
    FixedArray<T, 16> arr2(arr);
    int count = 50;

    arr.insertRange(0, arr2.end() - 2, arr2.end());
    vec.insert(vec.begin(), arr2.end() - 2, arr2.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(1, arr2.begin(), arr2.begin() + 2);
    vec.insert(vec.begin() + 1, arr2.begin(), arr2.begin() + 2);
    TEST_ENSURE(testArray(arr, vec) == 0);

    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    arr.insertRange(2, list);
    vec.insert(vec.begin() + 2, list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testInsertRangeMiddle()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    fill<T, 32>(arr, vec, 6);
    FixedArray<T, 32> source(arr);

    arr.insertRange(3, source.begin(), source.begin() + 3);
    vec.insert(vec.begin() + 3, source.begin(), source.begin() + 3);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.size() - 2, source.begin(), source.begin() + 5);
    vec.insert(vec.end() - 2, source.begin(), source.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(1, source.begin() + 4, source.end());
    vec.insert(vec.begin() + 1, source.begin() + 4, source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.size() - 1, source.begin(), source.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.size(), source.begin(), source.end());
    vec.insert(vec.end(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(testArray(source, std::vector<T>(vec.end() - 6, vec.end())) == 0);

    return 0;
}

template <typename T>
constexpr int testInsertRangeAtIterator()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    fill<T, 32>(arr, vec, 4);
    FixedArray<T, 32> source(arr);
    int count = 60;

    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    arr.insertRange(arr.begin() + 2, list);
    vec.insert(vec.begin() + 2, list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.end(), list);
    vec.insert(vec.end(), list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin(), list);
    vec.insert(vec.begin(), list);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin() + 3, source.begin(), source.begin() + 3);
    vec.insert(vec.begin() + 3, source.begin(), source.begin() + 3);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.end(), source.begin(), source.end());
    vec.insert(vec.end(), source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin(), source.begin() + 1, source.begin() + 3);
    vec.insert(vec.begin(), source.begin() + 1, source.begin() + 3);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin() + 1, source.end() - 1, source.end());
    vec.insert(vec.begin() + 1, source.end() - 1, source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insertRange(arr.begin() + 1, source.begin(), source.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testPopBack()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    fill<T, 32>(arr, vec, 32);

    arr.popBack();
    vec.pop_back();
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.popBack(5);
    vec.erase(vec.end() - 5, vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testRemove()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    fill<T, 32>(arr, vec, 32);

    arr.remove(0);
    vec.erase(vec.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.remove(arr.begin() + 5);
    vec.erase(vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testRemoveRange()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    fill<T, 32>(arr, vec, 32);

    arr.removeRange(arr.begin() + 2, arr.begin() + 5);
    vec.erase(vec.begin() + 2, vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testAssign()
{
    FixedArray<T, 24> arr;
    std::vector<T> vec;
    fill<T, 24>(arr, vec, 8);

    FixedArray<T, 8> arr2;
    arr2.assign(arr.begin(), arr.end());
    TEST_ENSURE(arr == arr2);
    TEST_ENSURE(testArray(arr2, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testResize()
{
    FixedArray<T, 24> arr;
    std::vector<T> vec;
    fill<T, 24>(arr, vec, 8);
    int arrcount = 50;
    int veccount = 50;

    arr.resize(20, makeArgument<T>(++arrcount));
    vec.resize(20, makeObject<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.resize(24, makeObject<T>(++arrcount));
    vec.resize(24, makeObject<T>(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);
    TEST_ENSURE(arr.full());

    if constexpr (mini::DefaultConstructibleT<T>) {
        arr.resize(4);
        vec.resize(4);
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T>
constexpr int testCopyAssign()
{
    FixedArray<T, 16> source;
    std::vector<T> vec;
    fill<T, 16>(source, vec, 8);

    FixedArray<T, 16> copied;
    copied.pushBack(makeObject<T>(9));
    copied = source;
    TEST_ENSURE(testArray(copied, vec) == 0);
    TEST_ENSURE(testArray(source, vec) == 0);

    FixedArray<T, 16> full;
    std::vector<T> unused;
    fill<T, 16>(full, unused, 16);
    full = source;
    TEST_ENSURE(testArray(full, vec) == 0);

    FixedArray<T, 16> empty;
    full = empty;
    TEST_ENSURE(full.empty());

    return 0;
}

template <typename T>
constexpr int testMoveAssign()
{
    FixedArray<T, 16> source;
    std::vector<T> vec;
    fill<T, 16>(source, vec, 8);

    FixedArray<T, 16> moved;
    moved.pushBack(makeObject<T>(42));
    moved = mini::moveCast(source);
    TEST_ENSURE(testArray(moved, vec) == 0);
    TEST_ENSURE(source.empty());

    FixedArray<T, 16> full;
    std::vector<T> unused;
    fill<T, 16>(full, unused, 16);
    full = mini::moveCast(moved);
    TEST_ENSURE(testArray(full, vec) == 0);
    TEST_ENSURE(moved.empty());

    FixedArray<T, 16> emptySource;
    full = mini::moveCast(emptySource);
    TEST_ENSURE(full.empty());
    TEST_ENSURE(emptySource.empty());

    full.pushBack(makeObject<T>(43));
    TEST_ENSURE(full.size() == 1);
    TEST_ENSURE(full.first() == makeObject<T>(43));

    return 0;
}

template <typename T>
constexpr int testClear()
{
    FixedArray<T, 24> arr;
    std::vector<T> vec;
    fill<T, 24>(arr, vec, 8);

    arr.clear();
    vec.clear();
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.clear();
    TEST_ENSURE(arr.empty());

    return 0;
}

constexpr int testEmpty()
{
    FixedArray<mini::int32, 8> arr;
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(!arr.full());
    TEST_ENSURE(arr.size() == 0);
    TEST_ENSURE(arr.capacity() == 8);
    TEST_ENSURE(arr.begin() == arr.end());

    arr.popBack();
    arr.popBack(3);
    arr.clear();
    TEST_ENSURE(arr.empty());

    arr.pushBack(1);
    TEST_ENSURE(!arr.empty());

    return 0;
}

constexpr int testFull()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.full());
    TEST_ENSURE(arr.size() == 8);
    TEST_ENSURE(arr.capacity() == 8);

    arr.popBack();
    TEST_ENSURE(!arr.full());

    FixedArray<mini::int32, 1> single;
    single.pushBack(1);
    TEST_ENSURE(single.full());
    TEST_ENSURE(single.first() == single.last());

    return 0;
}

constexpr int testValidIndex()
{
    FixedArray<mini::int32, 8> arr;
    TEST_ENSURE(!arr.validIndex(0));

    for (mini::int32 i = 0; i < 4; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.validIndex(0));
    TEST_ENSURE(arr.validIndex(3));
    TEST_ENSURE(!arr.validIndex(4));

    return 0;
}

constexpr int testValidIterator()
{
    FixedArray<mini::int32, 8> arr;
    TEST_ENSURE(!arr.validIterator(arr.begin()));

    for (mini::int32 i = 0; i < 4; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.validIterator(arr.begin()));
    TEST_ENSURE(arr.validIterator(arr.end() - 1));
    TEST_ENSURE(!arr.validIterator(arr.end()));

    return 0;
}

constexpr int testValidRange()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 4; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.validRange(arr.begin(), arr.end()));
    TEST_ENSURE(arr.validRange(arr.begin() + 1, arr.begin() + 2));
    TEST_ENSURE(!arr.validRange(arr.end(), arr.end()));

    return 0;
}

constexpr int testFirstLast()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.first() == 0);
    TEST_ENSURE(arr.last() == 7);

    arr.first() = 100;
    arr.last() = 200;
    TEST_ENSURE(arr[0] == 100);
    TEST_ENSURE(arr[7] == 200);

    FixedArray<mini::int32, 8> const& carr = arr;
    TEST_ENSURE(carr.first() == 100);
    TEST_ENSURE(carr.last() == 200);

    return 0;
}

constexpr int testSubscript()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr[3] == 3);
    TEST_ENSURE(arr.at(5) == 5);

    arr[1] = 101;
    arr.at(2) = 102;
    TEST_ENSURE(arr.at(1) == 101);
    TEST_ENSURE(arr[2] == 102);

    FixedArray<mini::int32, 8> const& carr = arr;
    TEST_ENSURE(carr[1] == 101);
    TEST_ENSURE(carr.at(2) == 102);

    return 0;
}

constexpr int testData()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(arr.data()[7] == 7);
    TEST_ENSURE(arr.data() == &arr.first());

    arr.data()[2] = 102;
    TEST_ENSURE(arr[2] == 102);

    FixedArray<mini::int32, 8> const& carr = arr;
    TEST_ENSURE(carr.data() == arr.data());
    TEST_ENSURE(carr.data()[2] == 102);

    return 0;
}

constexpr int testIteration()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    TEST_ENSURE(static_cast<mini::size_t>(arr.end() - arr.begin()) == arr.size());

    mini::int32 sum = 0;
    for (mini::int32 value : arr) {
        sum += value;
    }
    TEST_ENSURE(sum == 28);

    for (mini::int32& value : arr) {
        value += 10;
    }
    TEST_ENSURE(arr[0] == 10);
    TEST_ENSURE(arr[7] == 17);

    FixedArray<mini::int32, 8> const& carr = arr;
    TEST_ENSURE(*carr.begin() == 10);
    TEST_ENSURE(*(carr.end() - 1) == 17);
    TEST_ENSURE(static_cast<mini::size_t>(carr.end() - carr.begin()) == carr.size());

    sum = 0;
    for (mini::int32 const& value : carr) {
        sum += value;
    }
    TEST_ENSURE(sum == 108);

    return 0;
}

constexpr int testViewCtor()
{
    InitializerList list = {1, 2, 3, 4};
    ArrayView<mini::int32> view = list;

    FixedArray<mini::int32, 8> fromView(view);
    TEST_ENSURE(fromView.size() == 4);
    TEST_ENSURE(fromView.first() == 1 && fromView.last() == 4);

    FixedArray<mini::int32, 8> fromViewCount(view, 3);
    TEST_ENSURE(fromViewCount.size() == 3);
    TEST_ENSURE(fromViewCount.last() == 3);

    FixedArray<mini::int32, 8> fromViewLargerCount(view, 6);
    TEST_ENSURE(fromViewLargerCount.size() == 4);
    TEST_ENSURE(fromViewLargerCount.last() == 4);

    return 0;
}

constexpr int testConvertToView()
{
    FixedArray<mini::int32, 8> arr;
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);

    ArrayView<mini::int32> view = arr;
    TEST_ENSURE(view.size() == 3);
    TEST_ENSURE(view.data() == arr.data());
    TEST_ENSURE(view.first() == 1);
    TEST_ENSURE(view.last() == 3);

    FixedArray<mini::int32, 8> empty;
    TEST_ENSURE(ArrayView<mini::int32>(empty).empty());

    mini::int32 first = 1;
    mini::int32 second = 2;
    FixedArray<mini::int32*, 4> ptrs;
    ptrs.pushBack(&first);
    ptrs.pushBack(&second);

    ArrayView<mini::int32 const*> ptrView = ptrs;
    TEST_ENSURE(ptrView.size() == 2);
    TEST_ENSURE(ptrView.data() == ptrs.data());
    TEST_ENSURE(ptrView.first() == &first);
    TEST_ENSURE(*ptrView.last() == 2);

    return 0;
}

constexpr int testInsertRangeView()
{
    InitializerList list = {1, 2, 3, 4};
    ArrayView<mini::int32> view = list;

    FixedArray<mini::int32, 16> arr;
    arr.append(view);
    arr.append(view);
    arr.insertRange(0, view);
    arr.insertRange(4, view);
    TEST_ENSURE(arr.full());
    TEST_ENSURE(arr[0] == 1 && arr[4] == 1 && arr[15] == 4);

    return 0;
}

constexpr int testAssignRange()
{
    InitializerList list = {1, 2, 3, 4};
    FixedArray<mini::int32, 8> source(list);

    FixedArray<mini::int32, 16> arr(list);
    arr.assign(ArrayView<mini::int32>());
    TEST_ENSURE(arr.empty());

    arr.assign(list);
    TEST_ENSURE(arr == source);

    arr.assign(source.begin(), source.begin());
    TEST_ENSURE(arr.empty());

    return 0;
}

constexpr int testViewAssign()
{
    InitializerList list = {1, 2, 3, 4};
    FixedArray<mini::int32, 8> source(list);

    FixedArray<mini::int32, 8> arr;
    arr = ArrayView<mini::int32>(list);
    TEST_ENSURE(arr == source);

    arr = ArrayView<mini::int32>(list).subFront(2);
    TEST_ENSURE(arr.size() == 2);
    TEST_ENSURE(arr.last() == 2);

    return 0;
}

constexpr int testEqual()
{
    InitializerList list = {1, 2, 3, 4};
    FixedArray<mini::int32, 4> lhs(list);
    FixedArray<mini::int32, 16> rhs(list);
    FixedArray<mini::int32, 4> empty1;
    FixedArray<mini::int32, 8> empty2;

    TEST_ENSURE(lhs == lhs);
    TEST_ENSURE(lhs == rhs);
    TEST_ENSURE(rhs == lhs);
    TEST_ENSURE(empty1 == empty2);

    FixedArray<mini::int64, 8> wide;
    for (mini::int64 i = 1; i <= 4; ++i) {
        wide.pushBack(i);
    }
    TEST_ENSURE(lhs == wide);
    TEST_ENSURE(wide == lhs);

    wide.last() = 5;
    TEST_ENSURE(lhs != wide);

    return 0;
}

constexpr int testNotEqual()
{
    InitializerList list = {1, 2, 3, 4};
    FixedArray<mini::int32, 4> lhs(list);
    FixedArray<mini::int32, 16> rhs(list);
    FixedArray<mini::int32, 4> empty;

    TEST_ENSURE(lhs != empty);

    rhs.popBack();
    TEST_ENSURE(lhs != rhs);

    rhs.pushBack(5);
    TEST_ENSURE(lhs != rhs);

    FixedArray<mini::int32, 4> modified(list);
    modified[0] = 100;
    TEST_ENSURE(modified != lhs);

    return 0;
}

template <typename T>
constexpr int testEmptyCopyMoveCtor()
{
    FixedArray<T, 8> empty;
    if constexpr (mini::CopyableT<T>) {
        FixedArray<T, 8> copied(empty);
        TEST_ENSURE(copied.empty());
        copied.pushBack(makeObject<T>(3));
        TEST_ENSURE(copied.size() == 1);
        TEST_ENSURE(copied.first() == makeObject<T>(3));
    }

    FixedArray<T, 8> moved(mini::moveCast(empty));
    TEST_ENSURE(moved.empty());
    TEST_ENSURE(empty.empty());

    FixedArray<T, 8> source;
    std::vector<T> vec;
    fill<T, 8>(source, vec, 5);
    FixedArray<T, 8> target(mini::moveCast(source));
    TEST_ENSURE(testArray(target, vec) == 0);

    source.pushBack(makeObject<T>(4));
    TEST_ENSURE(source.size() == 1);
    TEST_ENSURE(source.first() == makeObject<T>(4));
    TEST_ENSURE(testArray(target, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testSingleElementRangeKeepsSource()
{
    FixedArray<T, 8> source;
    std::vector<T> vec;
    fill<T, 8>(source, vec, 4);

    FixedArray<T, 8> indexed;
    indexed.pushBack(makeObject<T>(9));
    indexed.insertRange(0, source.begin() + 1, source.begin() + 2);
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(indexed.size() == 2 && indexed.first() == vec[1]);

    FixedArray<T, 8> iterated;
    iterated.pushBack(makeObject<T>(9));
    iterated.insertRange(iterated.begin(), source.begin() + 3, source.end());
    TEST_ENSURE(testArray(source, vec) == 0);
    TEST_ENSURE(iterated.size() == 2 && iterated.first() == vec[3]);

    return 0;
}

template <typename T>
constexpr int testInsertAliased()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    fill<T, 16>(arr, vec, 2);

    arr.insert(0, arr.first());
    vec.insert(vec.begin(), vec.front());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), arr[1]);
    vec.insert(vec.begin(), vec[1]);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testAssignOverExisting()
{
    FixedArray<T, 16> source;
    std::vector<T> vec;
    fill<T, 16>(source, vec, 8);

    FixedArray<T, 16> arr;
    arr.pushBack(makeObject<T>(90));
    arr.pushBack(makeObject<T>(91));
    arr.pushBack(makeObject<T>(92));

    arr.assign(source.begin(), source.begin() + 6);
    TEST_ENSURE(testArray(arr, std::vector<T>(vec.begin(), vec.begin() + 6)) == 0);

    arr.assign(source.begin() + 5, source.end());
    TEST_ENSURE(testArray(arr, std::vector<T>(vec.begin() + 5, vec.end())) == 0);

    arr.assign(source.begin(), source.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    InitializerList list = {
        makeObject<T>(70),
        makeObject<T>(71),
        makeObject<T>(72),
        makeObject<T>(73),
    };

    arr.assign(list);
    TEST_ENSURE(testArray(arr, std::vector<T>(list)) == 0);

    FixedArray<T, 16> small;
    small.pushBack(makeObject<T>(80));
    small.assign(list);
    TEST_ENSURE(testArray(small, std::vector<T>(list)) == 0);

    small.pushBack(makeObject<T>(81));
    TEST_ENSURE(small.size() == 5);
    TEST_ENSURE(small.last() == makeObject<T>(81));
    TEST_ENSURE(testArray(source, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testCopyAssignSelf()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    fill<T, 16>(arr, vec, 8);

    arr = makeLvalueReference(arr);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T>
constexpr int testMoveAssignSelf()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    fill<T, 16>(arr, vec, 8);

    arr = mini::moveCast(makeLvalueReference(arr));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.pushBack(makeObject<T>(90));
    vec.push_back(makeObject<T>(90));
    TEST_ENSURE(testArray(arr, vec) == 0);

    FixedArray<T, 4> full;
    std::vector<T> fullVec;
    fill<T, 4>(full, fullVec, 4);
    full = mini::moveCast(makeLvalueReference(full));
    TEST_ENSURE(full.full());
    TEST_ENSURE(testArray(full, fullVec) == 0);

    FixedArray<T, 4> empty;
    empty = mini::moveCast(makeLvalueReference(empty));
    TEST_ENSURE(empty.empty());
    empty.pushBack(makeObject<T>(7));
    TEST_ENSURE(empty.size() == 1);

    return 0;
}

constexpr int testPopBackEdge()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(i);
    }

    arr.popBack(3);
    TEST_ENSURE(arr.size() == 5);
    TEST_ENSURE(arr.last() == 4);

    arr.popBack(0);
    TEST_ENSURE(arr.size() == 5);

    arr.popBack(arr.size());
    TEST_ENSURE(arr.empty());

    arr.pushBack(1);
    arr.pushBack(2);
    arr.popBack(100);
    TEST_ENSURE(arr.empty());
    TEST_ENSURE(arr.size() == 0);

    arr.pushBack(3);
    TEST_ENSURE(arr.size() == 1);
    TEST_ENSURE(arr.last() == 3);

    return 0;
}

constexpr int testInsertEdge()
{
    FixedArray<mini::int32, 8> arr;

    arr.insert(0, 1);
    arr.insert(arr.size(), 3);
    arr.insert(1, 2);
    arr.insert(arr.end(), 4);
    arr.insert(arr.begin(), 0);
    TEST_ENSURE(arr.size() == 5);
    for (mini::int32 i = 0; i < 5; ++i) {
        TEST_ENSURE(arr[static_cast<mini::size_t>(i)] == i);
    }

    return 0;
}

constexpr int testRemoveRangeEdge()
{
    FixedArray<mini::int32, 8> arr;
    for (mini::int32 i = 0; i < 5; ++i) {
        arr.pushBack(i);
    }

    arr.removeRange(arr.begin() + 1, arr.begin() + 1);
    TEST_ENSURE(arr.size() == 5);

    arr.removeRange(arr.begin() + 1, arr.begin() + 2);
    TEST_ENSURE(arr.size() == 4);
    TEST_ENSURE(arr[1] == 2);

    arr.removeRange(arr.begin(), arr.end());
    TEST_ENSURE(arr.empty());

    return 0;
}

constexpr int testResizeEdge()
{
    FixedArray<mini::int32, 8> arr;

    arr.resize(8, 5);
    TEST_ENSURE(arr.full());
    TEST_ENSURE(arr.first() == 5 && arr.last() == 5);

    arr.resize(8, 9);
    TEST_ENSURE(arr.last() == 5);

    arr.resize(2);
    TEST_ENSURE(arr.size() == 2);

    arr.resize(4);
    TEST_ENSURE(arr[1] == 5);
    TEST_ENSURE(arr[3] == 0);

    arr.resize(0);
    TEST_ENSURE(arr.empty());

    return 0;
}

int testRemoveLast()
{
    {
        FixedArray<CopyableObject, 4> arr;
        arr.pushBack(1);
        arr.pushBack(2);
        arr.pushBack(3);

        initializeCounter();
        mini::size_t alive = aliveCount;

        arr.remove(arr.size() - 1);
        TEST_ENSURE(dtor == 1);
        TEST_ENSURE(aliveCount == alive - 1);
        TEST_ENSURE(arr.size() == 2);
        TEST_ENSURE(arr.last() == CopyableObject(2));

        arr.remove(arr.end() - 1);
        TEST_ENSURE(dtor == 3);
        TEST_ENSURE(aliveCount == alive - 2);
        TEST_ENSURE(arr.size() == 1);
        TEST_ENSURE(arr.last() == CopyableObject(1));
        TEST_ENSURE(ctor + copyCtor + moveCtor + 2 == dtor);
    }
    TEST_ENSURE(violationCount == 0);

    return 0;
}

int testPopBackLifetime()
{
    {
        FixedArray<CopyableObject, 8> arr;
        for (mini::int32 i = 0; i < 5; ++i) {
            arr.pushBack(i);
        }

        initializeCounter();
        mini::size_t alive = aliveCount;

        arr.popBack(2);
        TEST_ENSURE(dtor == 2);
        TEST_ENSURE(aliveCount == alive - 2);
        TEST_ENSURE(arr.size() == 3);

        arr.popBack(100);
        TEST_ENSURE(dtor == 5);
        TEST_ENSURE(aliveCount == alive - 5);
        TEST_ENSURE(arr.empty());

        arr.pushBack(7);
        TEST_ENSURE(arr.size() == 1);
        TEST_ENSURE(arr.first() == CopyableObject(7));
    }
    TEST_ENSURE(violationCount == 0);

    return 0;
}

int testRemoveLifetime()
{
    {
        FixedArray<CopyableObject, 8> arr;
        for (mini::int32 i = 0; i < 8; ++i) {
            arr.pushBack(i);
        }

        initializeCounter();
        mini::size_t alive = aliveCount;

        arr.removeRange(arr.begin() + 1, arr.begin() + 4);
        TEST_ENSURE(aliveCount == alive - 3);
        TEST_ENSURE(arr.size() == 5);
        TEST_ENSURE(arr[0] == CopyableObject(0));
        TEST_ENSURE(arr[1] == CopyableObject(4));
        TEST_ENSURE(arr.last() == CopyableObject(7));

        arr.removeRange(arr.end() - 2, arr.end());
        TEST_ENSURE(aliveCount == alive - 5);
        TEST_ENSURE(arr.size() == 3);
        TEST_ENSURE(arr.last() == CopyableObject(5));

        arr.remove(1);
        TEST_ENSURE(aliveCount == alive - 6);
        TEST_ENSURE(arr.size() == 2);
        TEST_ENSURE(arr[1] == CopyableObject(5));

        arr.insert(1, 9);
        TEST_ENSURE(aliveCount == alive - 5);
        TEST_ENSURE(arr.size() == 3);
        TEST_ENSURE(arr[1] == CopyableObject(9));
        TEST_ENSURE(arr.last() == CopyableObject(5));

        TEST_ENSURE(copyCtor == 0);
        TEST_ENSURE(copyAssign == 0);
    }
    TEST_ENSURE(violationCount == 0);

    return 0;
}

int testLifetime()
{
    initializeCounter();
    {
        FixedArray<CopyableObject, 16> arr;
        TEST_ENSURE(ctor == 0);

        CopyableObject object(5);
        arr.pushBack(object);
        TEST_ENSURE(copyCtor == 1);

        arr.pushBack(CopyableObject(1));
        arr.pushBack(4);
        TEST_ENSURE(copyCtor == 1);
        TEST_ENSURE(arr[0] == object);

        arr.insert(0, object);
        arr.insert(arr.begin() + 2, 2);
        TEST_ENSURE(arr.size() == 5);
        TEST_ENSURE(arr[2] == CopyableObject(2));

        arr.resize(16, 3);
        TEST_ENSURE(arr.full());
        TEST_ENSURE(arr.last() == CopyableObject(3));

        arr.remove(1);
        arr.removeRange(arr.begin(), arr.begin() + 3);
        arr.popBack(2);
        TEST_ENSURE(arr.size() == 10);

        FixedArray<CopyableObject, 16> copied(arr);
        TEST_ENSURE(copied == arr);

        FixedArray<CopyableObject, 16> moved(mini::moveCast(copied));
        TEST_ENSURE(moved == arr);

        FixedArray<CopyableObject, 16> assigned;
        assigned.assign(arr.begin(), arr.end());
        TEST_ENSURE(assigned == arr);
        assigned.assign(arr.begin() + 2, arr.end());
        TEST_ENSURE(assigned.size() == arr.size() - 2);

        arr.clear();
        TEST_ENSURE(arr.empty());
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPushBack, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAppend, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAppend, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAppend, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAppend, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAppend, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAppend, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsert, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInsert, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsert, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInsert, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInsert, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testInsert, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInsert, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInsertRange, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRange, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInsertRange, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInsertRange, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInsertRange, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeMiddle, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeMiddle, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeMiddle, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeMiddle, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeMiddle, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeMiddle, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAtIterator, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAtIterator, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertRangeAtIterator, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeAtIterator, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeAtIterator, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInsertRangeAtIterator, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopBack, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testPopBack, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testPopBack, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testPopBack, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testPopBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRemove, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testRemove, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRemove, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testRemove, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testRemove, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testRemove, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testRemove, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRemoveRange, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testRemoveRange, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testRemoveRange, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testRemoveRange, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testRemoveRange, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testResize, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testResize, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testResize, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testResize, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testResize, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testResize, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testClear, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testClear, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testClear, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testClear, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testClear, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testClear, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testClear, OveralignedObject);

    TEST_CONSTEXPR(testEmpty);
    TEST_CONSTEXPR(testFull);

    TEST_CONSTEXPR(testValidIndex);
    TEST_CONSTEXPR(testValidIterator);
    TEST_CONSTEXPR(testValidRange);

    TEST_CONSTEXPR(testFirstLast);
    TEST_CONSTEXPR(testSubscript);
    TEST_CONSTEXPR(testData);
    TEST_CONSTEXPR(testIteration);

    TEST_CONSTEXPR(testViewCtor);
    TEST_CONSTEXPR(testConvertToView);
    TEST_CONSTEXPR(testInsertRangeView);
    TEST_CONSTEXPR(testAssignRange);
    TEST_CONSTEXPR(testViewAssign);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testNotEqual);

    TEST_CONSTEXPR_TEMPLATE(testEmptyCopyMoveCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testEmptyCopyMoveCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testEmptyCopyMoveCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testEmptyCopyMoveCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testEmptyCopyMoveCtor, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testEmptyCopyMoveCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testEmptyCopyMoveCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInsertAliased, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInsertAliased, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInsertAliased, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInsertAliased, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignOverExisting, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAssignOverExisting, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignOverExisting, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAssignOverExisting, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAssignOverExisting, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAssignOverExisting, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, OveralignedObject);

    TEST_CONSTEXPR(testPopBackEdge);
    TEST_CONSTEXPR(testInsertEdge);
    TEST_CONSTEXPR(testRemoveRangeEdge);
    TEST_CONSTEXPR(testResizeEdge);

    TEST_FUNCTION(testRemoveLast);
    TEST_FUNCTION(testPopBackLifetime);
    TEST_FUNCTION(testRemoveLifetime);
    TEST_FUNCTION(testLifetime);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
