#include "mini_engine.h"
#include "test/container.h"
#include "container/iterator/ptr_iterator.h"

#include <vector>
#include <numeric>

[[maybe_unused]] static constexpr void ArrayConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<int>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<int*>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<Foo>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(Array<Foo*>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<Foo, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticArray<Foo*, 1>::Iterator);

    static_assert(SameAsT<ArrayIterator<Foo, Array<Foo>>, Array<Foo>::Iterator>);
    static_assert(sizeof(Array<Foo>::Iterator) == alignof(void*) * 3);
    static_assert(sizeof(StaticArray<Foo, 1>::Iterator) == alignof(void*) * 2);
}

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

static Array<Foo> TestArray()
{
    Array<Foo> arr;

    static_assert(AccumulateArray<Array<int>>({1, 2, 3}) == 6);
    static_assert(SizeOfArray<Array<ConstExprFoo>>(5, "string") == 5);

    arr.Push("hello world! 0");
    arr.Insert(arr.Begin(), "hello world! 1");
    arr.Insert(0, "hello world! 2");
    arr.Insert(arr.End(), "hello world! 3");
    arr.Insert(2, "hello world! 4");
    arr.Insert(arr.Begin() + 3, "hello world! 5");
    arr.Insert(Array<Foo>::ConstIterator(arr.Begin()), "hello world! 6");

    Array<Foo> arr3 = arr;
    Array<Foo> arr4 = MoveArg(arr3);
    Array<Foo>::Iterator iter = arr4.Begin();
    Array<Foo>::ConstIterator iter2 = arr.End();

    arr.Assign(arr4.Begin(), arr4.End());
    arr.InsertRange(2, arr4.Begin(), arr4.End());
    arr.InsertRange(4, arr4.Begin() + 1, arr4.Begin() + 3);
    arr.AddRange(arr4.Begin(), arr4.End());
    arr.AddRange(arr4.Begin(), arr4.Begin() + 2);
    arr.RemoveRange(arr.End() - 4, arr.End());
    arr.RemoveLast();
    arr.RemoveAt(0);
    arr.RemoveAt(arr.Begin());
    arr.Reserve(arr.Size() + 5);
    arr.Resize(arr.Size() + 10);
    arr.RemoveLast(10);
    arr.Shrink();

    ASSERT(!arr.IsEmpty());
    ASSERT(iter.Increment());
    ASSERT(iter.Decrement());
    ASSERT(iter2.Finish());
    ASSERT(!arr.IsValidIterator(iter2));
    ASSERT(!arr.IsValidIterator(iter));
    ASSERT(arr.IsValidIterator(--iter2));
    ASSERT(!arr.IsValidIndex(arr.Size()));
    ASSERT(arr.IsValidIndex(arr.Size() - 1));
    ASSERT(iter.Finish());
    ASSERT(iter.Increment() == false);
    ASSERT(iter.Decrement());
    ASSERT(iter.Advance(-3));
    ASSERT(iter.Advance(2));

    memory::AddressOf(arr.End());
    ++iter;
    --iter2;
    arr3 = arr4;
    arr4 = MoveArg(arr3);
    arr3 = arr4;

    Array<Foo> other;
    other.Resize(arr.Size());
    CopyRange(other.Begin(), arr.Begin(), arr.End());

    return arr;
}

static StaticArray<Foo, 32> TestStaticArray()
{
    StaticArray<Foo, 32> arr;

    static_assert(arr.Capacity() == 32);
    static_assert(AccumulateArray<StaticArray<int, 3>>({1, 2, 3}) == 6);
    static_assert(SizeOfArray<StaticArray<Vector3, 5>>(5) == 5);

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

    ASSERT(!arr.IsEmpty());
    ASSERT(!arr.IsFull());
    ASSERT(iter.Increment());
    ASSERT(iter.Decrement());
    ASSERT(iter2.Finish());
    ASSERT(!arr.IsValidIterator(iter2));
    ASSERT(!arr.IsValidIterator(iter));
    ASSERT(arr.IsValidIterator(--iter2));
    ASSERT(!arr.IsValidIndex(arr.Size()));
    ASSERT(arr.IsValidIndex(arr.Size() - 1));
    ASSERT(iter.Finish());
    ASSERT(iter.Increment() == false);
    ASSERT(iter.Decrement());
    ASSERT(iter.Advance(-3));
    ASSERT(iter.Advance(2));

    ++iter;
    --iter2;

    arr3 = arr4;
    arr4 = MoveArg(arr3);
    arr3 = arr4;

    return arr;
}

void ArrayTest()
{
    Array<int*> ptrarr;
    {
        int a = 5;
        float b = 5.5f;
        ptrarr.Push(&a);

        PtrIterator<int*> piter(ptrarr[0]);
        PtrIterator<int const*> piter2(piter);
        PtrIterator<int const*> piter3 = piter;
        [[maybe_unused]] PtrIterator<float*> piter4(&b);

        ASSERT(piter - piter3 == 0);
        ASSERT(piter2 == piter);
        ASSERT(piter3 == piter2);
        ASSERT(*piter == piter2[0]);
    }

    Array<std::unique_ptr<Foo>> marr;
    {
        marr.Push(new Foo("hello world! 0"));
        marr.InsertRange(0, MoveIterator(marr.Begin()), MoveIterator(marr.End()));
        marr.AddRange(MoveIterator(marr.Begin()), MoveIterator(marr.End()));

        Array<std::unique_ptr<Foo>> arr4 = MoveArg(marr);
        Array<std::unique_ptr<Foo>>::Iterator iter = arr4.Begin();
        Array<std::unique_ptr<Foo>>::ConstIterator iter2 = marr.End();

        marr = MoveArg(arr4);
        ++iter;
        --iter2;
    }

    std::vector<Foo> vec;
    InitializeCounter();
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
    PrintCounter("std::vector");

    InitializeCounter();
    Array<Foo> arr = TestArray();
    PrintCounter("Array");

    InitializeCounter();
    StaticArray<Foo, 32> sarr = TestStaticArray();
    PrintCounter("StaticArray");

    ASSERT(arr.Size() == vec.size());
    ASSERT(arr.First() == vec.front());
    ASSERT(arr.Last() == vec.back());
    ASSERT(arr.Capacity() == vec.capacity());

    ASSERT(sarr.Size() == vec.size());
    ASSERT(sarr.First() == vec.front());
    ASSERT(sarr.Last() == vec.back());

    for (SizeT i = 0; i < arr.Size(); ++i)
    {
        ASSERT(arr[i] == vec[i]);
        ASSERT(arr.At(i) == vec.at(i));

        ASSERT(sarr[i] == vec[i]);
        ASSERT(sarr.At(i) == vec.at(i));
    }

    PrintLine();
}