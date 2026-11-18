#include <span>

#include "test_macro.h"

import mini.test;

using mini::ArrayView;
using mini::InitializerList;

template <typename T>
[[no_inline]] constexpr mini::int32 testArrayView(ArrayView<T> arr, std::span<T> span)
{
    TEST_ENSURE(arr.size() == span.size());
    TEST_ENSURE(arr.empty() == span.empty());

    if (!arr.empty()) {
        TEST_ENSURE(arr.first() == span.front());
        TEST_ENSURE(arr.last() == span.back());
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        TEST_ENSURE(arr[i] == span[i]);
        TEST_ENSURE(arr.at(i) == span[i]);
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCtor()
{
    ArrayView<T> view;
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == nullptr);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCArrayCtor()
{
    T array[4] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view(array);
    TEST_ENSURE(view.size() == 4);
    TEST_ENSURE(view.data() == array);
    TEST_ENSURE(testArrayView(view, std::span<T>(array)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testInitializerListCtor()
{
    InitializerList list = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view(list);
    TEST_ENSURE(view.size() == list.size());
    TEST_ENSURE(view.data() == list.begin());

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPointerCtor()
{
    InitializerList list = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view(list.begin(), list.size());
    TEST_ENSURE(view.size() == list.size());
    TEST_ENSURE(view.data() == list.begin());

    ArrayView<T> partial(list.begin() + 1, 2);
    TEST_ENSURE(partial.size() == 2);
    TEST_ENSURE(partial.data() == list.begin() + 1);
    TEST_ENSURE(partial.first() == *(list.begin() + 1));
    TEST_ENSURE(partial.last() == *(list.begin() + 2));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testSubFront()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view.subFront(3), span.subspan(0, 3)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testSubBack()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view.subBack(3), span.subspan(span.size() - 3, 3)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testSubView()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view.subView(1, 2), span.subspan(1, 2)) == 0);
    TEST_ENSURE(testArrayView(view.subView(view.begin() + 1, view.begin() + 3), span.subspan(1, 2)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopFront()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(5),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};

    ArrayView<T> popped = view;
    popped.popFront();
    TEST_ENSURE(testArrayView(popped, span.subspan(1, 4)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopFrontCount()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(5),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};

    ArrayView<T> popped = view;
    popped.popFront(3);
    TEST_ENSURE(testArrayView(popped, span.subspan(3, 2)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopBack()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(5),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};

    ArrayView<T> popped = view;
    popped.popBack();
    TEST_ENSURE(testArrayView(popped, span.subspan(0, 4)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testPopBackCount()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(5),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};

    ArrayView<T> popped = view;
    popped.popBack(3);
    TEST_ENSURE(testArrayView(popped, span.subspan(0, 2)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAssignList()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };

    InitializerList list = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };

    ArrayView<T> view;
    view = list;
    TEST_ENSURE(view.data() == list.begin());
    TEST_ENSURE(testArrayView(view, std::span<T>(array)) == 0);

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testAssignRawArray()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };

    ArrayView<T> view;
    view = array;
    TEST_ENSURE(view.data() == array);
    TEST_ENSURE(testArrayView(view, std::span<T>(array)) == 0);

    return 0;
}

[[no_inline]] constexpr mini::int32 testBeginEnd()
{
    ArrayView<mini::int32> empty;
    TEST_ENSURE(empty.begin() == empty.end());

    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    TEST_ENSURE(view.begin().address() == array);
    TEST_ENSURE(view.end().address() == array + 8);
    TEST_ENSURE(static_cast<mini::size_t>(view.end() - view.begin()) == view.size());

    return 0;
}

[[no_inline]] constexpr mini::int32 testRangeFor()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    mini::int32 sum = 0;
    for (mini::int32 value : view) {
        sum += value;
    }
    TEST_ENSURE(sum == 28);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidIndex()
{
    ArrayView<mini::int32> empty;
    TEST_ENSURE(!empty.validIndex(0));

    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    TEST_ENSURE(view.validIndex(0));
    TEST_ENSURE(view.validIndex(7));
    TEST_ENSURE(!view.validIndex(8));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidIterator()
{
    ArrayView<mini::int32> empty;
    TEST_ENSURE(!empty.validIterator(empty.begin()));

    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    TEST_ENSURE(view.validIterator(view.begin()));
    TEST_ENSURE(view.validIterator(view.end() - 1));
    TEST_ENSURE(!view.validIterator(view.end()));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidRange()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view.validRange(view.begin(), view.end()));
    TEST_ENSURE(view.validRange(view.begin() + 2, view.begin() + 4));
    TEST_ENSURE(view.validRange(view.begin() + 2, view.begin() + 2));

    return 0;
}

[[no_inline]] constexpr mini::int32 testEqual()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    mini::int32 same[] = {0, 1, 2, 3, 4, 5, 6, 7};
    mini::int32 different[] = {0, 1, 2, 3, 4, 5, 6, 8};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view == ArrayView<mini::int32>(same));
    TEST_ENSURE(view.subFront(7) == ArrayView<mini::int32>(different).subFront(7));
    TEST_ENSURE(ArrayView<mini::int32>() == ArrayView<mini::int32>());
    TEST_ENSURE(view == view);

    return 0;
}

[[no_inline]] constexpr mini::int32 testNotEqual()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    mini::int32 different[] = {0, 1, 2, 3, 4, 5, 6, 8};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view != ArrayView<mini::int32>(different));
    TEST_ENSURE(view != view.subFront(7));
    TEST_ENSURE(view != ArrayView<mini::int32>());

    return 0;
}

[[no_inline]] constexpr mini::int32 testConvertToConst()
{
    mini::int32 values[] = {1, 2, 3};
    ArrayView<mini::int32> view = values;

    ArrayView<mini::int32 const> constView = view;
    TEST_ENSURE(constView.size() == 3);
    TEST_ENSURE(constView.data() == values);
    TEST_ENSURE(constView.last() == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConvertPointerConst()
{
    mini::int32 values[] = {1, 2, 3};
    mini::int32* pointers[] = {values, values + 1, values + 2};
    ArrayView<mini::int32*> view = pointers;

    ArrayView<mini::int32 const*> constView = view;
    TEST_ENSURE(constView.size() == 3);
    TEST_ENSURE(static_cast<void const*>(constView.data()) == static_cast<void const*>(pointers));
    TEST_ENSURE(constView[0] == values);
    TEST_ENSURE(*constView.last() == 3);

    mini::int32** levels[] = {pointers, pointers + 1};
    ArrayView<mini::int32 const* const*> levelView = ArrayView<mini::int32**>(levels);
    TEST_ENSURE(levelView.size() == 2);
    TEST_ENSURE(**levelView[1] == 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubFrontBoundary()
{
    ArrayView<mini::int32> empty;
    TEST_ENSURE(empty.subFront(3).empty());

    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    TEST_ENSURE(view.subFront(0).empty());
    TEST_ENSURE(view.subFront(100) == view);
    TEST_ENSURE(view.subFront(3).data() == array);

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubBackBoundary()
{
    ArrayView<mini::int32> empty;
    TEST_ENSURE(empty.subBack(3).empty());

    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    TEST_ENSURE(view.subBack(0).empty());
    TEST_ENSURE(view.subBack(100) == view);
    TEST_ENSURE(view.subBack(3).data() == array + 5);

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubViewIndexBoundary()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view.subView(6, 100).size() == 2);
    TEST_ENSURE(view.subView(6, 100).first() == 6);
    TEST_ENSURE(view.subView(7, 1).first() == 7);
    TEST_ENSURE(view.subView(2, 0).empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubViewIteratorPosition()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    ArrayView<mini::int32> sub = view.subView(2, 3);
    TEST_ENSURE(sub.data() == array + 2);
    TEST_ENSURE(sub.begin().address() == array + 2);
    TEST_ENSURE(sub.end().address() == array + 5);

    ArrayView<mini::int32> nested = sub.subView(1, 10);
    TEST_ENSURE(nested.size() == 2);
    TEST_ENSURE(nested.first() == 3 && nested.last() == 4);

    ArrayView<mini::int32> ranged = sub.subView(sub.begin() + 1, sub.end());
    TEST_ENSURE(ranged == nested);
    TEST_ENSURE(ranged.data() == nested.data());

    ArrayView<mini::int32> empty = sub.subView(sub.begin() + 1, sub.begin() + 1);
    TEST_ENSURE(empty.empty());
    TEST_ENSURE(empty.data() == array + 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopBackOverflow()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    view.popBack(100);
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == array);

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopEmpty()
{
    ArrayView<mini::int32> empty;

    empty.popFront();
    empty.popFront(3);
    empty.popBack();
    empty.popBack(3);
    TEST_ENSURE(empty.empty());

    return 0;
}

[[no_inline]] constexpr mini::int32 testPopZero()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    view.popFront(0);
    view.popBack(0);
    TEST_ENSURE(view.size() == 8);
    TEST_ENSURE(view.data() == array);

    view.popFront(8);
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == array + 8);

    view = array;
    view.popFront(100);
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == array + 8);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidIteratorOutside()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    ArrayView<mini::int32> sub = view.subView(2, 4);

    TEST_ENSURE(!sub.validIterator(view.begin() + 1));
    TEST_ENSURE(sub.validIterator(view.begin() + 2));
    TEST_ENSURE(sub.validIterator(view.begin() + 5));
    TEST_ENSURE(!sub.validIterator(view.begin() + 6));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidRangeOutside()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;
    ArrayView<mini::int32> sub = view.subView(2, 4);

    TEST_ENSURE(sub.validRange(view.begin() + 2, view.begin() + 6));
    TEST_ENSURE(sub.validRange(view.begin() + 3, view.begin() + 5));
    TEST_ENSURE(!sub.validRange(view.begin(), view.begin() + 4));
    TEST_ENSURE(!sub.validRange(view.begin() + 3, view.begin() + 7));
    TEST_ENSURE(!sub.validRange(view.begin() + 6, view.end()));

    return 0;
}

[[no_inline]] constexpr mini::int32 testValidRangeReversed()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(!view.validRange(view.begin() + 5, view.begin() + 2));

    return 0;
}

[[no_inline]] constexpr mini::int32 testPointerCtorEmpty()
{
    mini::int32 array[] = {0, 1, 2, 3};
    ArrayView<mini::int32> view(array + 2, 0);

    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == array + 2);
    TEST_ENSURE(view.begin() == view.end());

    return 0;
}

[[no_inline]] constexpr mini::int32 testConstArrayCtor()
{
    mini::int32 const array[] = {4, 5, 6};
    ArrayView<mini::int32 const> view = array;

    TEST_ENSURE(view.size() == 3);
    TEST_ENSURE(view.data() == array);
    TEST_ENSURE(view.first() == 4 && view.last() == 6);

    return 0;
}

[[no_inline]] constexpr mini::int32 testSingleElement()
{
    mini::int32 array[] = {42};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view.size() == 1);
    TEST_ENSURE(&view.first() == &view.last());
    TEST_ENSURE(view.first() == 42);
    TEST_ENSURE(view.subFront(1) == view);
    TEST_ENSURE(view.subBack(1) == view);

    view.popFront();
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == array + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testReassign()
{
    mini::int32 first[] = {0, 1, 2};
    mini::int32 second[] = {5, 6, 7, 8, 9};
    ArrayView<mini::int32> view = second;

    view = ArrayView<mini::int32>(first).subBack(2);
    TEST_ENSURE(view.size() == 2);
    TEST_ENSURE(view.first() == 1);

    view = ArrayView<mini::int32>();
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view.data() == nullptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testAliasing()
{
    mini::int32 array[] = {0, 1, 2, 3, 4, 5, 6, 7};
    ArrayView<mini::int32> view = array;

    array[3] = 30;
    TEST_ENSURE(view[3] == 30);

    return 0;
}

[[no_inline]] constexpr mini::int32 testEqualOverlapping()
{
    mini::int32 array[] = {1, 1, 1, 1, 2};
    ArrayView<mini::int32> view = array;

    TEST_ENSURE(view.subView(0, 3) == view.subView(1, 3));
    TEST_ENSURE(view.subView(1, 3) != view.subView(2, 3));
    TEST_ENSURE(view.subFront(0) == ArrayView<mini::int32>());

    ArrayView<mini::int32 const> constView = view;
    TEST_ENSURE(constView == view);
    TEST_ENSURE(view == constView);

    return 0;
}

mini::int32 main()
{
    TEST_CONSTEXPR_TEMPLATE(testCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCArrayCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testCArrayCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCArrayCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testCArrayCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testCArrayCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testInitializerListCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPointerCtor, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPointerCtor, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPointerCtor, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPointerCtor, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPointerCtor, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSubFront, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSubFront, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSubFront, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testSubFront, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testSubFront, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSubBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSubBack, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSubBack, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testSubBack, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testSubBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testSubView, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testSubView, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSubView, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testSubView, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testSubView, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopFront, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFront, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopFrontCount, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopFrontCount, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFrontCount, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFrontCount, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopFrontCount, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopBack, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBack, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testPopBackCount, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignList, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignList, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignList, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignList, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignList, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testAssignRawArray, mini::int32*);
    TEST_CONSTEXPR_TEMPLATE(testAssignRawArray, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignRawArray, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignRawArray, NoDefaultObject);
    TEST_CONSTEXPR_TEMPLATE(testAssignRawArray, OveralignedObject);

    TEST_CONSTEXPR(testBeginEnd);
    TEST_CONSTEXPR(testRangeFor);
    TEST_CONSTEXPR(testValidIndex);
    TEST_CONSTEXPR(testValidIterator);
    TEST_CONSTEXPR(testValidRange);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testNotEqual);

    TEST_CONSTEXPR(testConvertToConst);
    TEST_CONSTEXPR(testConvertPointerConst);

    TEST_CONSTEXPR(testSubFrontBoundary);
    TEST_CONSTEXPR(testSubBackBoundary);
    TEST_CONSTEXPR(testSubViewIndexBoundary);
    TEST_CONSTEXPR(testSubViewIteratorPosition);

    TEST_CONSTEXPR(testPopBackOverflow);
    TEST_CONSTEXPR(testPopEmpty);
    TEST_CONSTEXPR(testPopZero);

    TEST_CONSTEXPR(testValidIteratorOutside);
    TEST_CONSTEXPR(testValidRangeOutside);
    TEST_CONSTEXPR(testValidRangeReversed);

    TEST_CONSTEXPR(testPointerCtorEmpty);
    TEST_CONSTEXPR(testConstArrayCtor);
    TEST_CONSTEXPR(testSingleElement);
    TEST_CONSTEXPR(testReassign);

    TEST_CONSTEXPR(testAliasing);
    TEST_CONSTEXPR(testEqualOverlapping);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
