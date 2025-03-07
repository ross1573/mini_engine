#include <vector>
#include <numeric>
#include <memory>
#include "test_macro.h"

import mini.test;

using namespace mini;

template <typename T>
static constexpr int AccumulateArray(std::initializer_list<int> l)
{
    T arr{};
    for (auto ele : l)
    {
        arr.Push(ele);
    }
    return std::accumulate(arr.Begin(), arr.End(), 0);
}

template <typename T, typename... Args>
static constexpr SizeT SizeOfArray(SizeT count, Args&&... args)
{
    T arr{};
    for (SizeT i = 0; i < count; ++i)
    {
        arr.Push(ForwardArg<Args>(args)...);
    }
    return arr.Size();
}

[[maybe_unused]] static constexpr void StaticArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<Foo, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<Foo*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(StaticArray<Foo, 1>);

    static_assert(sizeof(StaticArray<Foo, 1>::Iterator) == alignof(void*) * 2);
}

[[maybe_unused]] static constexpr void ConstexprStaticArray()
{
    static_assert(AccumulateArray<StaticArray<int, 3>>({1, 2, 3}) == 6);
    static_assert(SizeOfArray<StaticArray<Vector3, 5>>(5) == 5);
}

int main()
{
    StaticArray<Foo, 32> arr;
    // InitializeCounter();
    {
        static_assert(arr.Capacity() == 32);

        arr.Push("hello world! 0");
        arr.Insert(arr.Begin(), "hello world! 1");
        arr.Insert(0, "hello world! 2");
        arr.Insert(arr.End(), "hello world! 3");
        arr.Insert(2, "hello world! 4");
        arr.Insert(arr.Begin() + 3, "hello world! 5");
        arr.Insert(ArrayIterator<Foo const, decltype(arr)>
                   (arr.Begin()), "hello world! 6");
    
        decltype(arr) arr3 = arr;
        decltype(arr) arr4 = MoveArg(arr3);
        decltype(arr)::Iterator iter = arr4.Begin();
        decltype(arr)::ConstIterator iter2 = arr.End();
    
        arr.Assign(arr4.Begin(), arr4.End());
        arr.InsertRange(2, arr4.Begin(), arr4.End());
        arr.InsertRange(4, arr4.Begin() + 1, arr4.Begin() + 3);
        arr.AddRange(arr4.Begin(), arr4.End());
        arr.AddRange(arr4.Begin(), arr4.Begin() + 2);
        arr.RemoveRange(arr.End() - 4, arr.End());
        arr.RemoveLast();
        arr.RemoveAt(0);
        arr.RemoveAt(arr.Begin());
        arr.Resize(arr.Size() + 10);
        arr.RemoveLast(10);
    
        TEST_ENSURE(!arr.IsEmpty());
        TEST_ENSURE(!arr.IsFull());
        TEST_ENSURE(iter.Increment());
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter2.Finish());
        TEST_ENSURE(!arr.IsValidIterator(iter2));
        TEST_ENSURE(!arr.IsValidIterator(iter));
        TEST_ENSURE(arr.IsValidIterator(--iter2));
        TEST_ENSURE(!arr.IsValidIndex(arr.Size()));
        TEST_ENSURE(arr.IsValidIndex(arr.Size() - 1));
        TEST_ENSURE(iter.Finish());
        TEST_ENSURE(iter.Increment() == false);
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter.Advance(-3));
        TEST_ENSURE(iter.Advance(2));
    
        ++iter;
        --iter2;
    
        arr3 = arr4;
        arr4 = MoveArg(arr3);
        arr3 = arr4;
    }
    // PrintCounter("StaticArray");

    std::vector<Foo> vec;
    // InitializeCounter();
    {
        vec.emplace_back("hello world! 0");
        vec.emplace(vec.begin(), "hello world! 1");
        vec.emplace(vec.begin(), "hello world! 2");
        vec.emplace(vec.end(), "hello world! 3");
        vec.emplace(vec.begin() + 2, "hello world! 4");
        vec.emplace(vec.begin() + 3, "hello world! 5");
        vec.emplace(std::vector<Foo>::const_iterator(vec.begin()),
                    "hello world! 6");

        auto tmp = std::vector<Foo>(vec);
        vec.assign(tmp.begin(), tmp.end());
        vec.insert(vec.begin() + 2, tmp.begin(), tmp.end());
        vec.insert(vec.begin() + 4, tmp.begin() + 1, tmp.begin() + 3);
        vec.insert(vec.end(), tmp.begin(), tmp.end());
        vec.insert(vec.end(), tmp.begin(), tmp.begin() + 2);
        vec.erase(vec.end() - 4, vec.end());
        vec.pop_back();
        vec.erase(vec.begin());
        vec.erase(vec.begin());
        vec.reserve(vec.size() + 5);
        vec.resize(vec.size() + 10);
        vec.erase(vec.end() - 10, vec.end());
        vec.shrink_to_fit();
    }
    // PrintCounter("std::vector");

    TEST_ENSURE(arr.Size() == vec.size());
    TEST_ENSURE(arr.First() == vec.front());
    TEST_ENSURE(arr.Last() == vec.back());

    for (SizeT i = 0; i < arr.Size(); ++i)
    {
        TEST_ENSURE(arr[i] == vec[i]);
        TEST_ENSURE(arr.At(i) == vec.at(i));
    }

    return 0;
}