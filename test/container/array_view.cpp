#include <memory>
#include <span>

#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

#define TEST_ARRAY_VIEW(func, type, ...)                         \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                   \
    using name = decltype([](int32 count) {                  \
        String str("Hello world! This is a long string 0."); \
        *(str.End() - 2) = static_cast<char>(count);         \
        return __VA_ARGS__(str);                             \
    });

using IntPtrF = decltype([](int) {
    static const int dummy = 0;
    return const_cast<int*>(&dummy);
});

FACTORY(UniquePtrF, MakeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] constexpr void ArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<int>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<int*>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<TestObject>::ConstIterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(ArrayView<TestObject*>::ConstIterator);

    TEST_CONST_RANGE_BASED_FOR_SUPPORT(ArrayView<TestObject>);

    static_assert(sizeof(ArrayView<TestObject>::ConstIterator) == alignof(void*) * 2);
    static_assert(sizeof(ArrayView<TestObject>) == 16);
    static_assert(SameAsT<ArrayIterator<TestObject const, ArrayView<TestObject> const>,
                          ArrayView<TestObject>::ConstIterator>);
}

template <typename T>
[[no_inline]] constexpr int TestArrayView(ArrayView<T> arr, std::span<T> span)
{
    constexpr auto TestElement = [](T const& lhs, T const& rhs) -> bool {
        if constexpr (memory::DereferencableT<T>) {
            if (lhs == nullptr || rhs == nullptr) {
                return lhs == rhs;
            }
            return *lhs == *rhs;
        }
        return lhs == rhs;
    };

    TEST_ENSURE(arr.Size() == span.size());

    if (arr.Size() != 0) {
        TEST_ENSURE(TestElement(arr.First(), span.front()));
        TEST_ENSURE(TestElement(arr.Last(), span.back()));
    }

    for (size_t i = 0; i < arr.Size(); ++i) {
        TEST_ENSURE(TestElement(arr[i], span[i]));
        TEST_ENSURE(TestElement(arr.At(i), span[i]));
    }

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr int TestCtor()
{
    int32 count = 0;
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

    TEST_ENSURE(ArrayView<T>().Size() == 0);
    TEST_ENSURE(ArrayView<T>(array).Size() == 4);
    TEST_ENSURE(ArrayView<T>(list).Size() == list.size());
    TEST_ENSURE(ArrayView<T>(list.begin(), list.size()).Size() == list.size());
    TEST_ENSURE(ArrayView<T>(Array<T>{}).Size() == 0);
    TEST_ENSURE(ArrayView<T>(ArrayView<T>{}).Size() == 0);

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr int TestSubArray()
{
    int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(TestArrayView(view, span) == 0);

    ArrayView<T> frontView = view.SubFront(3);
    std::span<T> frontSpan = span.subspan(0, 3);
    TEST_ENSURE(TestArrayView(frontView, frontSpan) == 0);

    ArrayView<T> backView = view.SubBack(3);
    std::span<T> backSpan = span.subspan(span.size() - 3, 3);
    TEST_ENSURE(TestArrayView(backView, backSpan) == 0);

    ArrayView<T> subView = view.SubView(1, 2);
    std::span<T> subSpan = span.subspan(1, 2);
    TEST_ENSURE(TestArrayView(subView, subSpan) == 0);

    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr int TestModify()
{
    int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    ArrayView<T> view{array};
    std::span<T> span{array};
    TEST_ENSURE(TestArrayView(view, span) == 0);

    ArrayView<T> view2 = view;
    view2.PopFront();
    TEST_ENSURE(TestArrayView(view2, span.subspan(1, static_cast<size_t>(count) - 1)) == 0);

    ArrayView<T> view3 = view;
    view3.PopFront(3);
    TEST_ENSURE(TestArrayView(view3, span.subspan(3, static_cast<size_t>(count) - 3)) == 0);

    ArrayView<T> view4 = view;
    view4.PopBack();
    TEST_ENSURE(TestArrayView(view4, span.subspan(0, static_cast<size_t>(count) - 1)) == 0);

    ArrayView<T> view5 = view;
    view5.PopBack(3);
    TEST_ENSURE(TestArrayView(view5, span.subspan(0, static_cast<size_t>(count) - 3)) == 0);

    TEST_ENSURE(TestArrayView(view, span) == 0);
    return 0;
}

template <typename T, typename FactoryT>
[[no_inline]] constexpr int TestOperator()
{
    int32 count = 0;
    T array[] = {
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
        FactoryT{}(++count),
    };

    int32 initListCount = 0;
    InitializerList list = {
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
        FactoryT{}(++initListCount),
    };

    ArrayView<T> view = array;
    std::span<T> span{array};
    TEST_ENSURE(TestArrayView(view, span) == 0);

    ArrayView<T> view2 = view;
    TEST_ENSURE(TestArrayView(view2, span) == 0);

    view = list;
    TEST_ENSURE(TestArrayView(view, span) == 0);

    view = array;
    TEST_ENSURE(TestArrayView(view, span) == 0);

    return 0;
}

int main()
{
    TEST_ARRAY_VIEW(TestCtor, int*, IntPtrF);
    TEST_ARRAY_VIEW(TestCtor, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(TestCtor, UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(TestCtor, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((TestCtor<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(TestSubArray, int*, IntPtrF);
    TEST_ARRAY_VIEW(TestSubArray, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(TestSubArray, UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(TestSubArray, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((TestSubArray<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(TestModify, int*, IntPtrF);
    TEST_ARRAY_VIEW(TestModify, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(TestModify, UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(TestModify, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((TestModify<TestObject, FooF>() == 0));

    TEST_ARRAY_VIEW(TestOperator, int*, IntPtrF);
    TEST_ARRAY_VIEW(TestOperator, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY_VIEW(TestOperator, UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY_VIEW(TestOperator, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((TestOperator<TestObject, FooF>() == 0));

    return 0;
}