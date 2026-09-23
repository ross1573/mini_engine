#include <memory>
#include <string>

#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

#define TEST_QUEUE(func, type, ...)                              \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                   \
    using name = decltype([](int count) {                    \
        String str("Hello world! This is a long string 0."); \
        *(str.End() - 2) = static_cast<char>(count);         \
        return __VA_ARGS__(str);                             \
    });

FACTORY(UniquePtrF, MakeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] constexpr void QueueConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<int, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<int*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<TestObject, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<TestObject*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(FixedQueue<TestObject, 1>);
    TEST_CONST_RANGE_BASED_FOR_SUPPORT(FixedQueue<TestObject, 1>);

    static_assert(sizeof(FixedQueue<TestObject, 1>::Iterator) == (alignof(void*) * 2) + (sizeof(size_t) * 2));
}

template <typename T, size_t CapN>
constexpr int TestQueue(FixedQueue<T, CapN> const& que, Array<T> const& arr)
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

    TEST_ENSURE(que.Size() == arr.Size());
    TEST_ENSURE(que.Size() == Distance(que.Begin(), que.End()));

    if (arr.Size() != 0) {
        TEST_ENSURE(TestElement(que.First(), arr.First()));
        TEST_ENSURE(TestElement(que.Last(), arr.Last()));
    }

    for (size_t i = 0; i < que.Size(); ++i) {
        TEST_ENSURE(TestElement(que[i], arr[i]));
        TEST_ENSURE(TestElement(que.At(i), arr.At(i)));
    }

    return 0;
}

template <typename T, typename FactoryT>
constexpr int TestCtor()
{
    TEST_ENSURE((FixedQueue<T, 1>{}.Size() == 0));
    TEST_ENSURE((FixedQueue<T, 1>{}.Capacity() == 1));
    TEST_ENSURE((FixedQueue<T, 16>().Size() == 0));
    TEST_ENSURE((FixedQueue<T, 16>().Capacity() == 16));

    if constexpr (CopyableT<T>) {
        FixedQueue<T, 20> arr;
        int count = 0;
        for (int i = 0; i < 20; ++i) {
            arr.PushBack(FactoryT{}(++count));
        }

        TEST_ENSURE((FixedQueue<T, 20>(arr) == arr));
        TEST_ENSURE((FixedQueue<T, 20>(FixedQueue<T, 20>(arr)) == arr));
        TEST_ENSURE((FixedQueue<T, 20>(arr.Begin(), arr.End()) == arr));

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(TestQueue(FixedQueue<T, 4>(list), Array<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
constexpr int TestModify()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    int arrcount = 33;
    int veccount = 33;

    que.PushBack(FactoryT{}(++arrcount));
    arr.PushBack(FactoryT{}(++veccount));
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.PushBack(ArgFactoryT{}(++arrcount));
    arr.PushBack(ArgFactoryT{}(++veccount));
    TEST_ENSURE(TestQueue(que, arr) == 0);

    if constexpr (CopyableT<T>) {
        FixedQueue<T, 8> que2(que.Begin(), que.End());

        que.Append(que2.Begin(), que2.End());
        arr.Append(que2.Begin(), que2.End());
        TEST_ENSURE(TestQueue(que, arr) == 0);

        que.Clear();
        que.Assign(que2.Begin(), que2.End());
        TEST_ENSURE(que == que2);

        que.Append(que2.Begin(), que2.End());

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        que.Append(list);
        arr.Append(list);
        TEST_ENSURE(TestQueue(que, arr) == 0);

        que2.Assign(list);
        TEST_ENSURE(TestQueue(que2, Array<T>(list)) == 0);
    } else {
        que.PushBack(ArgFactoryT{}(++arrcount));
        que.PushBack(ArgFactoryT{}(++arrcount));
        arr.PushBack(ArgFactoryT{}(++veccount));
        arr.PushBack(ArgFactoryT{}(++veccount));
    }

    que.PopFront();
    arr.Remove(0);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.PopFront();
    arr.Remove(0);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.PopFront(2);
    arr.RemoveRange(arr.Begin(), arr.Begin() + 2);
    TEST_ENSURE(TestQueue(que, arr) == 0);

    que.Clear();
    arr.Clear();
    TEST_ENSURE(TestQueue(que, arr) == 0);

    return 0;
}

int TestRandom()
{
    FixedQueue<TestObject, 8> q;
    {
        static_assert(q.Capacity() == 8);

        q.PushBack(Format("hello world! {}", 0));
        q.PushBack(Format("hello world! {}", 1));
        q.PushBack(Format("hello world! {}", 2));

        decltype(q) q2 = q;
        decltype(q) q3 = MoveArg(q2);
        decltype(q)::Iterator iter = q3.Begin();
        decltype(q)::Iterator end = q3.End();

        q.Append(iter, end);
        q.PopFront(3);
        q.Append(iter, end);
        q.PopFront(2);
        q.Append(iter, end);
        q.PopFront();
        q.Assign(q.Begin(), q.End());

        iter = q.Begin();
        TEST_ENSURE(!q.Empty());
        TEST_ENSURE(!q.Full());
        TEST_ENSURE(iter.Increment());
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter.Finish());
        TEST_ENSURE(!q.ValidIterator(iter));
        TEST_ENSURE(!q3.ValidIterator(--iter));
        TEST_ENSURE(q.ValidIterator(iter));
        TEST_ENSURE(!q.ValidIndex(q.Size()));
        TEST_ENSURE(q.ValidIndex(q.Size() - 1));
        TEST_ENSURE(iter.Finish());
        TEST_ENSURE(iter.Increment() == false);
        TEST_ENSURE(iter.Decrement());
        TEST_ENSURE(iter.Advance(-5));
        TEST_ENSURE(iter.Advance(5));
        iter -= 32;
        iter += 32;
        TEST_ENSURE(++iter == q.End());
    }

    Array<TestObject> arr(8);
    {
        arr.PushBack(Format("hello world! {}", 0));
        arr.PushBack(Format("hello world! {}", 1));
        arr.PushBack(Format("hello world! {}", 2));

        Array<TestObject> arr2(arr);
        arr.Append(arr.Begin(), arr.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 3);
        arr.Append(arr2.Begin(), arr2.End());
        arr.RemoveRange(arr.Begin(), arr.Begin() + 2);
        arr.Append(arr2.Begin(), arr2.End());
        arr.Remove(arr.Begin());
        arr.Assign(arr.Begin(), arr.End());
    }

    return TestQueue(q, arr);
}

int main()
{
    static const int dummy = 0;
    using IntPtrF = decltype([](int) { return const_cast<int*>(&dummy); });
    using IntF = decltype([](int count) { return count; });
    using VecF = decltype([](int count) { return Vector2Int(count, count); });

    TEST_ENSURE((TestCtor<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestCtor<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestCtor<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((TestCtor<TestObject, FooF>() == 0));
    TEST_QUEUE(TestCtor, int, IntF);
    TEST_QUEUE(TestCtor, Vector2Int, VecF);

    TEST_ENSURE((TestModify<int*, IntPtrF>() == 0));
    TEST_ENSURE((TestModify<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((TestModify<UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((TestModify<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((TestModify<TestObject, FooF>() == 0));
    TEST_QUEUE(TestModify, int, IntF);
    TEST_QUEUE(TestModify, Vector2Int, VecF);

    return TestRandom();
}