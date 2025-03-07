#include <queue>
#include <numeric>
#include "test_macro.h"

import mini.test;

using namespace mini;

template <typename T>
static constexpr int AccumulateQueue(std::initializer_list<int> l)
{
    T queue{};
    for (auto ele : l)
    {
        queue.Enqueue(ele);
    }
    return std::accumulate(queue.Begin(), queue.End(), 0);
}

template <typename T, typename... Args>
static constexpr SizeT SizeOfQueue(SizeT count, Args&&... args)
{
    T queue{};
    for (SizeT i = 0; i < count; ++i)
    {
        queue.Enqueue(ForwardArg<Args>(args)...);
    }
    return queue.Size();
}

[[maybe_unused]] static constexpr void QueueConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(StaticQueue<Foo, 1>);

    static_assert(sizeof(StaticQueue<Foo, 1>::Iterator) == alignof(void*) * 2 + sizeof(SizeT) * 2);
}

[[maybe_unused]] static constexpr void ConstexprQueue()
{
    static_assert(AccumulateQueue<StaticQueue<int, 3>>({1, 2, 3}) == 6);
    static_assert(SizeOfQueue<StaticQueue<Vector3, 5>>(5) == 5);
}

int main()
{
    StaticQueue<Foo, 8> q;
    {
        static_assert(q.Capacity() == 8);

        q.Enqueue(Format("hello world! {}", 0));
        q.Enqueue(Format("hello world! {}", 1));
        q.Enqueue(Format("hello world! {}", 2));

        decltype(q) q2 = q;
        decltype(q) q3 = MoveArg(q2);
        decltype(q)::Iterator iter = q3.Begin();
        decltype(q)::Iterator end = q3.End();

        q.EnqueueRange(iter, end);
        q.RemoveFirst(3);
        q.EnqueueRange(iter, end);
        q.RemoveFirst(2);
        q.EnqueueRange(iter, end);
        q.RemoveFirst();
        //q.Assign(q.Begin(), q.End());

        memory::AddressOf(q.End());
        iter = q.Begin();
        TEST_ENSURE(!q.IsEmpty());
        TEST_ENSURE(!q.IsFull());
        TEST_ENSURE(iter.Increment());
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter.Finish());
        TEST_ENSURE(!q.IsValidIterator(iter));
        TEST_ENSURE(!q3.IsValidIterator(--iter));
        TEST_ENSURE(q.IsValidIterator(iter));
        TEST_ENSURE(!q.IsValidIndex(q.Size()));
        TEST_ENSURE(q.IsValidIndex(q.Size() - 1));
        TEST_ENSURE(iter.Finish());
        TEST_ENSURE(iter.Increment() == false);
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter.Advance(-5));
        TEST_ENSURE(iter.Advance(5));
        iter -= 32; iter += 32;
        TEST_ENSURE(++iter == q.End());
    }

    Array<Foo> arr(8);
    {
        arr.Push(Format("hello world! {}", 0));
        arr.Push(Format("hello world! {}", 1));
        arr.Push(Format("hello world! {}", 2));

        Array<Foo> arr2(arr);
        arr.AddRange(arr.Begin(), arr.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 3);
        arr.AddRange(arr2.Begin(), arr2.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 2);
        arr.AddRange(arr2.Begin(), arr2.End());
        arr.RemoveAt(arr.Begin());
        arr.Assign(arr.Begin(), arr.End());
    }

    TEST_ENSURE(q.Size() == arr.Size());
    for (SizeT i = 0; i < q.Size(); ++i)
    {
        TEST_ENSURE(q[i] == arr[i]);
        TEST_ENSURE(q.At(i) == arr.At(i));
    }

    return 0;
}