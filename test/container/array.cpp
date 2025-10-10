#include <memory>
#include <string>
#include <vector>

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

FACTORY(UniquePtrF, MakeUnique<ConstexprFoo>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprFoo>);
FACTORY(ConstexprFooF, ConstexprFoo);
FACTORY(FooF, Foo);
FACTORY(FooArgF);

[[maybe_unused]] static constexpr void ArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<int>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<int*>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<Foo>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<Foo*>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(Array<Foo>);

    static_assert(SameAsT<ArrayIterator<Foo, Array<Foo>>, Array<Foo>::Iterator>);
    static_assert(sizeof(Array<Foo>::Iterator) == alignof(void*) * 2);
}

template <typename T, typename AllocT, typename StdAllocT>
[[no_inline]] constexpr int TestArray(Array<T, AllocT> const& arr,
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
[[no_inline]] constexpr int TestCtor()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(Array<T>{}.Size() == 0);
    TEST_ENSURE(Array<T>{}.Capacity() == 0);
    TEST_ENSURE(Array<T>(alloc).Size() == 0);
    TEST_ENSURE(Array<T>(alloc).Capacity() == 0);
    TEST_ENSURE(Array<T>(MoveArg(alloc)).Size() == 0);
    TEST_ENSURE(Array<T>(MoveArg(alloc)).Capacity() == 0);
    TEST_ENSURE(Array<T>(16).Size() == 0);
    TEST_ENSURE(Array<T>(16).Capacity() == 16);
    TEST_ENSURE(Array<T>(16, alloc).Size() == 0);
    TEST_ENSURE(Array<T>(16, alloc).Capacity() == 16);
    TEST_ENSURE(Array<T>(16, MoveArg(alloc)).Size() == 0);
    TEST_ENSURE(Array<T>(16, MoveArg(alloc)).Capacity() == 16);

    if constexpr (CopyableT<T>) {
        Array<T> arr;
        int count = 0;
        for (int i = 0; i < 20; ++i) arr.Push(FactoryT{}(++count));

        TEST_ENSURE(Array<T>(arr) == arr);
        TEST_ENSURE(Array<T>(arr, alloc) == arr);
        TEST_ENSURE(Array<T>(Array<T>(arr)) == arr);
        TEST_ENSURE(Array<T>(Array<T>(arr), alloc) == arr);
        TEST_ENSURE(Array<T>(arr.Begin(), arr.End()) == arr);
        TEST_ENSURE(Array<T>(arr.Begin(), arr.End(), alloc) == arr);

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(TestArray(Array<T>(list), std::vector<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
[[no_inline]] constexpr int TestPush()
{
    Array<T> arr;
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
[[no_inline]] constexpr int TestInsert()
{
    Array<T> arr;
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
        arr.Reserve(4);
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
        Array<T> arr2(arr); // without the copy, it's UB

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
[[no_inline]] constexpr int TestRemove()
{
    Array<T> arr(32);
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
[[no_inline]] constexpr int TestModify()
{
    Array<T> arr(8);
    std::vector<T> vec;
    int arrcount = 33;
    int veccount = 33;

    vec.reserve(8);
    for (int i = 0; i < 8; ++i) {
        arr.Push(FactoryT{}(++arrcount));
        vec.emplace_back(FactoryT{}(++veccount));
    }

    if constexpr (CopyableT<T>) {
        Array<T> arr2;
        arr2.Assign(arr.Begin(), arr.End());
        TEST_ENSURE(arr == arr2);

        arr.Resize(20, ArgFactoryT{}(++arrcount));
        vec.resize(20, ArgFactoryT{}(++veccount));
        TEST_ENSURE(TestArray(arr, vec) == 0);
    }

    arr.Reserve(24);
    vec.reserve(24);
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.Shrink();
    vec.shrink_to_fit();
    TEST_ENSURE(TestArray(arr, vec) == 0);

    Array<T> arr3(MoveArg(arr));
    std::vector<T> vec2(MoveArg(vec));
    arr.Swap(arr3);
    vec.swap(vec2);
    TEST_ENSURE(TestArray(arr, vec) == 0);

    arr.Clear();
    vec.clear();
    TEST_ENSURE(TestArray(arr, vec) == 0);

    return 0;
}

int main()
{
    static const int dummy = 0;
    using IntPtrF = decltype([](int) { return const_cast<int*>(&dummy); });

    TEST_ARRAY(TestCtor, int*, IntPtrF);
    TEST_ARRAY(TestCtor, std::unique_ptr<ConstexprFoo>, StdUniquePtrF);
    TEST_ARRAY(TestCtor, UniquePtr<ConstexprFoo>, UniquePtrF);
    TEST_ARRAY(TestCtor, ConstexprFoo, ConstexprFooF);
    TEST_ENSURE((TestCtor<Foo, FooF>() == 0));

    TEST_ARRAY(TestPush, int*, IntPtrF);
    TEST_ARRAY(TestPush, std::unique_ptr<ConstexprFoo>, StdUniquePtrF);
    TEST_ARRAY(TestPush, UniquePtr<ConstexprFoo>, UniquePtrF);
    TEST_ARRAY(TestPush, ConstexprFoo, ConstexprFooF, FooArgF);
    TEST_ENSURE((TestPush<Foo, FooF, FooArgF>() == 0));

    TEST_ARRAY(TestInsert, int*, IntPtrF);
    TEST_ARRAY(TestInsert, std::unique_ptr<ConstexprFoo>, StdUniquePtrF);
    TEST_ARRAY(TestInsert, UniquePtr<ConstexprFoo>, UniquePtrF);
    TEST_ARRAY(TestInsert, ConstexprFoo, ConstexprFooF, FooArgF);
    TEST_ENSURE((TestInsert<Foo, FooF, FooArgF>() == 0));

    TEST_ARRAY(TestRemove, int*, IntPtrF);
    TEST_ARRAY(TestRemove, std::unique_ptr<ConstexprFoo>, StdUniquePtrF);
    TEST_ARRAY(TestRemove, UniquePtr<ConstexprFoo>, UniquePtrF);
    TEST_ARRAY(TestRemove, ConstexprFoo, ConstexprFooF);
    TEST_ENSURE((TestRemove<Foo, FooF>() == 0));

    TEST_ARRAY(TestModify, int*, IntPtrF);
    TEST_ARRAY(TestModify, std::unique_ptr<ConstexprFoo>, StdUniquePtrF);
    TEST_ARRAY(TestModify, UniquePtr<ConstexprFoo>, UniquePtrF);
    TEST_ARRAY(TestModify, ConstexprFoo, ConstexprFooF);
    TEST_ENSURE((TestModify<Foo, FooF>() == 0));
}