#include <memory>
#include <string>
#include <vector>

#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

#define TEST_ARRAY(func, type, ...)                              \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                                  \
    using name = decltype([](int c) {                                       \
        std::string str("Hello world! This is a long string 0.");           \
        str.replace(str.end() - 2, str.end() - 1, 1, static_cast<char>(c)); \
        return __VA_ARGS__(str);                                            \
    });

FACTORY(UniquePtrF, MakeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] static constexpr void StaticArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<TestObject, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<TestObject*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(StaticArray<TestObject, 1>);

    static_assert(sizeof(StaticArray<TestObject, 1>::Iterator) == alignof(void*) * 2);
}

template <typename T, SizeT CapN, typename StdAllocT>
static constexpr int TestArray(StaticArray<T, CapN> const& arr,
                               std::vector<T, StdAllocT> const& vec)
{
    constexpr auto TestElement = [](T const& l, T const& r) -> bool {
        if constexpr (memory::DereferencableT<T>) {
            if (l == nullptr || r == nullptr) return l == r;
            return *l == *r;
        }
        return l == r;
    };

    TEST_ENSURE(arr.Size() == vec.size());

    if (arr.Size() != 0) {
        TEST_ENSURE(TestElement(arr.First(), vec.front()));
        TEST_ENSURE(TestElement(arr.Last(), vec.back()));
    }

    for (SizeT i = 0; i < arr.Size(); ++i) {
        TEST_ENSURE(TestElement(arr[i], vec[i]));
        TEST_ENSURE(TestElement(arr.At(i), vec.at(i)));
    }

    return 0;
}

template <typename T, typename FactoryT>
static constexpr int TestCtor()
{
    TEST_ENSURE((StaticArray<T, 1>{}.Size() == 0));
    TEST_ENSURE((StaticArray<T, 1>{}.Capacity() == 1));
    TEST_ENSURE((StaticArray<T, 16>().Size() == 0));
    TEST_ENSURE((StaticArray<T, 16>().Capacity() == 16));

    if constexpr (CopyableT<T>) {
        StaticArray<T, 20> arr;
        int count = 0;
        for (int i = 0; i < 20; ++i) arr.Push(FactoryT{}(++count));

        TEST_ENSURE((StaticArray<T, 20>(arr) == arr));
        TEST_ENSURE((StaticArray<T, 20>(StaticArray<T, 20>(arr)) == arr));
        TEST_ENSURE((StaticArray<T, 20>(arr.Begin(), arr.End()) == arr));

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(TestArray(StaticArray<T, 20>(list), std::vector<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
static constexpr int TestPush()
{
    StaticArray<T, 8> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.Push(FactoryT{}(++arrcount));
    vec.push_back(FactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.Push(ArgFactoryT{}(++arrcount));
    vec.emplace_back(ArgFactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    if constexpr (CopyableT<T>) {
        arr.Append(arr.Begin(), arr.End());
        vec.insert(vec.end(), vec.begin(), vec.end());
        TEST_ENSURE(TestArray(arr, vec) == 0);

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        arr.Append(list);
        vec.append_range(list);
        TEST_ENSURE(TestArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
static constexpr int TestInsert()
{
    StaticArray<T, 16> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    arr.Insert(0, FactoryT{}(++arrcount));
    vec.insert(vec.begin(), FactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.Insert(0, ArgFactoryT{}(++arrcount));
    vec.emplace(vec.begin(), ArgFactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    if constexpr (CopyableT<T>) {
        vec.reserve(4);

        arr.Insert(0, arr.First());
        vec.insert(vec.begin(), vec.front());
        TEST_ENSURE(TestArray(arr, vec) == 0);

        arr.Insert(arr.Begin(), arr[1]);
        vec.insert(vec.begin(), vec[1]);
        TEST_ENSURE(TestArray(arr, vec) == 0);
    }

    arr.Insert(arr.Begin(), FactoryT{}(++arrcount));
    vec.insert(vec.begin(), FactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.Insert(arr.Begin(), ArgFactoryT{}(++arrcount));
    vec.emplace(vec.begin(), ArgFactoryT{}(++veccount));
    TEST_ENSURE(TestArray(arr, vec) == 0);

    if constexpr (CopyableT<T>) {
        StaticArray<T, 16> arr2(arr); // without the copy, it's UB

        arr.InsertRange(0, arr2.End() - 2, arr2.End());
        vec.insert(vec.begin(), arr2.End() - 2, arr2.End());
        TEST_ENSURE(TestArray(arr, vec) == 0);

        arr.InsertRange(1, arr2.Begin(), arr2.Begin() + 2);
        vec.insert(vec.begin() + 1, arr2.Begin(), arr2.Begin() + 2);
        TEST_ENSURE(TestArray(arr, vec) == 0);

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        arr.InsertRange(2, list);
        vec.insert(vec.begin() + 2, list);
        TEST_ENSURE(TestArray(arr, vec) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT>
static constexpr int TestRemove()
{
    StaticArray<T, 32> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(32);
    for (int i = 0; i < 32; ++i) {
        arr.Push(FactoryT{}(++arrcount));
        vec.push_back(FactoryT{}(++veccount));
    }

    arr.RemoveLast();
    vec.pop_back();
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.RemoveLast(5);
    vec.erase(vec.end() - 5, vec.end());
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.RemoveAt(0);
    vec.erase(vec.begin());
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.RemoveAt(arr.Begin() + 5);
    vec.erase(vec.begin() + 5);
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.RemoveRange(2, 5);
    vec.erase(vec.begin() + 2, vec.begin() + 5);
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.RemoveRange(arr.Begin() + 2, arr.Begin() + 5);
    vec.erase(vec.begin() + 2, vec.begin() + 5);
    TEST_ENSURE(TestArray(arr, vec) == 0);

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
static constexpr int TestModify()
{
    StaticArray<T, 24> arr;
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(8);
    for (int i = 0; i < 8; ++i) {
        arr.Push(FactoryT{}(++arrcount));
        vec.emplace_back(FactoryT{}(++veccount));
    }

    if constexpr (CopyableT<T>) {
        StaticArray<T, 8> arr2;
        arr2.Assign(arr.Begin(), arr.End());
        TEST_ENSURE(arr == arr2);

        arr.Resize(20, ArgFactoryT{}(++arrcount));
        vec.resize(20, ArgFactoryT{}(++veccount));
        TEST_ENSURE(TestArray(arr, vec) == 0);
    }

    arr.Clear();
    vec.clear();
    TEST_ENSURE(TestArray(arr, vec) == 0);

    return 0;
}

int main()
{
    static const int dummy = 0;
    using IntPtrF = decltype([](int) { return const_cast<int*>(&dummy); });
    using IntF = decltype([](int c) { return c; });
    using VecF = decltype([](int c) { return Vector2Int(c, c); });

    TEST_ENSURE((TestCtor<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestCtor<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((TestCtor<TestObject, FooF>() == 0));
    TEST_ARRAY(TestCtor, int, IntF);
    TEST_ARRAY(TestCtor, Vector2Int, VecF);

    TEST_ENSURE((TestPush<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestPush<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestPush<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestPush<ConstexprObject, ConstexprFooF, FooArgF>() == 0));
    TEST_ENSURE((TestPush<TestObject, FooF, FooArgF>() == 0));
    TEST_ARRAY(TestPush, int, IntF);
    TEST_ARRAY(TestPush, Vector2Int, VecF);

    TEST_ENSURE((TestInsert<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestInsert<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestInsert<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestInsert<ConstexprObject, ConstexprFooF, FooArgF>() == 0));
    TEST_ENSURE((TestInsert<TestObject, FooF, FooArgF>() == 0));
    TEST_ARRAY(TestInsert, int, IntF);
    TEST_ARRAY(TestInsert, Vector2Int, VecF);

    TEST_ENSURE((TestRemove<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestRemove<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestRemove<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestRemove<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((TestRemove<TestObject, FooF>() == 0));
    TEST_ARRAY(TestRemove, int, IntF);
    TEST_ARRAY(TestRemove, Vector2Int, VecF);

    TEST_ENSURE((TestModify<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestModify<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestModify<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestModify<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((TestModify<TestObject, FooF>() == 0));
    TEST_ARRAY(TestModify, int, IntF);
    TEST_ARRAY(TestModify, Vector2Int, VecF);
}