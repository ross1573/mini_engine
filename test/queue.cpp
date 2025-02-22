#include "mini_engine.h"
#include "test/container.h"

#include <queue>
#include <numeric>

[[maybe_unused]] static void QueueConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(StaticQueue<Foo, 1>);

    static_assert(sizeof(StaticQueue<Foo, 1>::Iterator) == alignof(void*) * 2 + sizeof(SizeT) * 2);
}

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

void QueueTest()
{
    static_assert(AccumulateQueue<StaticQueue<int, 3>>({1, 2, 3}) == 6);
    static_assert(SizeOfQueue<StaticQueue<Vector3, 5>>(5) == 5);

    StaticQueue<Foo, 8> q;
    {
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
        q.Assign(q.Begin(), q.End());

        memory::AddressOf(q.End());
        iter = q.Begin();
        ASSERT(!q.IsEmpty());
        ASSERT(!q.IsFull());
        ASSERT(iter.Increment());
        ASSERT(iter.Decrement());
        ASSERT(iter.Finish());
        ASSERT(!q.IsValidIterator(iter));
        ASSERT(!q3.IsValidIterator(--iter));
        ASSERT(q.IsValidIterator(iter));
        ASSERT(!q.IsValidIndex(q.Size()));
        ASSERT(q.IsValidIndex(q.Size() - 1));
        ASSERT(iter.Finish());
        ASSERT(iter.Increment() == false);
        ASSERT(iter.Decrement());
        ASSERT(iter.Advance(-5));
        ASSERT(iter.Advance(5));
        iter -= 32; iter += 32;
        ASSERT(++iter == q.End());

        static_assert(q.Capacity() == 8);
    }

    Array<Foo> arr(8);
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

    ASSERT(q.Size() == arr.Size());
    for (SizeT i = 0; i < q.Size(); ++i)
    {
        ASSERT(q[i] == arr[i]);
        ASSERT(q.At(i) == arr.At(i));
    }
}