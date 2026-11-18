#include "test_macro.h"

import mini.test;

template <typename T>
[[no_inline]] constexpr mini::int32 testTrivialEqualRangeSized()
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
[[no_inline]] constexpr mini::int32 testTrivialEqualRangeBounded()
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
[[no_inline]] constexpr mini::int32 testTrivialEqualRangeOrder()
{
    T small[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T large[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(5),
    };
    T first[] = {
        makeObject<T>(0),
        makeObject<T>(2),
        makeObject<T>(3),
    };
    T const* csmall = small;
    T const* clarge = large;
    T const* cfirst = first;

    TEST_ENSURE(!mini::memory::equalRange(csmall, clarge, clarge + 3));
    TEST_ENSURE(!mini::memory::equalRange(clarge, csmall, csmall + 3));
    TEST_ENSURE(!mini::memory::equalRange(csmall, cfirst, cfirst + 3));
    TEST_ENSURE(!mini::memory::equalRange(cfirst, csmall, csmall + 3));
    TEST_ENSURE(mini::memory::equalRange(csmall + 1, cfirst + 1, cfirst + 3));

    TEST_ENSURE(!mini::memory::equalRange(csmall, csmall + 3, clarge, clarge + 3));
    TEST_ENSURE(!mini::memory::equalRange(clarge, clarge + 3, csmall, csmall + 3));
    TEST_ENSURE(!mini::memory::equalRange(cfirst, cfirst + 3, csmall, csmall + 3));
    TEST_ENSURE(mini::memory::equalRange(cfirst + 1, cfirst + 3, csmall + 1, csmall + 3));

    return 0;
}

int main()
{
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeSized, mini::int32);
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeSized, mini::int64);
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeSized, TrivialObject);

    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeBounded, mini::int32);
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeBounded, mini::int64);
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeBounded, TrivialObject);

    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeOrder, mini::int32);
    TEST_FUNCTION_TEMPLATE(testTrivialEqualRangeOrder, mini::int64);

    return 0;
}
