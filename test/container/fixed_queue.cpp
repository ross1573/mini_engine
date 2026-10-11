#include <memory>
#include <string>

#include "test_macro.h"

import mini.test;

using mini::Array;
using mini::FixedQueue;
using mini::InitializerList;

#define TEST_QUEUE(func, type, ...)                              \
    static_assert(func<type __VA_OPT__(, ) __VA_ARGS__>() == 0); \
    TEST_ENSURE((func<type __VA_OPT__(, ) __VA_ARGS__>() == 0));

#define FACTORY(name, ...)                                         \
    using name = decltype([](mini::int32 count) {                  \
        mini::String str("Hello world! This is a long string 0."); \
        *(str.end() - 2) = static_cast<char>(count);               \
        return __VA_ARGS__(str);                                   \
    });

FACTORY(UniquePtrF, mini::makeUnique<ConstexprObject>);
FACTORY(StdUniquePtrF, std::make_unique<ConstexprObject>);
FACTORY(ConstexprFooF, ConstexprObject);
FACTORY(FooF, TestObject);
FACTORY(FooArgF);

[[maybe_unused]] constexpr void queueConstraints()
{
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<mini::int32, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<mini::int32*, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<TestObject, 1>::Iterator);
    RANDOM_ACCESS_ITERATOR_CONSTRAINTS(FixedQueue<TestObject*, 1>::Iterator);

    TEST_RANGE_BASED_FOR_SUPPORT(FixedQueue<TestObject, 1>);
    TEST_CONST_RANGE_BASED_FOR_SUPPORT(FixedQueue<TestObject, 1>);

    static_assert(sizeof(FixedQueue<TestObject, 1>::Iterator) == (alignof(void*) * 2) + (sizeof(size_t) * 2));
}

template <typename T, size_t CapN>
constexpr mini::int32 testQueue(FixedQueue<T, CapN> const& que, Array<T> const& arr)
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

    TEST_ENSURE(que.size() == arr.size());
    TEST_ENSURE(que.size() == distance(que.begin(), que.end()));

    if (!arr.empty()) {
        TEST_ENSURE(testElement(que.first(), arr.first()));
        TEST_ENSURE(testElement(que.last(), arr.last()));
    }

    for (size_t i = 0; i < que.size(); ++i) {
        TEST_ENSURE(testElement(que[i], arr[i]));
        TEST_ENSURE(testElement(que.at(i), arr.at(i)));
    }

    return 0;
}

template <typename T, typename FactoryT>
constexpr mini::int32 testCtor()
{
    TEST_ENSURE((FixedQueue<T, 1>{}.empty()));
    TEST_ENSURE((FixedQueue<T, 1>{}.capacity() == 1));
    TEST_ENSURE((FixedQueue<T, 16>().empty()));
    TEST_ENSURE((FixedQueue<T, 16>().capacity() == 16));

    if constexpr (mini::CopyableT<T>) {
        FixedQueue<T, 20> arr;
        mini::int32 count = 0;
        for (mini::int32 i = 0; i < 20; ++i) {
            arr.pushBack(FactoryT{}(++count));
        }

        TEST_ENSURE((FixedQueue<T, 20>(arr) == arr));
        TEST_ENSURE((FixedQueue<T, 20>(FixedQueue<T, 20>(arr)) == arr));
        TEST_ENSURE((FixedQueue<T, 20>(arr.begin(), arr.end()) == arr));

        InitializerList list = {
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
            FactoryT{}(++count),
        };

        TEST_ENSURE(testQueue(FixedQueue<T, 4>(list), Array<T>(list)) == 0);
    }

    return 0;
}

template <typename T, typename FactoryT, typename ArgFactoryT = FactoryT>
constexpr mini::int32 testModify()
{
    FixedQueue<T, 8> que;
    Array<T> arr;
    mini::int32 arrcount = 33;
    mini::int32 veccount = 33;

    que.pushBack(FactoryT{}(++arrcount));
    arr.pushBack(FactoryT{}(++veccount));
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.pushBack(ArgFactoryT{}(++arrcount));
    arr.pushBack(ArgFactoryT{}(++veccount));
    TEST_ENSURE(testQueue(que, arr) == 0);

    if constexpr (mini::CopyableT<T>) {
        FixedQueue<T, 8> que2(que.begin(), que.end());

        que.append(que2.begin(), que2.end());
        arr.append(que2.begin(), que2.end());
        TEST_ENSURE(testQueue(que, arr) == 0);

        que.clear();
        que.assign(que2.begin(), que2.end());
        TEST_ENSURE(que == que2);

        que.append(que2.begin(), que2.end());

        InitializerList list = {
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
            FactoryT{}(++arrcount),
        };

        que.append(list);
        arr.append(list);
        TEST_ENSURE(testQueue(que, arr) == 0);

        que2.assign(list);
        TEST_ENSURE(testQueue(que2, Array<T>(list)) == 0);
    } else {
        que.pushBack(ArgFactoryT{}(++arrcount));
        que.pushBack(ArgFactoryT{}(++arrcount));
        arr.pushBack(ArgFactoryT{}(++veccount));
        arr.pushBack(ArgFactoryT{}(++veccount));
    }

    que.popFront();
    arr.remove(0);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.popFront();
    arr.remove(0);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.popFront(2);
    arr.removeRange(arr.begin(), arr.begin() + 2);
    TEST_ENSURE(testQueue(que, arr) == 0);

    que.clear();
    arr.clear();
    TEST_ENSURE(testQueue(que, arr) == 0);

    return 0;
}

mini::int32 testRandom()
{
    FixedQueue<TestObject, 8> q;
    {
        static_assert(q.capacity() == 8);

        q.pushBack(mini::format("hello world! {}", 0));
        q.pushBack(mini::format("hello world! {}", 1));
        q.pushBack(mini::format("hello world! {}", 2));

        decltype(q) q2 = q;
        decltype(q) q3 = moveCast(q2);
        decltype(q)::Iterator iter = q3.begin();
        decltype(q)::Iterator end = q3.end();

        q.append(iter, end);
        q.popFront(3);
        q.append(iter, end);
        q.popFront(2);
        q.append(iter, end);
        q.popFront();
        q.assign(q.begin(), q.end());

        iter = q.begin();
        TEST_ENSURE(!q.empty());
        TEST_ENSURE(!q.full());
        TEST_ENSURE(iter.increment());
        TEST_ENSURE(iter.decrement());
        TEST_ENSURE(iter.finish());
        TEST_ENSURE(!q.validIterator(iter));
        TEST_ENSURE(!q3.validIterator(--iter));
        TEST_ENSURE(q.validIterator(iter));
        TEST_ENSURE(!q.validIndex(q.size()));
        TEST_ENSURE(q.validIndex(q.size() - 1));
        TEST_ENSURE(iter.finish());
        TEST_ENSURE(iter.increment() == false);
        TEST_ENSURE(iter.decrement());
        TEST_ENSURE(iter.advance(-5));
        TEST_ENSURE(iter.advance(5));
        iter -= 32;
        iter += 32;
        TEST_ENSURE(++iter == q.end());
    }

    Array<TestObject> arr(8);
    {
        arr.pushBack(mini::format("hello world! {}", 0));
        arr.pushBack(mini::format("hello world! {}", 1));
        arr.pushBack(mini::format("hello world! {}", 2));

        Array<TestObject> arr2(arr);
        arr.append(arr.begin(), arr.end());
        arr.removeRange(arr.begin(), arr.begin() + 3);
        arr.append(arr2.begin(), arr2.end());
        arr.removeRange(arr.begin(), arr.begin() + 2);
        arr.append(arr2.begin(), arr2.end());
        arr.remove(arr.begin());
        arr.assign(arr.begin(), arr.end());
    }

    return testQueue(q, arr);
}

mini::int32 main()
{
    static const mini::int32 dummy = 0;
    using IntPtrF = decltype([](mini::int32) { return const_cast<mini::int32*>(&dummy); });
    using IntF = decltype([](mini::int32 count) { return count; });
    using TrivialObjectF = decltype([](mini::int32 count) { return TrivialObject(count, count); });

    TEST_ENSURE((testCtor<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testCtor<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testCtor<mini::UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((testCtor<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((testCtor<TestObject, FooF>() == 0));
    TEST_QUEUE(testCtor, mini::int32, IntF);
    TEST_QUEUE(testCtor, TrivialObject, TrivialObjectF);

    TEST_ENSURE((testModify<mini::int32*, IntPtrF>() == 0));
    TEST_ENSURE((testModify<std::unique_ptr<ConstexprObject>, StdUniquePtrF>() == 0));
    TEST_ENSURE((testModify<mini::UniquePtr<ConstexprObject>, UniquePtrF>() == 0));
    TEST_ENSURE((testModify<ConstexprObject, ConstexprFooF>() == 0));
    TEST_ENSURE((testModify<TestObject, FooF>() == 0));
    TEST_QUEUE(testModify, mini::int32, IntF);
    TEST_QUEUE(testModify, TrivialObject, TrivialObjectF);

    return testRandom();
}