#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

static constexpr bool ConstexprWeakPtr()
{
    SharedPtr<String> p = MakeShared<String>("string");
    WeakPtr<String> w1 = p;
    WeakPtr<String> w2 = p;

    return w1.OwnerEquals(w2) && w1.OwnerEquals(p);
}

int32 TestCtor()
{
    SharedPtr<Foo> p = MakeShared<Foo>("string");
    WeakPtr<Foo> w(p);
    WeakPtr<Foo> w2(p);

    TEST_ENSURE(WeakPtr<Foo>(w).OwnerEquals(w));
    TEST_ENSURE(WeakPtr<Foo>(p).OwnerEquals(w));
    TEST_ENSURE(WeakPtr<Foo>(MoveArg(w2)).OwnerEquals(w));

    w2 = p;

    TEST_ENSURE(WeakPtr<const Foo>(w).OwnerEquals(w));
    TEST_ENSURE(WeakPtr<const Foo>(MoveArg(w2)).OwnerEquals(w));

    return 0;
}

int32 TestLock()
{
    SharedPtr<Foo> p = MakeShared<Foo>("string");
    WeakPtr<Foo> w(p);
    WeakPtr<Foo> w2 = w;

    TEST_ENSURE(w.Lock().Equals(p));
    {
        w.Lock();
    }
    TEST_ENSURE(p.IsValid());

    TEST_ENSURE(w2.Lock().Equals(p));
    {
        w2.Lock();
    }
    TEST_ENSURE(p.IsValid());

    p.Reset();
    TEST_ENSURE(w.Lock() == nullptr);
    TEST_ENSURE(w2.Lock() == nullptr);

    return 0;
}

int32 TestValid()
{
    SharedPtr<Foo> p = MakeShared<Foo>("string");
    WeakPtr<Foo> w(p);

    TEST_ENSURE(w.IsValid());

    p.Reset();
    TEST_ENSURE(!w.IsValid());

    WeakPtr<Foo> w2(w);
    TEST_ENSURE(!w2.IsValid());

    return 0;
}

int32 TestReset()
{
    SharedPtr<Foo> p = MakeShared<Foo>("string");
    WeakPtr<Foo> w(p);

    w.Reset();
    TEST_ENSURE(!w.IsValid());
    TEST_ENSURE(w.Lock() == nullptr);

    return 0;
}

int32 TestSwap()
{
    SharedPtr<Foo> p = MakeShared<Foo>("string");
    WeakPtr<Foo> w(p);
    WeakPtr<Foo> w2;

    w.Swap(w2);
    TEST_ENSURE(w.Lock() == nullptr);
    TEST_ENSURE(w2.Lock().OwnerEquals(p));

    return 0;
}

int32 main()
{
    static_assert(ConstexprWeakPtr());

    TEST_ENSURE(TestCtor() == 0);
    TEST_ENSURE(TestLock() == 0);
    TEST_ENSURE(TestValid() == 0);
    TEST_ENSURE(TestReset() == 0);
    TEST_ENSURE(TestSwap() == 0);

    return 0;
}