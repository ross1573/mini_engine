#include "test_macro.h"

import mini.test;

using mini::PtrIterator;

struct IntPredicate {
    constexpr bool operator()(mini::int32 value) const { return value > 2; }
};

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyRange()
{
    T src[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };
    T dest[] = {
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
    };

    T const* csrc = src;
    mini::memory::copyRange(dest, csrc, csrc + 4);
    for (mini::int32 i = 0; i < 4; ++i) {
        TEST_ENSURE(dest[i] == makeObject<T>(i + 1));
        TEST_ENSURE(src[i] == makeObject<T>(i + 1));
    }

    mini::memory::copyRange(dest + 1, src + 2, src + 4);
    TEST_ENSURE(dest[0] == makeObject<T>(1));
    TEST_ENSURE(dest[1] == makeObject<T>(3));
    TEST_ENSURE(dest[2] == makeObject<T>(4));
    TEST_ENSURE(dest[3] == makeObject<T>(4));

    mini::memory::copyRange(dest, src, src);
    TEST_ENSURE(dest[0] == makeObject<T>(1));

    mini::memory::copyRange(PtrIterator<T*>(dest), PtrIterator<T const*>(src + 1), PtrIterator<T const*>(src + 3));
    TEST_ENSURE(dest[0] == makeObject<T>(2));
    TEST_ENSURE(dest[1] == makeObject<T>(3));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyBackward()
{
    T src[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T dest[] = {
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
    };

    T const* csrc = src;
    mini::memory::copyBackward(dest + 4, csrc, csrc + 3);
    TEST_ENSURE(dest[0] == makeObject<T>(0));
    TEST_ENSURE(dest[1] == makeObject<T>(1));
    TEST_ENSURE(dest[2] == makeObject<T>(2));
    TEST_ENSURE(dest[3] == makeObject<T>(3));

    mini::memory::copyBackward(dest + 1, src, src);
    TEST_ENSURE(dest[0] == makeObject<T>(0));

    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(5),
    };
    mini::memory::copyBackward(data + 5, data, data + 4);
    TEST_ENSURE(data[0] == makeObject<T>(1));
    TEST_ENSURE(data[1] == makeObject<T>(1));
    TEST_ENSURE(data[2] == makeObject<T>(2));
    TEST_ENSURE(data[3] == makeObject<T>(3));
    TEST_ENSURE(data[4] == makeObject<T>(4));

    mini::memory::copyBackward(PtrIterator<T*>(dest + 2),
                               PtrIterator<T const*>(src + 1),
                               PtrIterator<T const*>(src + 3));
    TEST_ENSURE(dest[0] == makeObject<T>(2));
    TEST_ENSURE(dest[1] == makeObject<T>(3));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveRange()
{
    T src[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T dest[] = {
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
    };

    mini::memory::moveRange(dest, src, src + 3);
    for (mini::int32 i = 0; i < 3; ++i) {
        TEST_ENSURE(dest[i] == makeObject<T>(i + 1));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(src[i].resource() == nullptr);
        }
    }

    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };
    mini::memory::moveRange(data, data + 1, data + 4);
    TEST_ENSURE(data[0] == makeObject<T>(2));
    TEST_ENSURE(data[1] == makeObject<T>(3));
    TEST_ENSURE(data[2] == makeObject<T>(4));
    if constexpr (TestObjectT<T>) {
        TEST_ENSURE(data[3].resource() == nullptr);
    }

    mini::memory::moveRange(PtrIterator<T*>(src), PtrIterator<T*>(dest), PtrIterator<T*>(dest + 2));
    TEST_ENSURE(src[0] == makeObject<T>(1));
    TEST_ENSURE(src[1] == makeObject<T>(2));

    mini::memory::moveRange(dest, data, data);
    TEST_ENSURE(dest[2] == makeObject<T>(3));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveBackward()
{
    T src[] = {
        makeObject<T>(1),
        makeObject<T>(2),
    };
    T dest[] = {
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
    };

    mini::memory::moveBackward(dest + 3, src, src + 2);
    TEST_ENSURE(dest[0] == makeObject<T>(0));
    TEST_ENSURE(dest[1] == makeObject<T>(1));
    TEST_ENSURE(dest[2] == makeObject<T>(2));
    if constexpr (TestObjectT<T>) {
        TEST_ENSURE(src[0].resource() == nullptr);
        TEST_ENSURE(src[1].resource() == nullptr);
    }

    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };
    mini::memory::moveBackward(data + 4, data, data + 3);
    TEST_ENSURE(data[1] == makeObject<T>(1));
    TEST_ENSURE(data[2] == makeObject<T>(2));
    TEST_ENSURE(data[3] == makeObject<T>(3));
    if constexpr (TestObjectT<T>) {
        TEST_ENSURE(data[0].resource() == nullptr);
    }

    mini::memory::moveBackward(PtrIterator<T*>(src + 2), PtrIterator<T*>(dest + 1), PtrIterator<T*>(dest + 3));
    TEST_ENSURE(src[0] == makeObject<T>(1));
    TEST_ENSURE(src[1] == makeObject<T>(2));

    mini::memory::moveBackward(dest + 3, data, data);
    TEST_ENSURE(dest[0] == makeObject<T>(0));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testEqualRangeSized()
{
    T lhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T rhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T other[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(4),
    };
    T const* clhs = lhs;
    T const* crhs = rhs;
    T const* cother = other;

    TEST_ENSURE(mini::memory::equalRange(clhs, crhs, crhs + 3));
    TEST_ENSURE(!mini::memory::equalRange(clhs, cother, cother + 3));
    TEST_ENSURE(mini::memory::equalRange(clhs, cother, cother + 2));
    TEST_ENSURE(mini::memory::equalRange(clhs, crhs, crhs));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testEqualRangeBounded()
{
    T lhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T rhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T other[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(4),
    };
    T const* clhs = lhs;
    T const* crhs = rhs;
    T const* cother = other;

    TEST_ENSURE(mini::memory::equalRange(clhs, clhs + 3, crhs, crhs + 3));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 3, cother, cother + 3));
    TEST_ENSURE(mini::memory::equalRange(clhs, clhs + 2, cother, cother + 2));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 3, crhs, crhs + 2));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 2, crhs, crhs + 3));
    TEST_ENSURE(mini::memory::equalRange(clhs, clhs, crhs, crhs));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testEqualRangeIterator()
{
    T lhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T rhs[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(4),
    };
    PtrIterator<T const*> lbegin(lhs);
    PtrIterator<T const*> rbegin(rhs);

    TEST_ENSURE(mini::memory::equalRange(lbegin, rbegin, rbegin + 2));
    TEST_ENSURE(!mini::memory::equalRange(lbegin, rbegin, rbegin + 3));
    TEST_ENSURE(mini::memory::equalRange(lbegin, lbegin + 2, rbegin, rbegin + 2));
    TEST_ENSURE(!mini::memory::equalRange(lbegin, lbegin + 3, rbegin, rbegin + 3));
    TEST_ENSURE(!mini::memory::equalRange(lbegin, lbegin + 2, rbegin, rbegin + 1));
    TEST_ENSURE(!mini::memory::equalRange(lbegin, lbegin + 1, rbegin, rbegin + 2));
    TEST_ENSURE(mini::memory::equalRange(lbegin, lbegin, rbegin, rbegin));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFillRange()
{
    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    T const value = makeObject<T>(7);
    mini::memory::fillRange(data, data + 4, value);
    for (T const& element : data) {
        TEST_ENSURE(element == value);
    }

    mini::memory::fillRange(data + 1, data + 3, makeObject<T>(5));
    TEST_ENSURE(data[0] == makeObject<T>(7));
    TEST_ENSURE(data[1] == makeObject<T>(5));
    TEST_ENSURE(data[2] == makeObject<T>(5));
    TEST_ENSURE(data[3] == makeObject<T>(7));

    mini::memory::fillRange(PtrIterator<T*>(data), PtrIterator<T*>(data + 1), makeObject<T>(6));
    TEST_ENSURE(data[0] == makeObject<T>(6));
    TEST_ENSURE(data[1] == makeObject<T>(5));

    mini::memory::fillRange(data, data, makeObject<T>(1));
    TEST_ENSURE(data[0] == makeObject<T>(6));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFind()
{
    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(2),
    };
    T* begin = data;
    T* end = data + 4;

    TEST_ENSURE(mini::memory::find(begin, end, makeObject<T>(1)) == begin);
    TEST_ENSURE(mini::memory::find(begin, end, makeObject<T>(2)) == begin + 1);
    TEST_ENSURE(mini::memory::find(begin + 2, end, makeObject<T>(2)) == begin + 3);
    TEST_ENSURE(mini::memory::find(begin, end, makeObject<T>(5)) == end);
    TEST_ENSURE(mini::memory::find(begin, begin, makeObject<T>(1)) == begin);

    PtrIterator<T*> iter(data);
    TEST_ENSURE(mini::memory::find(iter, iter + 4, makeObject<T>(3)) == iter + 2);
    TEST_ENSURE(mini::memory::find(iter, iter + 4, makeObject<T>(9)) == iter + 4);

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMixedIterator()
{
    T src[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T dest[] = {
        makeObject<T>(0),
        makeObject<T>(0),
        makeObject<T>(0),
    };

    mini::memory::copyRange(dest, PtrIterator<T const*>(src), PtrIterator<T const*>(src + 3));
    TEST_ENSURE(dest[0] == makeObject<T>(1));
    TEST_ENSURE(dest[2] == makeObject<T>(3));

    mini::memory::fillRange(dest, dest + 3, makeObject<T>(0));
    T const* csrc = src;
    mini::memory::copyRange(PtrIterator<T*>(dest + 1), csrc, csrc + 2);
    TEST_ENSURE(dest[0] == makeObject<T>(0));
    TEST_ENSURE(dest[1] == makeObject<T>(1));
    TEST_ENSURE(dest[2] == makeObject<T>(2));

    mini::memory::copyBackward(PtrIterator<T*>(dest + 2), csrc + 1, csrc + 3);
    TEST_ENSURE(dest[0] == makeObject<T>(2));
    TEST_ENSURE(dest[1] == makeObject<T>(3));
    TEST_ENSURE(dest[2] == makeObject<T>(2));

    TEST_ENSURE(mini::memory::equalRange(PtrIterator<T const*>(src), csrc, csrc + 3));
    TEST_ENSURE(!mini::memory::equalRange(PtrIterator<T const*>(dest), csrc, csrc + 3));
    TEST_ENSURE(
        !mini::memory::equalRange(PtrIterator<T const*>(src + 1), PtrIterator<T const*>(src + 3), dest + 1, dest + 3));
    TEST_ENSURE(
        mini::memory::equalRange(PtrIterator<T const*>(src + 1), PtrIterator<T const*>(src + 3), dest, dest + 2));
    TEST_ENSURE(
        mini::memory::equalRange(PtrIterator<T const*>(src + 1), PtrIterator<T const*>(src + 2), dest + 2, dest + 3));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testCopyRangeConvert()
{
    mini::int16 src[] = {-1, 2, -3};
    mini::int64 dest[] = {0, 0, 0, 0};

    mini::memory::copyRange(dest, src, src + 3);
    TEST_ENSURE(dest[0] == -1 && dest[1] == 2 && dest[2] == -3 && dest[3] == 0);

    mini::memory::copyBackward(dest + 4, src, src + 2);
    TEST_ENSURE(dest[0] == -1 && dest[1] == 2 && dest[2] == -1 && dest[3] == 2);

    mini::memory::moveRange(PtrIterator<mini::int64*>(dest), src + 2, src + 3);
    TEST_ENSURE(dest[0] == -3 && dest[1] == 2);

    mini::memory::moveBackward(dest + 2, PtrIterator<mini::int16*>(src), PtrIterator<mini::int16*>(src + 1));
    TEST_ENSURE(dest[0] == -3 && dest[1] == -1 && dest[2] == -1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testEqualRangeConvert()
{
    mini::int32 lhs[] = {1, 2, 3};
    mini::int64 rhs[] = {1, 2, 3};
    mini::int64 other[] = {1, 2, 4};
    mini::int32 const* clhs = lhs;
    mini::int64 const* crhs = rhs;
    mini::int64 const* cother = other;

    TEST_ENSURE(mini::memory::equalRange(clhs, crhs, crhs + 3));
    TEST_ENSURE(!mini::memory::equalRange(clhs, cother, cother + 3));
    TEST_ENSURE(mini::memory::equalRange(clhs, cother, cother + 2));
    TEST_ENSURE(mini::memory::equalRange(clhs, clhs + 3, crhs, crhs + 3));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 3, cother, cother + 3));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 3, crhs, crhs + 2));
    TEST_ENSURE(!mini::memory::equalRange(clhs, clhs + 2, crhs, crhs + 3));

    return 0;
}

[[no_inline]] constexpr mini::int32 testFillRangeConvert()
{
    mini::int64 data[] = {1, 2, 3};

    mini::memory::fillRange(data, data + 3, mini::int16(-4));
    TEST_ENSURE(data[0] == -4 && data[1] == -4 && data[2] == -4);

    mini::int32 const* pointers[] = {nullptr, nullptr};
    mini::int32 target = 0;
    mini::memory::fillRange(pointers + 1, pointers + 2, &target);
    TEST_ENSURE(pointers[0] == nullptr && pointers[1] == &target);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindConvert()
{
    mini::int32 data[] = {1, 2, 3};
    mini::int32* begin = data;
    mini::int32* end = data + 3;

    TEST_ENSURE(mini::memory::find(begin, end, mini::int64(3)) == begin + 2);
    TEST_ENSURE(mini::memory::find(begin, end, mini::int64(2) << 32) == end);

    mini::int32* pointers[] = {data, nullptr, data + 1};
    TEST_ENSURE(mini::memory::find(pointers, pointers + 3, nullptr) == pointers + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIf()
{
    mini::int32 data[] = {1, 4, 2, 5};
    mini::int32* begin = data;
    mini::int32* end = data + 4;

    TEST_ENSURE(mini::memory::findIf(begin, end, IntPredicate{}) == begin + 1);
    TEST_ENSURE(mini::memory::findIf(begin + 2, end, IntPredicate{}) == begin + 3);
    TEST_ENSURE(mini::memory::findIf(begin, end, [](mini::int32 value) { return value > 10; }) == end);
    TEST_ENSURE(mini::memory::findIf(begin, begin, IntPredicate{}) == begin);

    PtrIterator<mini::int32 const*> iter(data);
    TEST_ENSURE(mini::memory::findIf(iter, iter + 4, IntPredicate{}) == iter + 1);

    mini::int32 calls = 0;
    auto counting = [&calls](mini::int32 value) {
        ++calls;
        return value == 2;
    };
    TEST_ENSURE(mini::memory::findIf(begin, end, counting) == begin + 2);
    TEST_ENSURE(calls == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIfNot()
{
    mini::int32 data[] = {4, 5, 1, 6};
    mini::int32* begin = data;
    mini::int32* end = data + 4;

    TEST_ENSURE(mini::memory::findIfNot(begin, end, IntPredicate{}) == begin + 2);
    TEST_ENSURE(mini::memory::findIfNot(begin + 3, end, IntPredicate{}) == end);
    TEST_ENSURE(mini::memory::findIfNot(begin, end, [](mini::int32 value) { return value < 10; }) == end);
    TEST_ENSURE(mini::memory::findIfNot(begin, begin, IntPredicate{}) == begin);

    PtrIterator<mini::int32 const*> iter(data);
    TEST_ENSURE(mini::memory::findIfNot(iter, iter + 4, IntPredicate{}) == iter + 2);

    mini::int32 calls = 0;
    auto counting = [&calls](mini::int32 value) {
        ++calls;
        return value != 1;
    };
    TEST_ENSURE(mini::memory::findIfNot(begin, end, counting) == begin + 2);
    TEST_ENSURE(calls == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIfObject()
{
    MoveOnlyObject data[] = {
        MoveOnlyObject(1),
        MoveOnlyObject(2),
        MoveOnlyObject(3),
    };
    MoveOnlyObject* begin = data;
    MoveOnlyObject* end = data + 3;

    TEST_ENSURE(mini::memory::findIf(begin, end, [](MoveOnlyObject const& value) { return value.value() == 2; }) ==
                begin + 1);
    TEST_ENSURE(mini::memory::findIfNot(begin, end, [](MoveOnlyObject const& value) { return value.value() < 3; }) ==
                begin + 2);
    TEST_ENSURE(mini::memory::findIf(begin, end, [](MoveOnlyObject const& value) { return value.value() > 3; }) == end);

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testCopyBackward, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testCopyBackward, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyBackward, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyBackward, OveralignedObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveRange, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveRange, MoveOnlyObject);

    TEST_CONSTEXPR_TEMPLATE(testMoveBackward, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMoveBackward, TrivialObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveBackward, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveBackward, MoveOnlyObject);

    TEST_FUNCTION_TEMPLATE(testEqualRangeSized, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testEqualRangeSized, CopyableObject);

    TEST_CONSTEXPR_TEMPLATE(testEqualRangeBounded, CopyableObject);

    TEST_CONSTEXPR_TEMPLATE(testEqualRangeIterator, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testEqualRangeIterator, CopyableObject);

    TEST_CONSTEXPR_TEMPLATE(testFillRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFillRange, CopyableObject);

    TEST_CONSTEXPR_TEMPLATE(testFind, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFind, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testFind, MoveOnlyObject);

    TEST_CONSTEXPR_TEMPLATE(testMixedIterator, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testMixedIterator, CopyableObject);

    TEST_CONSTEXPR(testCopyRangeConvert);
    TEST_CONSTEXPR(testEqualRangeConvert);
    TEST_CONSTEXPR(testFillRangeConvert);
    TEST_CONSTEXPR(testFindConvert);
    TEST_CONSTEXPR(testFindIf);
    TEST_CONSTEXPR(testFindIfNot);
    TEST_CONSTEXPR(testFindIfObject);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
