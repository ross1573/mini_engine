#include "test_macro.h"

import mini.test;

using mini::ArrayView;
using mini::PtrIterator;

using View = ArrayView<mini::int32>;

constexpr mini::int32 viewPool[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

template <typename T>
using Iter = PtrIterator<T*>;

template <typename T>
using ConstIter = PtrIterator<T const*>;

template <typename T>
constexpr T makeValue(mini::int32 value)
{
    if constexpr (mini::SameAsT<T, View>) {
        return View(viewPool, static_cast<mini::size_t>(value));
    } else {
        return makeObject<T>(value);
    }
}

template <typename T, typename U>
concept CopyRangeT = requires(T dest, U src) {
    mini::copyRange(dest, src, src);
    mini::copyBackward(dest, src, src);
};

template <typename T, typename U>
concept MoveRangeT = requires(T dest, U src) {
    mini::moveRange(dest, src, src);
    mini::moveBackward(dest, src, src);
};

template <typename T, typename U>
concept EqualRangeT = requires(T lhs, U rhs) { mini::equalRange(lhs, lhs, rhs, rhs); };

struct IntPredicate {
    constexpr bool operator()(mini::int32 value) const { return value > 2; }
};

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyRange()
{
    TEST_ENSURE((CopyRangeT<Iter<T>, ConstIter<T>>));

    if constexpr (CopyRangeT<Iter<T>, ConstIter<T>>) {
        T src[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
            makeValue<T>(4),
        };
        T dest[] = {
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
        };

        mini::copyRange(Iter<T>(dest), ConstIter<T>(src), ConstIter<T>(src + 4));
        for (mini::int32 i = 0; i < 4; ++i) {
            TEST_ENSURE(dest[i] == makeValue<T>(i + 1));
            TEST_ENSURE(src[i] == makeValue<T>(i + 1));
        }

        mini::copyRange(Iter<T>(dest + 1), ConstIter<T>(src + 2), ConstIter<T>(src + 4));
        TEST_ENSURE(dest[0] == makeValue<T>(1));
        TEST_ENSURE(dest[1] == makeValue<T>(3));
        TEST_ENSURE(dest[2] == makeValue<T>(4));
        TEST_ENSURE(dest[3] == makeValue<T>(4));

        mini::copyRange(Iter<T>(dest), ConstIter<T>(src), ConstIter<T>(src));
        TEST_ENSURE(dest[0] == makeValue<T>(1));

        mini::copyRange(Iter<T>(dest), ConstIter<T>(dest + 1), ConstIter<T>(dest + 4));
        TEST_ENSURE(dest[0] == makeValue<T>(3));
        TEST_ENSURE(dest[1] == makeValue<T>(4));
        TEST_ENSURE(dest[2] == makeValue<T>(4));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyBackward()
{
    TEST_ENSURE((CopyRangeT<Iter<T>, ConstIter<T>>));

    if constexpr (CopyRangeT<Iter<T>, ConstIter<T>>) {
        T src[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
        };
        T dest[] = {
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
        };

        mini::copyBackward(Iter<T>(dest + 4), ConstIter<T>(src), ConstIter<T>(src + 3));
        TEST_ENSURE(dest[0] == makeValue<T>(0));
        TEST_ENSURE(dest[1] == makeValue<T>(1));
        TEST_ENSURE(dest[2] == makeValue<T>(2));
        TEST_ENSURE(dest[3] == makeValue<T>(3));
        TEST_ENSURE(src[0] == makeValue<T>(1));

        mini::copyBackward(Iter<T>(dest + 1), ConstIter<T>(src), ConstIter<T>(src));
        TEST_ENSURE(dest[0] == makeValue<T>(0));

        T data[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
            makeValue<T>(4),
            makeValue<T>(5),
        };
        mini::copyBackward(Iter<T>(data + 5), ConstIter<T>(data), ConstIter<T>(data + 4));
        TEST_ENSURE(data[0] == makeValue<T>(1));
        TEST_ENSURE(data[1] == makeValue<T>(1));
        TEST_ENSURE(data[2] == makeValue<T>(2));
        TEST_ENSURE(data[3] == makeValue<T>(3));
        TEST_ENSURE(data[4] == makeValue<T>(4));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveRange()
{
    TEST_ENSURE((MoveRangeT<Iter<T>, Iter<T>>));

    if constexpr (MoveRangeT<Iter<T>, Iter<T>>) {
        T src[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
        };
        T dest[] = {
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
        };

        mini::moveRange(Iter<T>(dest), Iter<T>(src), Iter<T>(src + 3));
        for (mini::int32 i = 0; i < 3; ++i) {
            TEST_ENSURE(dest[i] == makeValue<T>(i + 1));
            if constexpr (TestObjectT<T>) {
                TEST_ENSURE(src[i].resource() == nullptr);
            }
        }

        T data[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
            makeValue<T>(4),
        };
        mini::moveRange(Iter<T>(data), Iter<T>(data + 1), Iter<T>(data + 4));
        TEST_ENSURE(data[0] == makeValue<T>(2));
        TEST_ENSURE(data[1] == makeValue<T>(3));
        TEST_ENSURE(data[2] == makeValue<T>(4));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(data[3].resource() == nullptr);
        }

        mini::moveRange(Iter<T>(dest), Iter<T>(data), Iter<T>(data));
        TEST_ENSURE(dest[0] == makeValue<T>(1));

        mini::moveRange(Iter<T>(dest), Iter<T>(dest), Iter<T>(dest + 3));
        TEST_ENSURE(dest[0] == makeValue<T>(1));
        TEST_ENSURE(dest[1] == makeValue<T>(2));
        TEST_ENSURE(dest[2] == makeValue<T>(3));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveBackward()
{
    TEST_ENSURE((MoveRangeT<Iter<T>, Iter<T>>));

    if constexpr (MoveRangeT<Iter<T>, Iter<T>>) {
        T src[] = {
            makeValue<T>(1),
            makeValue<T>(2),
        };
        T dest[] = {
            makeValue<T>(0),
            makeValue<T>(0),
            makeValue<T>(0),
        };

        mini::moveBackward(Iter<T>(dest + 3), Iter<T>(src), Iter<T>(src + 2));
        TEST_ENSURE(dest[0] == makeValue<T>(0));
        TEST_ENSURE(dest[1] == makeValue<T>(1));
        TEST_ENSURE(dest[2] == makeValue<T>(2));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(src[0].resource() == nullptr);
            TEST_ENSURE(src[1].resource() == nullptr);
        }

        T data[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
            makeValue<T>(4),
        };
        mini::moveBackward(Iter<T>(data + 4), Iter<T>(data), Iter<T>(data + 3));
        TEST_ENSURE(data[1] == makeValue<T>(1));
        TEST_ENSURE(data[2] == makeValue<T>(2));
        TEST_ENSURE(data[3] == makeValue<T>(3));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(data[0].resource() == nullptr);
        }

        mini::moveBackward(Iter<T>(dest + 3), Iter<T>(data), Iter<T>(data));
        TEST_ENSURE(dest[2] == makeValue<T>(2));

        mini::moveBackward(Iter<T>(dest + 3), Iter<T>(dest), Iter<T>(dest + 3));
        TEST_ENSURE(dest[0] == makeValue<T>(0));
        TEST_ENSURE(dest[1] == makeValue<T>(1));
        TEST_ENSURE(dest[2] == makeValue<T>(2));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testEqualRange()
{
    TEST_ENSURE((EqualRangeT<ConstIter<T>, ConstIter<T>>));
    TEST_ENSURE((EqualRangeT<Iter<T>, ConstIter<T>>));

    if constexpr (EqualRangeT<ConstIter<T>, ConstIter<T>> && EqualRangeT<Iter<T>, ConstIter<T>>) {
        T lhs[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
        };
        T rhs[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(3),
        };
        T other[] = {
            makeValue<T>(1),
            makeValue<T>(2),
            makeValue<T>(4),
        };

        TEST_ENSURE(
            mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs + 3), ConstIter<T>(rhs), ConstIter<T>(rhs + 3)));
        TEST_ENSURE(mini::equalRange(Iter<T>(lhs), Iter<T>(lhs + 3), ConstIter<T>(rhs), ConstIter<T>(rhs + 3)));
        TEST_ENSURE(
            !mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs + 3), ConstIter<T>(other), ConstIter<T>(other + 3)));
        TEST_ENSURE(
            mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs + 2), ConstIter<T>(other), ConstIter<T>(other + 2)));
        TEST_ENSURE(
            !mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs + 3), ConstIter<T>(rhs), ConstIter<T>(rhs + 2)));
        TEST_ENSURE(
            !mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs + 2), ConstIter<T>(rhs), ConstIter<T>(rhs + 3)));
        TEST_ENSURE(mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs), ConstIter<T>(rhs), ConstIter<T>(rhs)));
        TEST_ENSURE(!mini::equalRange(ConstIter<T>(lhs), ConstIter<T>(lhs), ConstIter<T>(rhs), ConstIter<T>(rhs + 1)));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFillRange()
{
    T data[] = {
        makeValue<T>(1),
        makeValue<T>(2),
        makeValue<T>(3),
        makeValue<T>(4),
    };

    T const value = makeValue<T>(7);
    mini::fillRange(Iter<T>(data), Iter<T>(data + 4), value);
    for (T const& element : data) {
        TEST_ENSURE(element == value);
    }

    mini::fillRange(Iter<T>(data + 1), Iter<T>(data + 3), makeValue<T>(5));
    TEST_ENSURE(data[0] == makeValue<T>(7));
    TEST_ENSURE(data[1] == makeValue<T>(5));
    TEST_ENSURE(data[2] == makeValue<T>(5));
    TEST_ENSURE(data[3] == makeValue<T>(7));

    mini::fillRange(Iter<T>(data), Iter<T>(data), makeValue<T>(1));
    TEST_ENSURE(data[0] == makeValue<T>(7));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFind()
{
    T data[] = {
        makeValue<T>(1),
        makeValue<T>(2),
        makeValue<T>(3),
        makeValue<T>(2),
    };
    Iter<T> begin(data);
    Iter<T> end(data + 4);

    TEST_ENSURE(mini::find(begin, end, makeValue<T>(1)) == begin);
    TEST_ENSURE(mini::find(begin, end, makeValue<T>(2)) == begin + 1);
    TEST_ENSURE(mini::find(begin, end, makeValue<T>(5)) == end);
    TEST_ENSURE(mini::find(begin + 2, end, makeValue<T>(2)) == begin + 3);
    TEST_ENSURE(mini::find(begin, begin + 1, makeValue<T>(2)) == begin + 1);
    TEST_ENSURE(mini::find(begin, begin, makeValue<T>(1)) == begin);

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename ViewT>
[[no_inline]] constexpr mini::int32 testCheckedEqualRange()
{
    typedef typename ViewT::ConstIterator ViewIter;
    TEST_ENSURE((EqualRangeT<ViewIter, ViewIter>));

    if constexpr (EqualRangeT<ViewIter, ViewIter>) {
        mini::int32 lhs[] = {3, 1, 4};
        mini::int32 rhs[] = {3, 1, 4};
        ViewT lview(lhs);
        ViewT rview(rhs);

        TEST_ENSURE(mini::equalRange(lview.begin(), lview.end(), rview.begin(), rview.end()));
        TEST_ENSURE(!mini::equalRange(lview.begin(), lview.end(), rview.begin(), rview.end() - 1));
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testDistance()
{
    mini::int32 data[] = {1, 2, 3, 4};

    TEST_ENSURE(mini::distance(Iter<mini::int32>(data), Iter<mini::int32>(data + 4)) == 4);
    TEST_ENSURE(mini::distance(Iter<mini::int32>(data + 1), Iter<mini::int32>(data + 3)) == 2);
    TEST_ENSURE(mini::distance(Iter<mini::int32>(data), Iter<mini::int32>(data)) == 0);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIf()
{
    mini::int32 data[] = {1, 4, 2, 5};
    Iter<mini::int32> begin(data);
    Iter<mini::int32> end(data + 4);

    TEST_ENSURE(mini::findIf(begin, end, IntPredicate{}) == begin + 1);
    TEST_ENSURE(mini::findIf(begin + 2, end, IntPredicate{}) == begin + 3);
    TEST_ENSURE(mini::findIf(begin, end, [](mini::int32 value) { return value > 10; }) == end);
    TEST_ENSURE(mini::findIf(begin, begin, IntPredicate{}) == begin);

    mini::int32 calls = 0;
    auto counting = [&calls](mini::int32 value) {
        ++calls;
        return value == 2;
    };
    TEST_ENSURE(mini::findIf(begin, end, counting) == begin + 2);
    TEST_ENSURE(calls == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIfNot()
{
    mini::int32 data[] = {4, 5, 1, 6};
    Iter<mini::int32> begin(data);
    Iter<mini::int32> end(data + 4);

    TEST_ENSURE(mini::findIfNot(begin, end, IntPredicate{}) == begin + 2);
    TEST_ENSURE(mini::findIfNot(begin + 3, end, IntPredicate{}) == end);
    TEST_ENSURE(mini::findIfNot(begin, end, [](mini::int32 value) { return value < 10; }) == end);
    TEST_ENSURE(mini::findIfNot(begin, begin, IntPredicate{}) == begin);

    mini::int32 calls = 0;
    auto counting = [&calls](mini::int32 value) {
        ++calls;
        return value != 1;
    };
    TEST_ENSURE(mini::findIfNot(begin, end, counting) == begin + 2);
    TEST_ENSURE(calls == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFillRangeConvert()
{
    mini::int64 data[3] = {};
    mini::fillRange(Iter<mini::int64>(data), Iter<mini::int64>(data + 3), mini::int16(-3));
    TEST_ENSURE(data[0] == -3 && data[1] == -3 && data[2] == -3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindConvert()
{
    mini::int32 data[] = {1, 2, 3};
    Iter<mini::int32> begin(data);
    Iter<mini::int32> end(data + 3);

    TEST_ENSURE(mini::find(begin, end, mini::int64(3)) == begin + 2);
    TEST_ENSURE(mini::find(begin, end, mini::int64(1) << 32) == end);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedIterator()
{
    mini::int32 data[] = {3, 1, 4, 1, 5};
    View view(data);

    TEST_ENSURE(mini::distance(view.begin(), view.end()) == 5);
    TEST_ENSURE(mini::distance(view.begin() + 1, view.begin() + 3) == 2);
    TEST_ENSURE(mini::find(view.begin(), view.end(), 4) == view.begin() + 2);
    TEST_ENSURE(mini::find(view.begin(), view.end(), 9) == view.end());
    TEST_ENSURE(mini::findIf(view.begin(), view.end(), IntPredicate{}) == view.begin());
    TEST_ENSURE(mini::findIfNot(view.begin(), view.end(), IntPredicate{}) == view.begin() + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedRange()
{
    View src[] = {
        makeValue<View>(1),
        makeValue<View>(2),
        makeValue<View>(3),
    };
    ArrayView<View> srcView(src);
    mini::Array<View> dest = srcView;

    mini::fillRange(dest.begin(), dest.end(), makeValue<View>(0));
    TEST_ENSURE(dest[0] == makeValue<View>(0));
    TEST_ENSURE(dest[1] == makeValue<View>(0));
    TEST_ENSURE(dest[2] == makeValue<View>(0));

    mini::copyRange(dest.begin(), srcView.begin(), srcView.end());
    TEST_ENSURE(dest[0] == makeValue<View>(1));
    TEST_ENSURE(dest[1] == makeValue<View>(2));
    TEST_ENSURE(dest[2] == makeValue<View>(3));

    mini::fillRange(dest.begin() + 1, dest.end(), makeValue<View>(5));
    TEST_ENSURE(dest[0] == makeValue<View>(1));
    TEST_ENSURE(dest[1] == makeValue<View>(5));
    TEST_ENSURE(dest[2] == makeValue<View>(5));

    mini::moveRange(dest.begin(), Iter<View>(src + 1), Iter<View>(src + 3));
    TEST_ENSURE(dest[0] == makeValue<View>(2));
    TEST_ENSURE(dest[1] == makeValue<View>(3));
    TEST_ENSURE(dest[2] == makeValue<View>(5));

    mini::copyRange(dest.begin(), srcView.begin(), srcView.begin());
    TEST_ENSURE(dest[0] == makeValue<View>(2));

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedBackward()
{
    View src[] = {
        makeValue<View>(1),
        makeValue<View>(2),
    };
    mini::Array<View> dest = ArrayView<View>(src);

    mini::copyBackward(dest.end(), Iter<View>(src), Iter<View>(src + 2));
    TEST_ENSURE(dest[0] == makeValue<View>(1));
    TEST_ENSURE(dest[1] == makeValue<View>(2));

    mini::moveBackward(dest.end(), Iter<View>(src), Iter<View>(src + 2));
    TEST_ENSURE(dest[0] == makeValue<View>(1));
    TEST_ENSURE(dest[1] == makeValue<View>(2));

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedEmptyRangeAtEnd()
{
    View src[] = {
        makeValue<View>(1),
        makeValue<View>(2),
    };
    mini::Array<View> dest = ArrayView<View>(src);

    mini::copyRange(dest.end(), Iter<View>(src), Iter<View>(src));
    mini::moveRange(dest.end(), Iter<View>(src), Iter<View>(src));
    mini::copyBackward(dest.begin(), Iter<View>(src), Iter<View>(src));
    mini::moveBackward(dest.begin(), Iter<View>(src), Iter<View>(src));
    TEST_ENSURE(dest[0] == makeValue<View>(1));
    TEST_ENSURE(dest[1] == makeValue<View>(2));

    return 0;
}

int main()
{
    TEST_FUNCTION_TEMPLATE(testCopyRange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCopyRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, View);

    TEST_FUNCTION_TEMPLATE(testCopyBackward, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCopyBackward, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyBackward, View);

    TEST_FUNCTION_TEMPLATE(testMoveRange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testMoveRange, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveRange, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveRange, View);

    TEST_FUNCTION_TEMPLATE(testMoveBackward, mini::int32);
    TEST_FUNCTION_TEMPLATE(testMoveBackward, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveBackward, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveBackward, View);

    TEST_FUNCTION_TEMPLATE(testEqualRange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testEqualRange, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testEqualRange, View);

    TEST_CONSTEXPR_TEMPLATE(testFillRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFillRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testFillRange, View);

    TEST_CONSTEXPR_TEMPLATE(testFind, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFind, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testFind, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testFind, View);

    TEST_FUNCTION_TEMPLATE(testCheckedEqualRange, View);

    TEST_CONSTEXPR(testDistance);
    TEST_CONSTEXPR(testFindIf);
    TEST_CONSTEXPR(testFindIfNot);

    TEST_CONSTEXPR(testFillRangeConvert);
    TEST_CONSTEXPR(testFindConvert);

    TEST_CONSTEXPR(testCheckedIterator);
    TEST_FUNCTION(testCheckedRange);
    TEST_FUNCTION(testCheckedBackward);
    TEST_FUNCTION(testCheckedEmptyRangeAtEnd);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
