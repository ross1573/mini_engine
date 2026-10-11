#include <memory>
#include <string>
#include <vector>

#include "test_macro.h"

import mini.test;

using mini::Array;
using mini::ArrayIterator;
using mini::InitializerList;

#define TEST_ARRAY(func, type, ...)                              \
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
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<mini::int32>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<mini::int32*>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<TestObject>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<TestObject*>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(Array<TestObject>);
    TEST_CONST_RANGE_BASED_FOR_SUPPORT(Array<TestObject>);

    using TestObjectIterator = Array<TestObject>::Iterator;
    using TestObjectArrayIterator = mini::ArrayIterator<TestObject, Array<TestObject>>;
    static_assert(mini::SameAsT<TestObjectIterator, TestObjectArrayIterator>);
    static_assert(sizeof(Array<TestObject>::Iterator) == alignof(void*) * 2);
}

template <typename T, typename AllocT, typename StdAllocT>
[[no_inline]] constexpr mini::int32 testArray(Array<T, AllocT> const& arr, std::vector<T, StdAllocT> const& vec)
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
[[no_inline]] constexpr mini::int32 testCtor()
{
    constexpr auto alloc = mini::Allocator<T>{};

    TEST_ENSURE(Array<T>{}.empty());
    TEST_ENSURE(Array<T>{}.capacity() == 0);
    TEST_ENSURE(Array<T>(alloc).empty());
    TEST_ENSURE(Array<T>(alloc).capacity() == 0);
    TEST_ENSURE(Array<T>(moveCast(alloc)).empty());
    TEST_ENSURE(Array<T>(moveCast(alloc)).capacity() == 0);
    TEST_ENSURE(Array<T>(16).empty());
    TEST_ENSURE(Array<T>(16).capacity() == 16);
    TEST_ENSURE(Array<T>(16, alloc).empty());
    TEST_ENSURE(Array<T>(16, alloc).capacity() == 16);
    TEST_ENSURE(Array<T>(16, moveCast(alloc)).empty());
    TEST_ENSURE(Array<T>(16, moveCast(alloc)).capacity() == 16);

    if constexpr (mini::CopyableT<T>) {
        Array<T> arr;
        mini::int32 count = 0;
        for (mini::int32 i = 0; i < 20; ++i) {
            arr.pushBack(FactoryT{}(++count));
        }

        TEST_ENSURE(Array<T>(arr) == arr);
        TEST_ENSURE(Array<T>(arr, alloc) == arr);
        TEST_ENSURE(Array<T>(Array<T>(arr)) == arr);
        TEST_ENSURE(Array<T>(Array<T>(arr), alloc) == arr);
        TEST_ENSURE(Array<T>(arr.begin(), arr.end()) == arr);
        TEST_ENSURE(Array<T>(arr.begin(), arr.end(), alloc) == arr);

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(testArray(Array<T>(list), std::vector<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
[[no_inline]] constexpr mini::int32 testPush()
{
    Array<T> arr;
    std::vector<T> vec;
    mini::int32 arrcount = 33;
    mini::int32 veccount = 33;

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
[[no_inline]] constexpr mini::int32 testInsert()
{
    Array<T> arr;
    std::vector<T> vec;
    mini::int32 arrcount = 33;
    mini::int32 veccount = 33;

    arr.insert(0, FactoryT{}(++arrcount));
    vec.insert(vec.begin(), FactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.insert(0, ArgFactoryT{}(++arrcount));
    vec.emplace(vec.begin(), ArgFactoryT{}(++veccount));
    TEST_ENSURE(testArray(arr, vec) == 0);

    if constexpr (mini::CopyableT<T>) {
        arr.reserve(4);
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
        Array<T> arr2(arr); // without the copy, it's UB

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
[[no_inline]] constexpr mini::int32 testRemove()
{
    Array<T> arr(32);
    std::vector<T> vec;
    mini::int32 arrcount = 33;
    mini::int32 veccount = 33;

    vec.reserve(32);
    for (mini::int32 i = 0; i < 32; ++i) {
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
[[no_inline]] constexpr mini::int32 testModify()
{
    Array<T> arr(8);
    std::vector<T> vec;
    mini::int32 arrcount = 33;
    mini::int32 veccount = 33;

    vec.reserve(8);
    for (mini::int32 i = 0; i < 8; ++i) {
        arr.pushBack(FactoryT{}(++arrcount));
        vec.emplace_back(FactoryT{}(++veccount));
    }

    if constexpr (mini::CopyableT<T>) {
        Array<T> arr2;
        arr2.assign(arr.begin(), arr.end());
        TEST_ENSURE(arr == arr2);

        arr.resize(20, ArgFactoryT{}(++arrcount));
        vec.resize(20, ArgFactoryT{}(++veccount));
        TEST_ENSURE(testArray(arr, vec) == 0);
    }

    arr.reserve(24);
    vec.reserve(24);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(testArray(arr, vec) == 0);

    Array<T> arr3(mini::moveCast(arr));
    std::vector<T> vec2(mini::moveCast(vec));
    arr.swap(arr3);
    vec.swap(vec2);
    TEST_ENSURE(testArray(arr, vec) == 0);

    arr.clear();
    vec.clear();
    TEST_ENSURE(testArray(arr, vec) == 0);

    return 0;
}

mini::int32 main()
{
    TEST_ARRAY(testCtor, mini::int32*, IntPtrF);
    TEST_ARRAY(testCtor, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY(testCtor, mini::UniquePtr<ConstexprObject>, UniquePtrF);
    TEST_ARRAY(testCtor, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testCtor<TestObject, FooF>() == 0));

    TEST_ARRAY(testPush, mini::int32*, IntPtrF);
    TEST_ARRAY(testPush, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY(testPush, ConstexprObject, ConstexprFooF, FooArgF);
    TEST_ENSURE((testPush<TestObject, FooF, FooArgF>() == 0));

    TEST_ARRAY(testInsert, mini::int32*, IntPtrF);
    TEST_ARRAY(testInsert, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY(testInsert, ConstexprObject, ConstexprFooF, FooArgF);
    TEST_ENSURE((testInsert<TestObject, FooF, FooArgF>() == 0));

    TEST_ARRAY(testRemove, mini::int32*, IntPtrF);
    TEST_ARRAY(testRemove, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY(testRemove, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testRemove<TestObject, FooF>() == 0));

    TEST_ARRAY(testModify, mini::int32*, IntPtrF);
    TEST_ARRAY(testModify, std::unique_ptr<ConstexprObject>, StdUniquePtrF);
    TEST_ARRAY(testModify, ConstexprObject, ConstexprFooF);
    TEST_ENSURE((testModify<TestObject, FooF>() == 0));
}