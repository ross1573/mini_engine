#include "test_macro.h"

import mini.test;

using mini::makeShared;
using mini::SharedPtr;
using mini::WeakPtr;

constexpr bool ConstexprWeakPtr()
{
    SharedPtr<mini::String> p = makeShared<mini::String>("string");
    WeakPtr<mini::String> w1 = p;
    WeakPtr<mini::String> w2 = p;

    return w1.ownerEquals(w2) && w1.ownerEquals(p);
}

mini::int32 TestCtor()
{
    SharedPtr<TestObject> p = makeShared<TestObject>("string");
    WeakPtr<TestObject> w(p);
    WeakPtr<TestObject> w2(p);

    TEST_ENSURE(WeakPtr<TestObject>(w).ownerEquals(w));
    TEST_ENSURE(WeakPtr<TestObject>(p).ownerEquals(w));
    TEST_ENSURE(WeakPtr<TestObject>(moveCast(w2)).ownerEquals(w));

    w2 = p;

    TEST_ENSURE(WeakPtr<const TestObject>(w).ownerEquals(w));
    TEST_ENSURE(WeakPtr<const TestObject>(moveCast(w2)).ownerEquals(w));

    return 0;
}

mini::int32 TestLock()
{
    SharedPtr<TestObject> p = makeShared<TestObject>("string");
    WeakPtr<TestObject> w(p);
    WeakPtr<TestObject> w2 = w;

    TEST_ENSURE(w.lock().equals(p));
    {
        [[maybe_unused]] auto _ = w.lock();
    }
    TEST_ENSURE(p.valid());

    TEST_ENSURE(w2.lock().equals(p));
    {
        [[maybe_unused]] auto _ = w2.lock();
    }
    TEST_ENSURE(p.valid());

    p.reset();
    TEST_ENSURE(w.lock() == nullptr);
    TEST_ENSURE(w2.lock() == nullptr);

    return 0;
}

mini::int32 TestValid()
{
    SharedPtr<TestObject> p = makeShared<TestObject>("string");
    WeakPtr<TestObject> w(p);

    TEST_ENSURE(w.valid());

    p.reset();
    TEST_ENSURE(!w.valid());

    WeakPtr<TestObject> w2(w);
    TEST_ENSURE(!w2.valid());

    return 0;
}

mini::int32 TestReset()
{
    SharedPtr<TestObject> p = makeShared<TestObject>("string");
    WeakPtr<TestObject> w(p);

    w.reset();
    TEST_ENSURE(!w.valid());
    TEST_ENSURE(w.lock() == nullptr);

    return 0;
}

mini::int32 TestSwap()
{
    SharedPtr<TestObject> p = makeShared<TestObject>("string");
    WeakPtr<TestObject> w(p);
    WeakPtr<TestObject> w2;

    w.swap(w2);
    TEST_ENSURE(w.lock() == nullptr);
    TEST_ENSURE(w2.lock().ownerEquals(p));

    return 0;
}

mini::int32 main()
{
    static_assert(ConstexprWeakPtr());

    TEST_ENSURE(TestCtor() == 0);
    TEST_ENSURE(TestLock() == 0);
    TEST_ENSURE(TestValid() == 0);
    TEST_ENSURE(TestReset() == 0);
    TEST_ENSURE(TestSwap() == 0);

    return 0;
}