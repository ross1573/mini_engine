#include <memory>
#include <span>

#include "test_macro.h"

import mini.test;

using mini::Array;
using mini::ArrayIterator;
using mini::ArrayView;
using mini::InitializerList;

#define TEST_ARRAY_VIEW(func, type, ...)                         \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                         \
    using name = decltype([](mini::int32 count) {                  \
        mini::String str("Hello world! This is a long string 0."); \
        *(str.end() - 2) = static_cast<char>(count);               \
        return __VA_ARGS__(str);                                   \
    });

using IntPtrF = decltype([](mini::int32) {
    static const mini::int32 dummy = 0;
    return const_cast<mini::int32*>(&dummy);
});

FACTORY(UniquePtrF, makeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] constexpr void arrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<mini::int32>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<mini::int32*>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<TestObject>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<TestObject*>::ConstIterator);

    TEST_CONST_RANGE_BASED_FOR_SUPPORT(ArrayView<TestObject>);

    using TestObjectIterator = ArrayView<TestObject>::ConstIterator;
    using TestObjectArrayIterator = ArrayIterator<TestObject const, ArrayView<TestObject> const>;
    static_assert(sizeof(ArrayView<TestObject>) == 16);
    static_assert(sizeof(TestObjectIterator) == alignof(void*) * 2);
    static_assert(mini::SameAsT<TestObjectArrayIterator, TestObjectIterator>);
}

template <typename T>
[[no_inline]] constexpr mini::int32 testArrayView(ArrayView<T> arr, std::span<T> span)
{
    constexpr auto testElement = [](T const& lhs, T const& rhs) -> bool {
        if constexpr (mini::DereferencableT<T>) {
            if (lhs == nullptr || rhs == nullptr) {
                return lhs == rhs;
            }
            return *lhs == *rhs;
        }
        return lhs == rhs;
    };

    TEST_ENSURE(arr.size() == span.size());

    if (!arr.empty()) {
        TEST_ENSURE(testElement(arr.first(), span.front()));
        TEST_ENSURE(testElement(arr.last(), span.back()));
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        TEST_ENSURE(testElement(arr[i], span[i]));
        TEST_ENSURE(testElement(arr.at(i), span[i]));
    }

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr mini::int32 testCtor()
{
    mini::int32 count = 0;
    InitializerList list = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    T array[4] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    TEST_ENSURE(ArrayView<T>().empty());
    TEST_ENSURE(ArrayView<T>(array).size() == 4);
    TEST_ENSURE(ArrayView<T>(list).size() == list.size());
    TEST_ENSURE(ArrayView<T>(list.begin(), list.size()).size() == list.size());
    TEST_ENSURE(ArrayView<T>(Array<T>{}).empty());
    TEST_ENSURE(ArrayView<T>(ArrayView<T>{}).empty());

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr mini::int32 testSubArray()
{
    mini::int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view, span) == 0);

    ArrayView<T> frontView = view.subFront(3);
    std::span<T> frontSpan = span.subspan(0, 3);
    TEST_ENSURE(testArrayView(frontView, frontSpan) == 0);

    ArrayView<T> backView = view.subBack(3);
    std::span<T> backSpan = span.subspan(span.size() - 3, 3);
    TEST_ENSURE(testArrayView(backView, backSpan) == 0);

    ArrayView<T> subview = view.subView(1, 2);
    std::span<T> subSpan = span.subspan(1, 2);
    TEST_ENSURE(testArrayView(subview, subSpan) == 0);

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr mini::int32 testModify()
{
    mini::int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view, span) == 0);

    ArrayView<T> view2 = view;
    view2.popFront();
    TEST_ENSURE(testArrayView(view2, span.subspan(1, static_cast<size_t>(count) - 1)) == 0);

    ArrayView<T> view3 = view;
    view3.popFront(3);
    TEST_ENSURE(testArrayView(view3, span.subspan(3, static_cast<size_t>(count) - 3)) == 0);

    ArrayView<T> view4 = view;
    view4.popBack();
    TEST_ENSURE(testArrayView(view4, span.subspan(0, static_cast<size_t>(count) - 1)) == 0);

    ArrayView<T> view5 = view;
    view5.popBack(3);
    TEST_ENSURE(testArrayView(view5, span.subspan(0, static_cast<size_t>(count) - 3)) == 0);

    TEST_ENSURE(testArrayView(view, span) == 0);
    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr mini::int32 testOperator()
{
    mini::int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    mini::int32 initListCount = 0;
    InitializerList list = {
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
    };

    ArrayView<T> view = array;
    std::span<T> span{array};
    TEST_ENSURE(testArrayView(view, span) == 0);

    ArrayView<T> view2 = view;
    TEST_ENSURE(testArrayView(view2, span) == 0);

    view = list;
    TEST_ENSURE(testArrayView(view, span) == 0);

    view = array;
    TEST_ENSURE(testArrayView(view, span) == 0);

    return 0;
}

mini::int32 main()
{
    TEST_ARRAY_VIEW(testCtor, mini::int32*, IntPtrF);
    TEST_ARRAY_VIEW(testCtor, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(testCtor, mini::UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(testCtor, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testCtor<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(testSubArray, mini::int32*, IntPtrF);
    TEST_ARRAY_VIEW(testSubArray, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(testSubArray, mini::UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(testSubArray, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testSubArray<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(testModify, mini::int32*, IntPtrF);
    TEST_ARRAY_VIEW(testModify, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(testModify, mini::UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(testModify, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testModify<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(testOperator, mini::int32*, IntPtrF);
    TEST_ARRAY_VIEW(testOperator, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(testOperator, mini::UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(testOperator, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testOperator<TestObject, FooF>() == 0));

    return 0;
}