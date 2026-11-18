#include "test_macro.h"

import mini.test;

using mini::Array;
using mini::ArrayView;
using mini::FixedQueue;
using mini::InitializerList;

template <typename T, size_t CapN>
constexpr mini::int32 testQueue(FixedQueue<T, CapN> const& que, Array<T> const& arr)
{
    TEST_ENSURE(que.size() == arr.size());
    TEST_ENSURE(que.size() == distance(que.begin(), que.end()));

    if (!arr.empty()) {
        TEST_ENSURE(que.first() == arr.first());
        TEST_ENSURE(que.last() == arr.last());
    }

    for (size_t i = 0; i < que.size(); ++i) {
        TEST_ENSURE(que[i] == arr[i]);
        TEST_ENSURE(que.at(i) == arr.at(i));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T, size_t CapN>
constexpr void fill(FixedQueue<T, CapN>& que, Array<T>& arr, size_t count)
{
    mini::int32 quecount = 33;
    mini::int32 arrcount = 33;

    for (size_t i = 0; i < count; ++i) {
        que.pushBack(makeObject<T>(++quecount));
        arr.pushBack(makeObject<T>(++arrcount));
    }
}

template <typename T>
constexpr void fillWrapped(FixedQueue<T, 8>& que, Array<T>& arr)
{
    fill<T, 8>(que, arr, 6);
    que.popFront(4);
    arr.removeRange(arr.begin(), arr.begin() + 4);

    for (mini::int32 i = 0; i < 4; ++i) {
        que.pushBack(makeObject<T>(60 + i));
        arr.pushBack(makeObject<T>(60 + i));
    }
}

template <size_t CapN>
constexpr bool sameAs(FixedQueue<mini::int32, CapN> const& que, InitializerList<mini::int32> expected)
{
    if (que.size() != expected.size()) {
        return false;
    }

    mini::size_t index = 0;
    for (mini::int32 value : expected) {
        if (que[index] != value || que.at(index) != value) {
            return false;
        }
        ++index;
    }

    index = 0;
    for (mini::int32 value : que) {
        if (value != expected.begin()[index]) {
            return false;
        }
        ++index;
    }

    return index == expected.size();
}

template <typename T>
constexpr mini::int32 testCtor()
{
    TEST_ENSURE((FixedQueue<T, 1>{}.empty()));
    TEST_ENSURE((FixedQueue<T, 1>{}.capacity() == 1));
    TEST_ENSURE((FixedQueue<T, 16>().empty()));
    TEST_ENSURE((FixedQueue<T, 16>().capacity() == 16));

    return 0;
}

template <typename T>
constexpr mini::int32 testCopyCtor()
{
    FixedQueue<T, 20> que;
    Array<T> arr;
    fill<T, 20>(que, arr, 20);

    FixedQueue<T, 20> copied(que);
    TEST_ENSURE(copied == que);
    TEST_ENSURE(testQueue(copied, arr) == 0);

    copied.popFront();
    copied.pushBack(makeObject<T>(70));
    arr.remove(0);
    arr.pushBack(makeObject<T>(70));
    TEST_ENSURE(testQueue(copied, arr) == 0);

    FixedQueue<T, 8> wrapped;
    Array<T> wrappedArr;
    fillWrapped<T>(wrapped, wrappedArr);
    FixedQueue<T, 8> wrappedCopied(wrapped);
    TEST_ENSURE(testQueue(wrappedCopied, wrappedArr) == 0);
    TEST_ENSURE(testQueue(wrapped, wrappedArr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testMoveCtor()
{
    FixedQueue<T, 20> que;
    Array<T> arr;
    fill<T, 20>(que, arr, 20);

    FixedQueue<T, 20> moved(mini::moveCast(que));
    TEST_ENSURE(que.empty());
    TEST_ENSURE(testQueue(moved, arr) == 0);

    FixedQueue<T, 8> wrapped;
    Array<T> wrappedArr;
    fillWrapped<T>(wrapped, wrappedArr);
    FixedQueue<T, 8> wrappedMoved(mini::moveCast(wrapped));
    TEST_ENSURE(testQueue(wrappedMoved, wrappedArr) == 0);
    TEST_ENSURE(wrapped.empty());

    return 0;
}

template <typename T>
constexpr mini::int32 testMoveIteratorCtor()
{
    FixedQueue<T, 8> source;
    Array<T> arr;
    fillWrapped<T>(source, arr);

    FixedQueue<T, 8> que(mini::MoveIterator(source.begin()), mini::MoveIterator(source.end()));
    TEST_ENSURE(testQueue(que, arr) == 0);
    TEST_ENSURE(source.size() == que.size());

    FixedQueue<T, 8> appended;
    appended.pushBack(makeObject<T>(5));
    que.popFront(4);
    appended.append(mini::MoveIterator(que.begin()), mini::MoveIterator(que.end()));
    TEST_ENSURE(appended.size() == 3);
    TEST_ENSURE(appended.first() == makeObject<T>(5));
    TEST_ENSURE(appended[1] == arr[4]);
    TEST_ENSURE(appended.last() == arr.last());

    FixedQueue<T, 8> assigned;
    assigned.pushBack(makeObject<T>(7));
    assigned.assign(mini::MoveIterator(appended.begin()), mini::MoveIterator(appended.end()));
    TEST_ENSURE(assigned.size() == 3);
    TEST_ENSURE(assigned.first() == makeObject<T>(5));
    TEST_ENSURE(assigned[1] == arr[4]);
    TEST_ENSURE(assigned.last() == arr.last());

    return 0;
}

template <typename T>
constexpr mini::int32 testRangeCtor()
{
    FixedQueue<T, 20> que;
    Array<T> arr;
    fill<T, 20>(que, arr, 20);

    TEST_ENSURE((FixedQueue<T, 20>(que.begin(), que.end()) == que));
    TEST_ENSURE((FixedQueue<T, 20>(que.begin(), que.begin()).empty()));

    return 0;
}

template <typename T>
constexpr mini::int32 testInitializerListCtor()
{
    mini::int32 count = 0;
    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    TEST_ENSURE(testQueue(FixedQueue<T, 4>(list), Array<T>(list)) == 0);

    FixedQueue<T, 4> que(list);
    Array<T> arr(list);
    TEST_ENSURE(que.full());
    que.popFront();
    que.pushBack(makeObject<T>(++count));
    arr.remove(0);
    arr.pushBack(makeObject<T>(count));
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testPushBack()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    mini::int32 quecount = 33;
    mini::int32 arrcount = 33;

    que.pushBack(makeObject<T>(++quecount));
    arr.pushBack(makeObject<T>(++arrcount));
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.pushBack(makeArgument<T>(++quecount));
    arr.pushBack(makeArgument<T>(++arrcount));
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testAppend()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fill<T, 8>(que, arr, 2);
    FixedQueue<T, 8> source(que.begin(), que.end());
    mini::int32 count = 50;

    que.append(source.begin(), source.end());
    arr.append(source.begin(), source.end());
    TEST_ENSURE(testQueue(que, arr) == 0);

    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    que.append(list);
    arr.append(list);
    TEST_ENSURE(testQueue(que, arr) == 0);
    TEST_ENSURE(que.full());

    que.popFront();
    que.pushBack(makeObject<T>(++count));
    arr.remove(0);
    arr.pushBack(makeObject<T>(count));
    TEST_ENSURE(testQueue(que, arr) == 0);

    FixedQueue<T, 4> exact;
    Array<T> exactArr;
    exact.append(list);
    exactArr.append(list);
    exact.popFront();
    exact.pushBack(makeObject<T>(++count));
    exactArr.remove(0);
    exactArr.pushBack(makeObject<T>(count));
    TEST_ENSURE(testQueue(exact, exactArr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testAssign()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fill<T, 8>(que, arr, 2);
    FixedQueue<T, 8> source(que.begin(), que.end());
    mini::int32 count = 50;

    que.clear();
    que.assign(source.begin(), source.end());
    TEST_ENSURE(que == source);

    InitializerList list = {
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
        makeObject<T>(++count),
    };

    que.assign(list);
    TEST_ENSURE(testQueue(que, Array<T>(list)) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testPopFront()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fill<T, 8>(que, arr, 8);
    TEST_ENSURE(que.full());

    que.popFront();
    arr.remove(0);
    TEST_ENSURE(testQueue(que, arr) == 0);
    TEST_ENSURE(!que.full());

    que.popFront();
    arr.remove(0);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.popFront(2);
    arr.removeRange(arr.begin(), arr.begin() + 2);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.clear();
    arr.clear();
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.pushBack(makeObject<T>(70));
    arr.pushBack(makeObject<T>(70));
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testPopFirst()
{
    FixedQueue<T, 4> que;
    Array<T> arr;
    fill<T, 4>(que, arr, 4);

    T popped = que.popFirst();
    TEST_ENSURE(popped == arr.first());

    arr.remove(0);
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testClear()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fill<T, 8>(que, arr, 6);

    que.clear();
    arr.clear();
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.clear();
    TEST_ENSURE(que.empty());

    return 0;
}

template <typename T>
constexpr mini::int32 testCopyAssign()
{
    FixedQueue<T, 8> source;
    Array<T> arr;
    fillWrapped<T>(source, arr);

    FixedQueue<T, 8> copied;
    copied.pushBack(makeObject<T>(9));
    copied = source;
    TEST_ENSURE(testQueue(copied, arr) == 0);
    TEST_ENSURE(testQueue(source, arr) == 0);

    FixedQueue<T, 8> full;
    Array<T> unused;
    fill<T, 8>(full, unused, 8);
    full = source;
    TEST_ENSURE(testQueue(full, arr) == 0);

    FixedQueue<T, 8> empty;
    full = empty;
    TEST_ENSURE(full.empty());

    FixedQueue<T, 8> wrappedDest;
    Array<T> wrappedUnused;
    fill<T, 8>(wrappedDest, wrappedUnused, 7);
    wrappedDest.popFront(6);
    wrappedDest = source;
    TEST_ENSURE(testQueue(wrappedDest, arr) == 0);

    wrappedDest.pushBack(makeObject<T>(80));
    wrappedDest.pushBack(makeObject<T>(81));
    arr.pushBack(makeObject<T>(80));
    arr.pushBack(makeObject<T>(81));
    TEST_ENSURE(wrappedDest.full());
    TEST_ENSURE(testQueue(wrappedDest, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testMoveAssign()
{
    FixedQueue<T, 8> source;
    Array<T> arr;
    fillWrapped<T>(source, arr);

    FixedQueue<T, 8> moved;
    moved = mini::moveCast(source);
    TEST_ENSURE(testQueue(moved, arr) == 0);
    TEST_ENSURE(source.empty());

    FixedQueue<T, 8> nonEmpty;
    nonEmpty.pushBack(makeObject<T>(42));
    nonEmpty = mini::moveCast(moved);
    TEST_ENSURE(testQueue(nonEmpty, arr) == 0);
    TEST_ENSURE(moved.empty());

    FixedQueue<T, 8> full;
    Array<T> unused;
    fill<T, 8>(full, unused, 8);
    full = mini::moveCast(nonEmpty);
    TEST_ENSURE(testQueue(full, arr) == 0);
    TEST_ENSURE(nonEmpty.empty());

    FixedQueue<T, 8> emptySource;
    full = mini::moveCast(emptySource);
    TEST_ENSURE(full.empty());
    TEST_ENSURE(emptySource.empty());

    full.pushBack(makeObject<T>(43));
    TEST_ENSURE(full.size() == 1);
    TEST_ENSURE(full.first() == makeObject<T>(43));

    return 0;
}

constexpr mini::int32 testEmpty()
{
    FixedQueue<mini::int32, 4> que;
    TEST_ENSURE(que.empty());
    TEST_ENSURE(!que.full());
    TEST_ENSURE(que.size() == 0);
    TEST_ENSURE(que.capacity() == 4);
    TEST_ENSURE(que.begin() == que.end());

    que.popFront();
    que.popFront(3);
    que.clear();
    TEST_ENSURE(que.empty());

    que.pushBack(1);
    TEST_ENSURE(!que.empty());
    TEST_ENSURE(sameAs(que, {1}));

    return 0;
}

constexpr mini::int32 testFull()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(!que.full());
    TEST_ENSURE(que.size() == 3);

    que.pushBack(4);
    TEST_ENSURE(que.full());
    TEST_ENSURE(que.size() == 4);
    TEST_ENSURE(que.capacity() == 4);

    que.popFront();
    TEST_ENSURE(!que.full());

    return 0;
}

constexpr mini::int32 testValidIndex()
{
    FixedQueue<mini::int32, 4> que;
    TEST_ENSURE(!que.validIndex(0));

    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(que.validIndex(0));
    TEST_ENSURE(que.validIndex(2));
    TEST_ENSURE(!que.validIndex(3));

    que.popFront(2);
    que.pushBack(4);
    que.pushBack(5);
    TEST_ENSURE(sameAs(que, {3, 4, 5}));
    TEST_ENSURE(que.validIndex(2));
    TEST_ENSURE(!que.validIndex(3));

    return 0;
}

constexpr mini::int32 testValidIterator()
{
    FixedQueue<mini::int32, 4> que;
    TEST_ENSURE(!que.validIterator(que.begin()));

    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(que.validIterator(que.begin()));
    TEST_ENSURE(que.validIterator(que.end() - 1));
    TEST_ENSURE(!que.validIterator(que.end()));

    que.popFront(2);
    que.pushBack(4);
    que.pushBack(5);
    TEST_ENSURE(que.validIterator(que.begin()));
    TEST_ENSURE(que.validIterator(que.end() - 1));
    TEST_ENSURE(!que.validIterator(que.end()));

    return 0;
}

constexpr mini::int32 testValidRange()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(que.validRange(que.begin(), que.end()));
    TEST_ENSURE(que.validRange(que.begin() + 1, que.begin() + 2));
    TEST_ENSURE(!que.validRange(que.end(), que.end()));

    que.popFront(2);
    que.pushBack(4);
    que.pushBack(5);
    TEST_ENSURE(que.validRange(que.begin(), que.end()));

    return 0;
}

constexpr mini::int32 testFirstLast()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(que.first() == 1);
    TEST_ENSURE(que.last() == 3);

    que.first() = 10;
    que.last() = 30;
    TEST_ENSURE(sameAs(que, {10, 2, 30}));

    que.popFront();
    que.pushBack(4);
    que.pushBack(5);
    TEST_ENSURE(que.first() == 2);
    TEST_ENSURE(que.last() == 5);

    FixedQueue<mini::int32, 4> const& cque = que;
    TEST_ENSURE(cque.first() == 2);
    TEST_ENSURE(cque.last() == 5);

    return 0;
}

constexpr mini::int32 testSubscript()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.popFront();
    que.pushBack(4);
    que.pushBack(5);

    TEST_ENSURE(que[0] == 2);
    TEST_ENSURE(que.at(3) == 5);

    que[0] = 20;
    que.at(1) = 30;
    TEST_ENSURE(que.at(0) == 20);
    TEST_ENSURE(que[1] == 30);

    FixedQueue<mini::int32, 4> const& cque = que;
    TEST_ENSURE(cque[1] == 30);
    TEST_ENSURE(cque.at(2) == 4);

    return 0;
}

constexpr mini::int32 testData()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    TEST_ENSURE(que.data()[0] == 1);

    que.popFront();
    que.pushBack(4);
    que.pushBack(5);
    TEST_ENSURE(que.data()[0] == 5);

    FixedQueue<mini::int32, 4> const& cque = que;
    TEST_ENSURE(cque.data() == que.data());

    return 0;
}

constexpr mini::int32 testIteration()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.popFront();
    que.pushBack(4);
    que.pushBack(5);

    FixedQueue<mini::int32, 4> const& cque = que;
    TEST_ENSURE(*cque.begin() == 2);
    TEST_ENSURE(*(cque.end() - 1) == 5);
    TEST_ENSURE(static_cast<mini::size_t>(cque.end() - cque.begin()) == cque.size());

    mini::int32 sum = 0;
    for (mini::int32 const& value : cque) {
        sum += value;
    }
    TEST_ENSURE(sum == 14);

    for (mini::int32& value : que) {
        value *= 2;
    }
    TEST_ENSURE(sameAs(que, {4, 6, 8, 10}));

    return 0;
}

constexpr mini::int32 testAppendView()
{
    InitializerList list = {10, 11, 12};
    ArrayView<mini::int32> view = list;

    FixedQueue<mini::int32, 4> que;
    que.pushBack(3);
    que.append(list);
    TEST_ENSURE(sameAs(que, {3, 10, 11, 12}));
    TEST_ENSURE(que.full());

    que.popFront(3);
    que.append(view.subFront(2));
    TEST_ENSURE(sameAs(que, {12, 10, 11}));

    que.append(ArrayView<mini::int32>());
    TEST_ENSURE(sameAs(que, {12, 10, 11}));

    return 0;
}

constexpr mini::int32 testAppendIterator()
{
    InitializerList list = {10, 11, 12};
    FixedQueue<mini::int32, 4> source(list);

    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.popFront(2);
    que.append(source.begin(), source.end());
    TEST_ENSURE(sameAs(que, {3, 10, 11, 12}));

    que.popFront(3);
    que.append(source.begin(), source.begin());
    TEST_ENSURE(sameAs(que, {12}));

    return 0;
}

constexpr mini::int32 testAssignRange()
{
    InitializerList list = {10, 11, 12};
    FixedQueue<mini::int32, 4> source(list);

    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.popFront(2);
    que.pushBack(4);
    que.pushBack(5);

    que.assign(list);
    TEST_ENSURE(sameAs(que, {10, 11, 12}));

    que.assign(ArrayView<mini::int32>());
    TEST_ENSURE(que.empty());

    que.assign(source.begin(), source.end());
    TEST_ENSURE(sameAs(que, {10, 11, 12}));

    que.assign(source.begin(), source.begin() + 1);
    TEST_ENSURE(sameAs(que, {10}));

    que.assign(source.begin(), source.begin());
    TEST_ENSURE(que.empty());

    return 0;
}

constexpr mini::int32 testViewCtor()
{
    InitializerList list = {10, 11, 12};
    ArrayView<mini::int32> view = list;

    FixedQueue<mini::int32, 4> fromView(view);
    TEST_ENSURE(sameAs(fromView, {10, 11, 12}));

    FixedQueue<mini::int32, 3> exact(view);
    TEST_ENSURE(exact.full());
    exact.popFront();
    exact.pushBack(13);
    TEST_ENSURE(sameAs(exact, {11, 12, 13}));
    TEST_ENSURE(exact.last() == 13);

    return 0;
}

constexpr mini::int32 testViewAssign()
{
    InitializerList list = {10, 11, 12};

    FixedQueue<mini::int32, 4> que;
    que = ArrayView<mini::int32>(list);
    TEST_ENSURE(sameAs(que, {10, 11, 12}));

    que = ArrayView<mini::int32>(list).subFront(1);
    TEST_ENSURE(sameAs(que, {10}));

    return 0;
}

constexpr mini::int32 testEqual()
{
    InitializerList list = {10, 11, 12};
    FixedQueue<mini::int32, 4> lhs(list);
    FixedQueue<mini::int32, 8> rhs(list);
    FixedQueue<mini::int32, 4> empty1;
    FixedQueue<mini::int32, 8> empty2;

    TEST_ENSURE(lhs == lhs);
    TEST_ENSURE(lhs == rhs);
    TEST_ENSURE(rhs == lhs);
    TEST_ENSURE(empty1 == empty2);

    FixedQueue<mini::int64, 8> wide;
    wide.pushBack(10);
    wide.pushBack(11);
    wide.pushBack(12);
    TEST_ENSURE(lhs == wide);
    TEST_ENSURE(wide == lhs);

    wide.popFront();
    wide.pushBack(13);
    TEST_ENSURE(lhs != wide);

    rhs.pushBack(13);
    lhs.pushBack(13);
    TEST_ENSURE(lhs == rhs);

    lhs.popFront();
    lhs.pushBack(14);
    rhs.popFront();
    rhs.pushBack(14);
    TEST_ENSURE(lhs == rhs);

    return 0;
}

constexpr mini::int32 testNotEqual()
{
    InitializerList list = {10, 11, 12};
    FixedQueue<mini::int32, 4> lhs(list);
    FixedQueue<mini::int32, 8> rhs(list);
    FixedQueue<mini::int32, 4> empty;

    TEST_ENSURE(lhs != empty);

    rhs.pushBack(13);
    TEST_ENSURE(lhs != rhs);

    lhs.pushBack(13);
    lhs.popFront();
    lhs.pushBack(14);
    rhs.popFront();
    TEST_ENSURE(lhs != rhs);

    FixedQueue<mini::int32, 4> same(list);
    FixedQueue<mini::int32, 8> different(list);
    different[1] = 20;
    TEST_ENSURE(same.size() == different.size());
    TEST_ENSURE(same != different);

    same.popFront();
    same.pushBack(13);
    same.pushBack(14);
    different.popFront();
    different.pushBack(13);
    different.pushBack(14);
    TEST_ENSURE(same.size() == different.size());
    TEST_ENSURE(same != different);

    return 0;
}

template <typename T>
constexpr mini::int32 testSingleElementRangeKeepsSource()
{
    FixedQueue<T, 8> source;
    Array<T> arr;
    fill<T, 8>(source, arr, 4);

    FixedQueue<T, 8> appended;
    appended.pushBack(makeObject<T>(9));
    appended.append(source.begin() + 1, source.begin() + 2);
    TEST_ENSURE(testQueue(source, arr) == 0);
    TEST_ENSURE(appended.size() == 2 && appended.last() == arr[1]);

    return 0;
}

template <typename T>
constexpr mini::int32 testAssignWrapped()
{
    FixedQueue<T, 8> source;
    Array<T> arr;
    fill<T, 8>(source, arr, 3);

    FixedQueue<T, 8> que;
    Array<T> unused;
    fillWrapped<T>(que, unused);
    que.assign(source.begin(), source.begin() + 2);
    arr.removeRange(arr.begin() + 2, arr.end());
    TEST_ENSURE(testQueue(que, arr) == 0);

    for (mini::int32 i = 0; i < 6; ++i) {
        que.pushBack(makeObject<T>(70 + i));
        arr.pushBack(makeObject<T>(70 + i));
    }
    TEST_ENSURE(que.full());
    TEST_ENSURE(testQueue(que, arr) == 0);

    InitializerList list = {
        makeObject<T>(80),
        makeObject<T>(81),
        makeObject<T>(82),
    };

    que.assign(list);
    TEST_ENSURE(testQueue(que, Array<T>(list)) == 0);

    que.pushBack(makeObject<T>(83));
    TEST_ENSURE(que.size() == 4);
    TEST_ENSURE(que.last() == makeObject<T>(83));

    return 0;
}

template <typename T>
constexpr mini::int32 testCopyAssignSelf()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fillWrapped<T>(que, arr);

    que = makeLvalueReference(que);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.assign(que.begin(), que.end());
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

template <typename T>
constexpr mini::int32 testMoveAssignSelf()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    fillWrapped<T>(que, arr);

    que = mini::moveCast(makeLvalueReference(que));
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.popFront();
    que.pushBack(makeObject<T>(90));
    arr.remove(0);
    arr.pushBack(makeObject<T>(90));
    TEST_ENSURE(testQueue(que, arr) == 0);

    FixedQueue<T, 4> full;
    Array<T> fullArr;
    fill<T, 4>(full, fullArr, 4);
    full = mini::moveCast(makeLvalueReference(full));
    TEST_ENSURE(full.full());
    TEST_ENSURE(testQueue(full, fullArr) == 0);

    FixedQueue<T, 4> empty;
    empty = mini::moveCast(makeLvalueReference(empty));
    TEST_ENSURE(empty.empty());
    empty.pushBack(makeObject<T>(7));
    TEST_ENSURE(empty.size() == 1);

    return 0;
}

constexpr mini::int32 testWrap()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);
    que.pushBack(5);

    que.popFront();
    que.pushBack(6);
    TEST_ENSURE(sameAs(que, {3, 4, 5, 6}));

    for (mini::int32 i = 7; i < 40; ++i) {
        que.popFront();
        que.pushBack(i);
        TEST_ENSURE(que.first() == i - 3);
        TEST_ENSURE(que.last() == i);
        TEST_ENSURE(que.full());
    }
    TEST_ENSURE(sameAs(que, {36, 37, 38, 39}));

    return 0;
}

constexpr mini::int32 testPopFrontEdge()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);

    que.popFront(2);
    TEST_ENSURE(sameAs(que, {3, 4}));

    que.popFront(0);
    TEST_ENSURE(sameAs(que, {3, 4}));

    que.popFront(100);
    TEST_ENSURE(que.empty());

    que.pushBack(1);
    que.pushBack(2);
    que.popFront();
    que.popFront();
    que.pushBack(3);
    TEST_ENSURE(sameAs(que, {3}));
    TEST_ENSURE(que.first() == que.last());

    return 0;
}

constexpr mini::int32 testPopFrontCountBoundary()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);

    que.popFront(2);
    que.pushBack(5);
    que.pushBack(6);
    que.popFront(2);
    TEST_ENSURE(sameAs(que, {5, 6}));
    TEST_ENSURE(que.first() == 5);

    que.popFront();
    TEST_ENSURE(sameAs(que, {6}));
    TEST_ENSURE(que.first() == 6);

    que.pushBack(7);
    TEST_ENSURE(sameAs(que, {6, 7}));

    return 0;
}

constexpr mini::int32 testMoveFull()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);

    FixedQueue<mini::int32, 4> moved(mini::moveCast(que));
    TEST_ENSURE(moved.full());
    moved.popFront();
    moved.pushBack(5);
    TEST_ENSURE(sameAs(moved, {2, 3, 4, 5}));
    TEST_ENSURE(moved.last() == 5);

    FixedQueue<mini::int32, 4> assigned;
    assigned.pushBack(9);
    assigned = mini::moveCast(moved);
    TEST_ENSURE(assigned.full());
    assigned.popFront();
    assigned.pushBack(6);
    TEST_ENSURE(sameAs(assigned, {3, 4, 5, 6}));
    TEST_ENSURE(assigned.last() == 6);

    return 0;
}

constexpr mini::int32 testPopFirstWrap()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);

    TEST_ENSURE(que.popFirst() == 1);
    TEST_ENSURE(sameAs(que, {2, 3}));

    TEST_ENSURE(que.popFirst() == 2);
    TEST_ENSURE(sameAs(que, {3}));

    for (mini::int32 i = 0; i < 10; ++i) {
        que.pushBack(que.popFirst() + 1);
    }
    TEST_ENSURE(sameAs(que, {13}));

    return 0;
}

constexpr mini::int32 testPopFrontAcrossWrap()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);

    que.popFront(3);
    TEST_ENSURE(sameAs(que, {4}));

    que.pushBack(5);
    que.pushBack(6);
    TEST_ENSURE(sameAs(que, {4, 5, 6}));

    que.popFront(2);
    TEST_ENSURE(sameAs(que, {6}));
    TEST_ENSURE(que.first() == 6);
    TEST_ENSURE(que.last() == 6);

    que.pushBack(7);
    que.pushBack(8);
    que.pushBack(9);
    TEST_ENSURE(que.full());
    TEST_ENSURE(sameAs(que, {6, 7, 8, 9}));

    que.popFront(4);
    TEST_ENSURE(que.empty());

    que.pushBack(10);
    TEST_ENSURE(sameAs(que, {10}));

    return 0;
}

constexpr mini::int32 testFullWrapped()
{
    FixedQueue<mini::int32, 4> que;
    que.pushBack(1);
    que.pushBack(2);
    que.pushBack(3);
    que.pushBack(4);
    que.popFront();
    que.pushBack(5);
    TEST_ENSURE(que.full());
    TEST_ENSURE(sameAs(que, {2, 3, 4, 5}));
    TEST_ENSURE(static_cast<mini::size_t>(que.end() - que.begin()) == 4);

    TEST_ENSURE(que.validIterator(que.begin()));
    TEST_ENSURE(que.validIterator(que.end() - 1));
    TEST_ENSURE(!que.validIterator(que.end()));
    TEST_ENSURE(que.validRange(que.begin(), que.end()));
    TEST_ENSURE(que.validRange(que.begin() + 2, que.end()));

    FixedQueue<mini::int32, 4> copied(que);
    TEST_ENSURE(sameAs(copied, {2, 3, 4, 5}));

    FixedQueue<mini::int32, 4> moved(mini::moveCast(copied));
    TEST_ENSURE(sameAs(moved, {2, 3, 4, 5}));
    TEST_ENSURE(copied.empty());

    que.clear();
    TEST_ENSURE(que.empty());
    que.pushBack(6);
    TEST_ENSURE(sameAs(que, {6}));

    return 0;
}

mini::int32 testWrappedLifetime()
{
    {
        FixedQueue<CopyableObject, 4> que;
        que.pushBack(1);
        que.pushBack(2);
        que.pushBack(3);
        que.pushBack(4);
        que.popFront(3);
        que.pushBack(5);
        que.pushBack(6);

        initializeCounter();
        mini::size_t alive = aliveCount;

        que.popFront(2);
        TEST_ENSURE(dtor == 2);
        TEST_ENSURE(aliveCount == alive - 2);
        TEST_ENSURE(que.size() == 1);

        que.pushBack(7);
        que.pushBack(8);
        que.pushBack(9);
        TEST_ENSURE(que.full());

        initializeCounter();
        alive = aliveCount;

        que.clear();
        TEST_ENSURE(dtor == 4);
        TEST_ENSURE(aliveCount == alive - 4);
        TEST_ENSURE(que.empty());

        que.pushBack(10);
        TEST_ENSURE(que.size() == 1);
        TEST_ENSURE(que.first() == CopyableObject(10));
    }
    TEST_ENSURE(violationCount == 0);

    return 0;
}

mini::int32 testLifetime()
{
    initializeCounter();
    {
        FixedQueue<CopyableObject, 4> que;
        TEST_ENSURE(ctor == 0);

        CopyableObject object(1);
        que.pushBack(object);
        TEST_ENSURE(copyCtor == 1);

        que.pushBack(CopyableObject(2));
        que.pushBack(3);
        TEST_ENSURE(copyCtor == 1);

        for (mini::int32 i = 0; i < 10; ++i) {
            CopyableObject popped = que.popFirst();
            que.pushBack(mini::moveCast(popped));
        }
        TEST_ENSURE(copyCtor == 1);
        TEST_ENSURE(que.size() == 3);

        que.pushBack(4);
        TEST_ENSURE(que.full());

        FixedQueue<CopyableObject, 4> copied(que);
        TEST_ENSURE(copied == que);

        FixedQueue<CopyableObject, 4> moved(mini::moveCast(copied));
        TEST_ENSURE(moved == que);

        moved.popFront(2);
        moved.clear();
        TEST_ENSURE(moved.empty());

        que.popFront();
        que.clear();
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

mini::int32 main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCtor, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveIteratorCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveIteratorCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testRangeCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testRangeCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testInitializerListCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPushBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testPushBack, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testPushBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAppend, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAppend, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAppend, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAppend, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAppend, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAppend, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopFront, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testPopFront, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testPopFront, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testPopFront, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testPopFront, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopFirst, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopFirst, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testPopFirst, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testPopFirst, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testPopFirst, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testPopFirst, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testPopFirst, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testClear, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testClear, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testClear, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testClear, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testClear, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testClear, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testClear, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssign, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssign, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssign, OveralignedObject);

    TEST_CONSTEXPR(testEmpty);
    TEST_CONSTEXPR(testFull);

    TEST_CONSTEXPR(testValidIndex);
    TEST_CONSTEXPR(testValidIterator);
    TEST_CONSTEXPR(testValidRange);

    TEST_CONSTEXPR(testFirstLast);
    TEST_CONSTEXPR(testSubscript);
    TEST_CONSTEXPR(testData);
    TEST_CONSTEXPR(testIteration);

    TEST_CONSTEXPR(testAppendView);
    TEST_CONSTEXPR(testAppendIterator);
    TEST_CONSTEXPR(testAssignRange);
    TEST_CONSTEXPR(testViewCtor);
    TEST_CONSTEXPR(testViewAssign);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testNotEqual);

    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testSingleElementRangeKeepsSource, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testSingleElementRangeKeepsSource, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignWrapped, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignWrapped, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAssignWrapped, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testAssignWrapped, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testAssignWrapped, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testAssignWrapped, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyAssignSelf, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testCopyAssignSelf, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveAssignSelf, TrivialObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, NoDefaultObject);
    TEST_FUNCTION_TEMPLATE(testMoveAssignSelf, OveralignedObject);

    TEST_CONSTEXPR(testWrap);
    TEST_CONSTEXPR(testPopFrontEdge);
    TEST_CONSTEXPR(testPopFrontCountBoundary);
    TEST_CONSTEXPR(testMoveFull);
    TEST_CONSTEXPR(testPopFirstWrap);
    TEST_CONSTEXPR(testPopFrontAcrossWrap);
    TEST_CONSTEXPR(testFullWrapped);

    TEST_FUNCTION(testWrappedLifetime);
    TEST_FUNCTION(testLifetime);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
