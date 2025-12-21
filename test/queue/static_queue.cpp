#include <memory>
#include <string>

#include "test_macro.h"

import mini.test;
import mini.utility;
import mini.memory;
import mini.math;
import mini.algorithm;
import mini.array;
import mini.string;
import mini.queue;

using namespace mini;
using namespace mini::test;

#define TEST_QUEUE(func, type, ...)                              \
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

[[maybe_unused]] static constexpr void QueueConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(StaticQueue<Foo*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(StaticQueue<Foo, 1>);

    static_assert(sizeof(StaticQueue<Foo, 1>::Iterator) == alignof(void*) * 2 + sizeof(SizeT) * 2);
}

template <typename T, SizeT CapN>
static constexpr int TestQueue(StaticQueue<T, CapN> const& que, Array<T> const& arr)
{
    constexpr auto TestElement = [](T const& l, T const& r) -> bool {
        if constexpr (memory::DereferencableT<T>) {
            if (l == nullptr || r == nullptr) return l == r;
            return *l == *r;
        }
        return l == r;
    };

    TEST_ENSURE(que.Size() == arr.Size());
    TEST_ENSURE(que.Size() == Distance(que.Begin(), que.End()));

    if (arr.Size() != 0) {
        TEST_ENSURE(TestElement(que.First(), arr.First()));
        TEST_ENSURE(TestElement(que.Last(), arr.Last()));
    }

    for (SizeT i = 0; i < que.Size(); ++i) {
        TEST_ENSURE(TestElement(que[i], arr[i]));
        TEST_ENSURE(TestElement(que.At(i), arr.At(i)));
    }

    return 0;
}

template <typename T, typename FactoryT>
static constexpr int TestCtor()
{
    TEST_ENSURE((StaticQueue<T, 1>{}.Size() == 0));
    TEST_ENSURE((StaticQueue<T, 1>{}.Capacity() == 1));
    TEST_ENSURE((StaticQueue<T, 16>().Size() == 0));
    TEST_ENSURE((StaticQueue<T, 16>().Capacity() == 16));

    if constexpr (CopyableT<T>) {
        StaticQueue<T, 20> arr;
        int count = 0;
        for (int i = 0; i < 20; ++i) arr.Enqueue(FactoryT{}(++count));

        TEST_ENSURE((StaticQueue<T, 20>(arr) == arr));
        TEST_ENSURE((StaticQueue<T, 20>(StaticQueue<T, 20>(arr)) == arr));
        TEST_ENSURE((StaticQueue<T, 20>(arr.Begin(), arr.End()) == arr));

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(TestQueue(StaticQueue<T, 4>(list), Array<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
static constexpr int TestModify()
{
    StaticQueue<T, 8> que;
    Array<T> arr;
    int arrcount = 33;
    int veccount = 33;

    que.Enqueue(FactoryT{}(++arrcount));
    arr.Push(FactoryT{}(++veccount));
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.Enqueue(ArgFactoryT{}(++arrcount));
    arr.Push(ArgFactoryT{}(++veccount));
    TEST_ENSURE(TestQueue(que, arr) == 0);

    if constexpr (CopyableT<T>) {
        StaticQueue<T, 8> que2(que.Begin(), que.End());

        que.EnqueueRange(que2.Begin(), que2.End());
        arr.Append(que2.Begin(), que2.End());
        TEST_ENSURE(TestQueue(que, arr) == 0);

        que.Clear();
        que.Assign(que2.Begin(), que2.End());
        TEST_ENSURE(que == que2);

        que.EnqueueRange(que2.Begin(), que2.End());

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        que.EnqueueRange(list);
        arr.Append(list);
        TEST_ENSURE(TestQueue(que, arr) == 0);

        que2.Assign(list);
        TEST_ENSURE(TestQueue(que2, Array<T>(list)) == 0);
    }
    else {
        que.Enqueue(ArgFactoryT{}(++arrcount));
        que.Enqueue(ArgFactoryT{}(++arrcount));
        arr.Push(ArgFactoryT{}(++veccount));
        arr.Push(ArgFactoryT{}(++veccount));
    }

    que.Dequeue();
    arr.RemoveAt(0);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.RemoveFirst();
    arr.RemoveAt(0);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.RemoveFirst(2);
    arr.RemoveRange(0, 2);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.Clear();
    arr.Clear();
    TEST_ENSURE(TestQueue(que, arr) == 0);

    return 0;
}

int TestRandom()
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
        q.Assign(q.Begin(), q.End());

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
        iter -= 32;
        iter += 32;
        TEST_ENSURE(++iter == q.End());
    }

    Array<Foo> arr(8);
    {
        arr.Push(Format("hello world! {}", 0));
        arr.Push(Format("hello world! {}", 1));
        arr.Push(Format("hello world! {}", 2));

        Array<Foo> arr2(arr);
        arr.Append(arr.Begin(), arr.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 3);
        arr.Append(arr2.Begin(), arr2.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 2);
        arr.Append(arr2.Begin(), arr2.End());
        arr.RemoveAt(arr.Begin());
        arr.Assign(arr.Begin(), arr.End());
    }

    return TestQueue(q, arr);
}

int main()
{
    static const int dummy = 0;
    using IntPtrF = decltype([](int) { return const_cast<int*>(&dummy); });
    using IntF = decltype([](int c) { return c; });
    using VecF = decltype([](int c) { return Vector2Int(c, c); });

    TEST_ENSURE((TestCtor<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestCtor<std::unique_ptr<ConstexprFoo>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<UniquePtr<ConstexprFoo>, UniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<ConstexprFoo, ConstexprFooF>() == 0));
    TEST_ENSURE((TestCtor<Foo, FooF>() == 0));
    TEST_QUEUE(TestCtor, int, IntF);
    TEST_QUEUE(TestCtor, Vector2Int, VecF);

    TEST_ENSURE((TestModify<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestModify<std::unique_ptr<ConstexprFoo>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestModify<UniquePtr<ConstexprFoo>, UniquePtrF>() == 0));
    TEST_ENSURE((TestModify<ConstexprFoo, ConstexprFooF>() == 0));
    TEST_ENSURE((TestModify<Foo, FooF>() == 0));
    TEST_QUEUE(TestModify, int, IntF);
    TEST_QUEUE(TestModify, Vector2Int, VecF);

    return TestRandom();
}