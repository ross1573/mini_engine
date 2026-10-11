#include <memory>
#include <string>
#include <vector>

#include "test_macro.h"

import mini.test;

using mini::FixedArray;
using mini::InitializerList;

#define TEST_ARRAY(func, type, ...)                              \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                         \
    using name = decltype([](int count) {                          \
        mini::String str("Hello world! This is a long string 0."); \
        *(str.end() - 2) = static_cast<char>(count);               \
        return __VA_ARGS__(str);                                   \
    });

FACTORY(UniquePtrF, mini::makeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] constexpr void fixedArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedArray<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedArray<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedArray<TestObject, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedArray<TestObject*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(FixedArray<TestObject, 1>);
    TEST_CONST_RANGE_BASED_FOR_SUPPORT(FixedArray<TestObject, 1>);

    static_assert(sizeof(FixedArray<TestObject, 1>::Iterator) == alignof(void*) * 2);
}

template <typename T, size_t CapN, typename StdAllocT>
constexpr int testArray(FixedArray<T, CapN> const& arr, std::vector<T, StdAllocT> const& vec)
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

    TEST_ENSURE(arr.size() == vec.size());

    if (!arr.empty()) {
        TEST_ENSURE(testElement(arr.first(), vec.front()));
        TEST_ENSURE(testElement(arr.last(), vec.back()));
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        TEST_ENSURE(testElement(arr[i], vec[i]));
        TEST_ENSURE(testElement(arr.at(i), vec.at(i)));
    }

    return 0;
}

template <typename T, typename FactoryT>
constexpr int testCtor()
{
    TEST_ENSURE((FixedArray<T, 1>{}.empty()));
    TEST_ENSURE((FixedArray<T, 1>{}.capacity() == 1));
    TEST_ENSURE((FixedArray<T, 16>().empty()));
    TEST_ENSURE((FixedArray<T, 16>().capacity() == 16));

    if constexpr (mini::CopyableT<T>) {
        FixedArray<T, 20> arr;
        int count = 0;
        for (int i = 0; i < 20; ++i) {
            arr.pushBack(FactoryT{}(++count));
        }

        TEST_ENSURE((FixedArray<T, 20>(arr) == arr));
        TEST_ENSURE((FixedArray<T, 20>(FixedArray<T, 20>(arr)) == arr));
        TEST_ENSURE((FixedArray<T, 20>(arr.begin(), arr.end()) == arr));

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(testArray(FixedArray<T, 20>(list), std::vector<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
constexpr int testPush()
{
    FixedArray<T, 8> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.pushBack(FactoryT{}(++arrcount));
    vec.push_back(FactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.pushBack(ArgFactoryT{}(++arrcount));
    vec.emplace_back(ArgFactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    if constexpr (mini::CopyableT<T>) {
        arr.append(arr.begin(), arr.end());
        vec.insert(vec.end(), vec.begin(), vec.end());
        TEST_ENSURE(testArray(arr, vec) == 0);

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        arr.append(list);
        vec.append_range(list);
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
constexpr int testInsert()
{
    FixedArray<T, 16> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.insert(0, FactoryT{}(++arrcount));
    vec.insert(vec.begin(), FactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(0, ArgFactoryT{}(++arrcount));
    vec.emplace(vec.begin(), ArgFactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    if constexpr (mini::CopyableT<T>) {
        vec.reserve(4);

        arr.insert(0, arr.first());
        vec.insert(vec.begin(), vec.front());
        TEST_ENSURE(testArray(arr, vec) == 0);

        arr.insert(arr.begin(), arr[1]);
        vec.insert(vec.begin(), vec[1]);
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    arr.insert(arr.begin(), FactoryT{}(++arrcount));
    vec.insert(vec.begin(), FactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(arr.begin(), ArgFactoryT{}(++arrcount));
    vec.emplace(vec.begin(), ArgFactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    if constexpr (mini::CopyableT<T>) {
        FixedArray<T, 16> arr2(arr); // without the copy, it's UB

        arr.insertRange(0, arr2.end() - 2, arr2.end());
        vec.insert(vec.begin(), arr2.end() - 2, arr2.end());
        TEST_ENSURE(testArray(arr, vec) == 0);

        arr.insertRange(1, arr2.begin(), arr2.begin() + 2);
        vec.insert(vec.begin() + 1, arr2.begin(), arr2.begin() + 2);
        TEST_ENSURE(testArray(arr, vec) == 0);

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        arr.insertRange(2, list);
        vec.insert(vec.begin() + 2, list);
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT>
constexpr int testRemove()
{
    FixedArray<T, 32> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(32);
    for (int i = 0; i < 32; ++i) {
        arr.pushBack(FactoryT{}(++arrcount));
        vec.push_back(FactoryT{}(++veccount));
    }

    arr.popBack();
    vec.pop_back();
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.popBack(5);
    vec.erase(vec.end() - 5, vec.end());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.remove(0);
    vec.erase(vec.begin());
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.remove(arr.begin() + 5);
    vec.erase(vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.removeRange(arr.begin() + 2, arr.begin() + 5);
    vec.erase(vec.begin() + 2, vec.begin() + 5);
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
constexpr int testModify()
{
    FixedArray<T, 24> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(8);
    for (int i = 0; i < 8; ++i) {
        arr.pushBack(FactoryT{}(++arrcount));
        vec.emplace_back(FactoryT{}(++veccount));
    }

    if constexpr (mini::CopyableT<T>) {
        FixedArray<T, 8> arr2;
        arr2.assign(arr.begin(), arr.end());
        TEST_ENSURE(arr == arr2);

        arr.resize(20, ArgFactoryT{}(++arrcount));
        vec.resize(20, ArgFactoryT{}(++veccount));
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    arr.clear();
    vec.clear();
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

int main()
{
    static const int dummy = 0;
    using IntPtrF = decltype([](int) { return const_cast<int*>(&dummy); });
    using IntF = decltype([](int count) { return count; });
    using TrivialF = decltype([](int count) { return TrivialObject{count, count}; });

    TEST_ENSURE((testCtor<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testCtor<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testCtor<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((testCtor<TestObject, FooF>() == 0));
    TEST_ARRAY(testCtor, mini::int32, IntF);
    TEST_ARRAY(testCtor, TrivialObject, TrivialF);

    TEST_ENSURE((testPush<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testPush<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testPush<ConstexprObject, ConstexprFooF, FooArgF>() == 0));
    TEST_ENSURE((testPush<TestObject, FooF, FooArgF>() == 0));
    TEST_ARRAY(testPush, mini::int32, IntF);
    TEST_ARRAY(testPush, TrivialObject, TrivialF);

    TEST_ENSURE((testInsert<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testInsert<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testInsert<ConstexprObject, ConstexprFooF, FooArgF>() == 0));
    TEST_ENSURE((testInsert<TestObject, FooF, FooArgF>() == 0));
    TEST_ARRAY(testInsert, mini::int32, IntF);
    TEST_ARRAY(testInsert, TrivialObject, TrivialF);

    TEST_ENSURE((testRemove<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testRemove<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testRemove<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((testRemove<TestObject, FooF>() == 0));
    TEST_ARRAY(testRemove, mini::int32, IntF);
    TEST_ARRAY(testRemove, TrivialObject, TrivialF);

    TEST_ENSURE((testModify<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testModify<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testModify<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((testModify<TestObject, FooF>() == 0));
    TEST_ARRAY(testModify, mini::int32, IntF);
    TEST_ARRAY(testModify, TrivialObject, TrivialF);
}