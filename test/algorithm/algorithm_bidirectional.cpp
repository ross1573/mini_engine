#include "test_macro.h"

import mini.test;

using View = mini::ArrayView<mini::int32>;

constexpr mini::int32 viewPool[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

template <typename T>
class BidirectionalIter {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

private:
    T* m_ptr = nullptr;
    T* m_begin = nullptr;
    T* m_end = nullptr;
    mini::int32* m_budget = nullptr;

public:
    constexpr BidirectionalIter() = default;
    constexpr BidirectionalIter(T* ptr, T* begin, T* end, mini::int32* budget)
        : m_ptr(ptr)
        , m_begin(begin)
        , m_end(end)
        , m_budget(budget)
    {
    }

    constexpr bool valid() const { return m_ptr != nullptr && m_ptr >= m_begin && m_ptr < m_end; }
    constexpr bool validWith(BidirectionalIter const& other) const
    {
        return valid() && m_begin == other.m_begin && m_ptr <= other.m_ptr && other.m_ptr <= m_end;
    }

    constexpr bool increment()
    {
        if (m_ptr + 1 >= m_end) {
            return false;
        }

        ++m_ptr;
        return true;
    }

    constexpr Pointer address() const { return m_ptr; }
    constexpr Pointer operator->() const { return m_ptr; }
    constexpr Reference operator*() const { return *m_ptr; }

    constexpr BidirectionalIter& operator++()
    {
        ++m_ptr;
        return *this;
    }

    constexpr BidirectionalIter operator++(mini::int32)
    {
        BidirectionalIter tmp(*this);
        ++m_ptr;
        return tmp;
    }

    constexpr bool decrement()
    {
        if (m_ptr <= m_begin) {
            return false;
        }

        --m_ptr;
        return true;
    }

    constexpr BidirectionalIter& operator--()
    {
        --m_ptr;
        return *this;
    }

    constexpr BidirectionalIter operator--(mini::int32)
    {
        BidirectionalIter tmp(*this);
        --m_ptr;
        return tmp;
    }

    constexpr bool operator==(BidirectionalIter const& other) const
    {
        if (m_budget != nullptr && --*m_budget < 0) {
            return true;
        }

        return m_ptr == other.m_ptr;
    }
};

template <typename T, mini::size_t N>
class Range {
public:
    T data[N];
    mini::int32 budget = 1000;

    constexpr BidirectionalIter<T> at(mini::size_t index)
    {
        return BidirectionalIter<T>(data + index, data, data + N, &budget);
    }

    constexpr BidirectionalIter<T> begin() { return at(0); }
    constexpr BidirectionalIter<T> end() { return at(N); }
};

template <typename T>
constexpr T makeValue(mini::int32 value)
{
    if constexpr (mini::SameAsT<T, View>) {
        return View(viewPool, static_cast<mini::size_t>(value));
    } else {
        return makeObject<T>(value);
    }
}

template <typename T>
constexpr Range<T, 4> makeRange(mini::int32 a, mini::int32 b, mini::int32 c, mini::int32 d)
{
    return Range<T, 4>{
        {makeValue<T>(a), makeValue<T>(b), makeValue<T>(c), makeValue<T>(d)}
    };
}

struct IntPredicate {
    constexpr bool operator()(mini::int32 value) const { return value > 2; }
};

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

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyRange()
{
    TEST_ENSURE((CopyRangeT<BidirectionalIter<T>, BidirectionalIter<T>>));

    if constexpr (CopyRangeT<BidirectionalIter<T>, BidirectionalIter<T>>) {
        Range<T, 4> src = makeRange<T>(1, 2, 3, 4);
        Range<T, 4> dest = makeRange<T>(0, 0, 0, 0);

        mini::copyRange(dest.begin(), src.begin(), src.end());
        for (mini::int32 i = 0; i < 4; ++i) {
            TEST_ENSURE(dest.data[i] == makeValue<T>(i + 1));
            TEST_ENSURE(src.data[i] == makeValue<T>(i + 1));
        }

        mini::copyRange(dest.at(1), src.at(2), src.end());
        TEST_ENSURE(dest.data[0] == makeValue<T>(1));
        TEST_ENSURE(dest.data[1] == makeValue<T>(3));
        TEST_ENSURE(dest.data[2] == makeValue<T>(4));
        TEST_ENSURE(dest.data[3] == makeValue<T>(4));

        mini::copyRange(dest.begin(), src.begin(), src.begin());
        TEST_ENSURE(dest.data[0] == makeValue<T>(1));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveRange()
{
    TEST_ENSURE((MoveRangeT<BidirectionalIter<T>, BidirectionalIter<T>>));

    if constexpr (MoveRangeT<BidirectionalIter<T>, BidirectionalIter<T>>) {
        Range<T, 4> src = makeRange<T>(1, 2, 3, 4);
        Range<T, 4> dest = makeRange<T>(0, 0, 0, 0);

        mini::moveRange(dest.begin(), src.begin(), src.at(3));
        TEST_ENSURE(dest.data[0] == makeValue<T>(1));
        TEST_ENSURE(dest.data[1] == makeValue<T>(2));
        TEST_ENSURE(dest.data[2] == makeValue<T>(3));
        TEST_ENSURE(dest.data[3] == makeValue<T>(0));

        Range<T, 4> data = makeRange<T>(1, 2, 3, 4);
        mini::moveRange(data.begin(), data.at(1), data.end());
        TEST_ENSURE(data.data[0] == makeValue<T>(2));
        TEST_ENSURE(data.data[1] == makeValue<T>(3));
        TEST_ENSURE(data.data[2] == makeValue<T>(4));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(data.data[3].resource() == nullptr);
        }
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testCopyBackward()
{
    TEST_ENSURE((CopyRangeT<BidirectionalIter<T>, BidirectionalIter<T>>));

    if constexpr (CopyRangeT<BidirectionalIter<T>, BidirectionalIter<T>>) {
        Range<T, 4> src = makeRange<T>(1, 2, 3, 4);
        Range<T, 4> dest = makeRange<T>(0, 0, 0, 0);

        mini::copyBackward(dest.end(), src.begin(), src.at(3));
        TEST_ENSURE(dest.data[0] == makeValue<T>(0));
        TEST_ENSURE(dest.data[1] == makeValue<T>(1));
        TEST_ENSURE(dest.data[2] == makeValue<T>(2));
        TEST_ENSURE(dest.data[3] == makeValue<T>(3));
        TEST_ENSURE(src.data[0] == makeValue<T>(1));

        Range<T, 4> data = makeRange<T>(1, 2, 3, 4);
        mini::copyBackward(data.end(), data.begin(), data.at(3));
        TEST_ENSURE(data.data[0] == makeValue<T>(1));
        TEST_ENSURE(data.data[1] == makeValue<T>(1));
        TEST_ENSURE(data.data[2] == makeValue<T>(2));
        TEST_ENSURE(data.data[3] == makeValue<T>(3));

        mini::copyBackward(dest.at(1), src.begin(), src.begin());
        TEST_ENSURE(dest.data[0] == makeValue<T>(0));
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveBackward()
{
    TEST_ENSURE((MoveRangeT<BidirectionalIter<T>, BidirectionalIter<T>>));

    if constexpr (MoveRangeT<BidirectionalIter<T>, BidirectionalIter<T>>) {
        Range<T, 4> src = makeRange<T>(1, 2, 3, 4);
        Range<T, 4> dest = makeRange<T>(0, 0, 0, 0);

        mini::moveBackward(dest.end(), src.at(1), src.end());
        TEST_ENSURE(dest.data[0] == makeValue<T>(0));
        TEST_ENSURE(dest.data[1] == makeValue<T>(2));
        TEST_ENSURE(dest.data[2] == makeValue<T>(3));
        TEST_ENSURE(dest.data[3] == makeValue<T>(4));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(src.data[1].resource() == nullptr);
            TEST_ENSURE(src.data[3].resource() == nullptr);
            TEST_ENSURE(src.data[0] == makeValue<T>(1));
        }

        Range<T, 4> data = makeRange<T>(1, 2, 3, 4);
        mini::moveBackward(data.end(), data.begin(), data.at(3));
        TEST_ENSURE(data.data[1] == makeValue<T>(1));
        TEST_ENSURE(data.data[2] == makeValue<T>(2));
        TEST_ENSURE(data.data[3] == makeValue<T>(3));
        if constexpr (TestObjectT<T>) {
            TEST_ENSURE(data.data[0].resource() == nullptr);
        }
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testEqualRange()
{
    Range<T, 4> lhs = makeRange<T>(1, 2, 3, 4);
    Range<T, 4> rhs = makeRange<T>(1, 2, 3, 4);
    Range<T, 4> other = makeRange<T>(1, 2, 3, 5);

    TEST_ENSURE(mini::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
    TEST_ENSURE(!mini::equalRange(lhs.begin(), lhs.end(), other.begin(), other.end()));
    TEST_ENSURE(mini::equalRange(lhs.begin(), lhs.at(3), other.begin(), other.at(3)));
    TEST_ENSURE(!mini::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.at(3)));
    TEST_ENSURE(!mini::equalRange(lhs.begin(), lhs.at(3), rhs.begin(), rhs.end()));
    TEST_ENSURE(mini::equalRange(lhs.begin(), lhs.begin(), rhs.begin(), rhs.begin()));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFillRange()
{
    Range<T, 4> data = makeRange<T>(1, 2, 3, 4);

    mini::fillRange(data.begin(), data.end(), makeValue<T>(7));
    for (T const& element : data.data) {
        TEST_ENSURE(element == makeValue<T>(7));
    }

    mini::fillRange(data.at(1), data.at(3), makeValue<T>(5));
    TEST_ENSURE(data.data[0] == makeValue<T>(7));
    TEST_ENSURE(data.data[1] == makeValue<T>(5));
    TEST_ENSURE(data.data[2] == makeValue<T>(5));
    TEST_ENSURE(data.data[3] == makeValue<T>(7));

    mini::fillRange(data.begin(), data.begin(), makeValue<T>(1));
    TEST_ENSURE(data.data[0] == makeValue<T>(7));

    return 0;
}

template <typename T>
[[no_inline]] constexpr mini::int32 testFind()
{
    Range<T, 4> data = makeRange<T>(1, 2, 3, 2);

    TEST_ENSURE(mini::find(data.begin(), data.end(), makeValue<T>(1)) == data.begin());
    TEST_ENSURE(mini::find(data.begin(), data.end(), makeValue<T>(2)) == data.at(1));
    TEST_ENSURE(mini::find(data.at(2), data.end(), makeValue<T>(2)) == data.at(3));
    TEST_ENSURE(mini::find(data.begin(), data.end(), makeValue<T>(5)) == data.end());
    TEST_ENSURE(mini::find(data.begin(), data.begin(), makeValue<T>(1)) == data.begin());

    return 0;
}

[[no_inline]] constexpr mini::int32 testDistance()
{
    Range<mini::int32, 4> data = makeRange<mini::int32>(1, 2, 3, 4);

    TEST_ENSURE(mini::distance(data.begin(), data.begin()) == 0);
    TEST_ENSURE(mini::distance(data.begin(), data.end()) == 4);
    TEST_ENSURE(mini::distance(data.at(1), data.at(3)) == 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testEqualRangeMixed()
{
    Range<mini::int32, 4> lhs = makeRange<mini::int32>(1, 2, 3, 4);
    Range<mini::int64, 4> rhs = makeRange<mini::int64>(1, 2, 3, 4);
    Range<mini::int64, 4> other = makeRange<mini::int64>(1, 2, 3, 0);

    TEST_ENSURE(mini::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
    TEST_ENSURE(mini::equalRange(rhs.begin(), rhs.end(), lhs.begin(), lhs.end()));
    TEST_ENSURE(!mini::equalRange(lhs.begin(), lhs.end(), other.begin(), other.end()));
    TEST_ENSURE(!mini::equalRange(lhs.begin(), lhs.at(3), rhs.begin(), rhs.end()));

    return 0;
}

[[no_inline]] constexpr mini::int32 testFillRangeConvert()
{
    Range<mini::int64, 4> data = makeRange<mini::int64>(1, 2, 3, 4);

    mini::fillRange(data.begin(), data.at(3), mini::int16(-3));
    TEST_ENSURE(data.data[0] == -3);
    TEST_ENSURE(data.data[1] == -3);
    TEST_ENSURE(data.data[2] == -3);
    TEST_ENSURE(data.data[3] == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIf()
{
    Range<mini::int32, 4> data = makeRange<mini::int32>(1, 4, 2, 5);

    TEST_ENSURE(mini::findIf(data.begin(), data.end(), IntPredicate{}) == data.at(1));
    TEST_ENSURE(mini::findIf(data.at(2), data.end(), IntPredicate{}) == data.at(3));
    TEST_ENSURE(mini::findIf(data.begin(), data.end(), [](mini::int32 value) { return value > 10; }) == data.end());
    TEST_ENSURE(mini::findIf(data.begin(), data.begin(), IntPredicate{}) == data.begin());

    TEST_ENSURE(mini::findIfNot(data.at(1), data.end(), IntPredicate{}) == data.at(2));
    TEST_ENSURE(mini::findIfNot(data.at(3), data.end(), IntPredicate{}) == data.end());
    TEST_ENSURE(mini::findIfNot(data.begin(), data.end(), [](mini::int32 value) { return value < 10; }) == data.end());
    TEST_ENSURE(mini::findIfNot(data.begin(), data.begin(), IntPredicate{}) == data.begin());

    return 0;
}

[[no_inline]] constexpr mini::int32 testFindIfCalls()
{
    Range<mini::int32, 4> data = makeRange<mini::int32>(4, 5, 1, 6);

    mini::int32 calls = 0;
    auto counting = [&calls](mini::int32 value) {
        ++calls;
        return value == 1;
    };
    TEST_ENSURE(mini::findIf(data.begin(), data.end(), counting) == data.at(2));
    TEST_ENSURE(calls == 3);

    calls = 0;
    auto notCounting = [&calls](mini::int32 value) {
        ++calls;
        return value > 3;
    };
    TEST_ENSURE(mini::findIfNot(data.begin(), data.end(), notCounting) == data.at(2));
    TEST_ENSURE(calls == 3);

    TEST_ENSURE(mini::find(data.begin(), data.end(), mini::int64(6)) == data.at(3));
    TEST_ENSURE(mini::find(data.begin(), data.end(), mini::int64(1) << 32) == data.end());

    return 0;
}

int main()
{
    TEST_FUNCTION_TEMPLATE(testCopyRange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCopyRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testCopyRange, View);

    TEST_FUNCTION_TEMPLATE(testMoveRange, mini::int32);
    TEST_FUNCTION_TEMPLATE(testMoveRange, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveRange, MoveOnlyObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveRange, View);

    TEST_FUNCTION_TEMPLATE(testCopyBackward, mini::int32);
    TEST_FUNCTION_TEMPLATE(testCopyBackward, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testCopyBackward, View);

    TEST_FUNCTION_TEMPLATE(testMoveBackward, mini::int32);
    TEST_FUNCTION_TEMPLATE(testMoveBackward, CopyableObject);
    TEST_FUNCTION_TEMPLATE(testMoveBackward, MoveOnlyObject);
    TEST_FUNCTION_TEMPLATE(testMoveBackward, View);

    TEST_CONSTEXPR_TEMPLATE(testEqualRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testEqualRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testEqualRange, View);

    TEST_CONSTEXPR_TEMPLATE(testFillRange, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFillRange, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testFillRange, View);

    TEST_CONSTEXPR_TEMPLATE(testFind, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testFind, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testFind, View);

    TEST_FUNCTION(testDistance);
    TEST_CONSTEXPR(testEqualRangeMixed);
    TEST_CONSTEXPR(testFillRangeConvert);
    TEST_CONSTEXPR(testFindIf);
    TEST_CONSTEXPR(testFindIfCalls);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
