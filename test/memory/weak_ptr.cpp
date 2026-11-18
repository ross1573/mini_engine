#include "test_macro.h"

import mini.test;

using mini::makeShared;
using mini::SharedPtr;
using mini::WeakPtr;

#if MSVC
#  undef TEST_CONSTEXPR
#  define TEST_CONSTEXPR(func) TEST_FUNCTION(func)
#endif

struct Base {
    mini::int32 value = 1;

    virtual ~Base() = default;
};

struct Derived : Base { };

struct Left {
    mini::int32 left = 3;
};

struct Right {
    mini::int32 right = 4;
};

struct Both : Left, Right { };

constexpr WeakPtr<CopyableObject> makeExpiredWeak()
{
    SharedPtr<CopyableObject> p(new CopyableObject(1), mini::DefaultDeleter<CopyableObject>{}, CountingAlloc<CopyableObject>{});
    return WeakPtr<CopyableObject>(p);
}

constexpr mini::int32 testSharedCtor()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);
    TEST_ENSURE(w.ownerEquals(p));

    WeakPtr<CopyableObject> fromEmpty = SharedPtr<CopyableObject>();
    TEST_ENSURE(!fromEmpty.valid());
    TEST_ENSURE(fromEmpty.lock() == nullptr);
    TEST_ENSURE(fromEmpty.ownerEquals(WeakPtr<CopyableObject>()));

    SharedPtr<Derived> derived = makeShared<Derived>();
    WeakPtr<Base> fromDerived(derived);
    TEST_ENSURE(fromDerived.ownerEquals(derived));
    TEST_ENSURE(fromDerived.lock().get() == derived.get());

    return 0;
}

constexpr mini::int32 testCopyCtor()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);

    WeakPtr<CopyableObject> copied(w);
    TEST_ENSURE(copied.ownerEquals(w));
    TEST_ENSURE(w.valid());

    return 0;
}

constexpr mini::int32 testMoveCtor()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);
    WeakPtr<CopyableObject> w2(p);

    WeakPtr<CopyableObject> moved(mini::moveCast(w2));
    TEST_ENSURE(moved.ownerEquals(w));
    TEST_ENSURE(!w2.valid());

    return 0;
}

constexpr mini::int32 testConvertCtor()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);
    WeakPtr<CopyableObject> w2(p);

    TEST_ENSURE(WeakPtr<CopyableObject const>(w).ownerEquals(w));
    TEST_ENSURE(WeakPtr<CopyableObject const>(mini::moveCast(w2)).ownerEquals(w));
    TEST_ENSURE(!w2.valid());

    SharedPtr<Derived> derived = makeShared<Derived>();
    WeakPtr<Derived> derivedWeak = derived;

    WeakPtr<Base> baseWeak = derivedWeak;
    TEST_ENSURE(baseWeak.ownerEquals(derived));
    TEST_ENSURE(baseWeak.lock()->value == 1);

    WeakPtr<Base> baseMoved(mini::moveCast(derivedWeak));
    TEST_ENSURE(baseMoved.ownerEquals(derived));
    TEST_ENSURE(!derivedWeak.valid());

    return 0;
}

constexpr mini::int32 testSharedAssign()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(2);
    SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(3);

    WeakPtr<CopyableObject> w;
    w = p;
    TEST_ENSURE(w.ownerEquals(p));

    w = p2;
    TEST_ENSURE(w.ownerEquals(p2));
    TEST_ENSURE(!w.ownerEquals(p));

    SharedPtr<Derived> derived = makeShared<Derived>();
    WeakPtr<Base> baseWeak;
    baseWeak = derived;
    TEST_ENSURE(baseWeak.ownerEquals(derived));

    return 0;
}

constexpr mini::int32 testCopyAssign()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(2);
    WeakPtr<CopyableObject> w = p;

    WeakPtr<CopyableObject> w2;
    w2 = w;
    TEST_ENSURE(w2.ownerEquals(p));
    TEST_ENSURE(w.ownerEquals(p));

    w2 = makeLvalueReference(w2);
    TEST_ENSURE(w2.ownerEquals(p));

    return 0;
}

constexpr mini::int32 testMoveAssign()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(2);
    WeakPtr<CopyableObject> w = p;

    WeakPtr<CopyableObject> w2;
    w2 = mini::moveCast(w);
    TEST_ENSURE(w2.ownerEquals(p));
    TEST_ENSURE(!w.valid());

    WeakPtr<CopyableObject> empty;
    w2 = mini::moveCast(empty);
    TEST_ENSURE(!w2.valid());
    TEST_ENSURE(w2.ownerEquals(WeakPtr<CopyableObject>()));

    return 0;
}

constexpr mini::int32 testConvertAssign()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(2);
    SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(3);
    WeakPtr<CopyableObject> w = p;
    WeakPtr<CopyableObject> w2 = p2;

    WeakPtr<CopyableObject const> constWeak;
    constWeak = w;
    TEST_ENSURE(constWeak.ownerEquals(p));

    constWeak = mini::moveCast(w2);
    TEST_ENSURE(constWeak.ownerEquals(p2));
    TEST_ENSURE(!w2.valid());

    constWeak = p;
    TEST_ENSURE(constWeak.ownerEquals(p));
    TEST_ENSURE(constWeak.lock()->value() == 2);

    return 0;
}

constexpr mini::int32 testLock()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);

    TEST_ENSURE(w.lock().equals(p));
    TEST_ENSURE(w.lock()->value() == 1);
    {
        [[maybe_unused]] auto locked = w.lock();
    }
    TEST_ENSURE(p.valid());

    p.reset();
    TEST_ENSURE(w.lock() == nullptr);

    return 0;
}

constexpr mini::int32 testValid()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);
    TEST_ENSURE(w.valid());

    p.reset();
    TEST_ENSURE(!w.valid());

    WeakPtr<CopyableObject> w2(w);
    TEST_ENSURE(!w2.valid());

    return 0;
}

mini::int32 testExpire()
{
    initializeCounter();
    {
        WeakPtr<CopyableObject> w;
        {
            SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
            w = p;
            TEST_ENSURE(w.valid());
            TEST_ENSURE(dtor == 0);
        }
        TEST_ENSURE(dtor == 1);
        TEST_ENSURE(!w.valid());
        TEST_ENSURE(w.lock() == nullptr);
    }
    TEST_ENSURE(ctor == 1 && dtor == 1);

    return 0;
}

constexpr mini::int32 testExpireAllocator()
{
    if !consteval {
        initializeCounter();
    }
    {
        WeakPtr<CopyableObject> w;
        WeakPtr<CopyableObject> inplace;
        {
            CountingAlloc<CopyableObject> alloc;
            SharedPtr<CopyableObject> p(new CopyableObject(1), mini::DefaultDeleter<CopyableObject>{}, alloc);
            SharedPtr<CopyableObject> q = mini::allocateShared<CopyableObject>(alloc, 2);
            w = p;
            inplace = q;
            if !consteval {
                TEST_ENSURE(aliveAllocCount == 2);
            }
        }
        TEST_ENSURE(!w.valid());
        TEST_ENSURE(!inplace.valid());
        TEST_ENSURE(w.lock() == nullptr);
        TEST_ENSURE(inplace.lock() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 2);
            TEST_ENSURE(aliveAllocCount == 2);
        }

        w.reset();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
        TEST_ENSURE(aliveAllocCount == 0);
    }

    return 0;
}

constexpr mini::int32 testReset()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);

    w.reset();
    TEST_ENSURE(!w.valid());
    TEST_ENSURE(w.lock() == nullptr);
    TEST_ENSURE(p.valid());

    return 0;
}

constexpr mini::int32 testSwap()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w(p);
    WeakPtr<CopyableObject> w2;

    w.swap(w2);
    TEST_ENSURE(w.lock() == nullptr);
    TEST_ENSURE(w2.lock().ownerEquals(p));

    SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(4);
    w = p2;
    w.swap(w2);
    TEST_ENSURE(w.ownerEquals(p));
    TEST_ENSURE(w2.ownerEquals(p2));

    return 0;
}

constexpr mini::int32 testOwnerEquals()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    WeakPtr<CopyableObject> w = p;
    WeakPtr<CopyableObject> w2 = p;
    WeakPtr<CopyableObject> empty;
    WeakPtr<CopyableObject> empty2;

    TEST_ENSURE(w.ownerEquals(w2));
    TEST_ENSURE(w.ownerEquals(p));
    TEST_ENSURE(empty.ownerEquals(empty2));
    TEST_ENSURE(!empty.ownerEquals(w));
    TEST_ENSURE(empty.ownerEquals(SharedPtr<CopyableObject>()));

    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;
    WeakPtr<Derived> derivedWeak = derived;
    WeakPtr<Base> baseWeak = derived;
    WeakPtr<Base> otherWeak = makeShared<Derived>();

    TEST_ENSURE(baseWeak.ownerEquals(derivedWeak));
    TEST_ENSURE(derivedWeak.ownerEquals(baseWeak));
    TEST_ENSURE(derivedWeak.ownerEquals(base));
    TEST_ENSURE(baseWeak.ownerEquals(derived));
    TEST_ENSURE(!otherWeak.ownerEquals(derivedWeak));

    return 0;
}

constexpr mini::int32 testMoveAssignSelf()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p(new CopyableObject(2), mini::DefaultDeleter<CopyableObject>{}, CountingAlloc<CopyableObject>{});
        WeakPtr<CopyableObject> w = p;

        w = makeRvalueReference(w);
        TEST_ENSURE(w.valid());
        TEST_ENSURE(w.ownerEquals(p));
        TEST_ENSURE(w.lock()->value() == 2);

        p.reset();
        TEST_ENSURE(!w.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
            TEST_ENSURE(aliveAllocCount == 1);
        }

        w = makeRvalueReference(w);
        TEST_ENSURE(!w.valid());
        TEST_ENSURE(!w.ownerEquals(WeakPtr<CopyableObject>()));
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }

        w.reset();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testConvertAssignSameOwner()
{
    SharedPtr<Derived> derived = makeShared<Derived>();
    WeakPtr<Derived> derivedWeak = derived;
    WeakPtr<Base> baseWeak = derived;

    baseWeak = derivedWeak;
    TEST_ENSURE(baseWeak.ownerEquals(derived));
    TEST_ENSURE(derivedWeak.valid());

    baseWeak = mini::moveCast(derivedWeak);
    TEST_ENSURE(!derivedWeak.valid());
    TEST_ENSURE(baseWeak.lock().get() == derived.get());

    derived.reset();
    TEST_ENSURE(!baseWeak.valid());

    return 0;
}

constexpr mini::int32 testAliasOwnership()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    SharedPtr<mini::int32> alias(p, p->resource());
    WeakPtr<CopyableObject> w = p;
    WeakPtr<mini::int32> aliasWeak = alias;

    TEST_ENSURE(aliasWeak.lock().ownerEquals(p));
    TEST_ENSURE(w.lock().ownerEquals(aliasWeak.lock()));
    TEST_ENSURE(aliasWeak.lock().get() == p->resource());

    p.reset();
    TEST_ENSURE(w.valid());
    TEST_ENSURE(aliasWeak.valid());
    TEST_ENSURE(*aliasWeak.lock() == 1);

    alias.reset();
    TEST_ENSURE(!w.valid());
    TEST_ENSURE(!aliasWeak.valid());

    return 0;
}

constexpr mini::int32 testAliasConvertLock()
{
    SharedPtr<Derived> owner = makeShared<Derived>();
    Derived other;
    SharedPtr<Derived> alias(owner, &other);
    WeakPtr<Derived> aliasWeak = alias;

    WeakPtr<Base> baseWeak = aliasWeak;
    SharedPtr<Base> locked = baseWeak.lock();
    TEST_ENSURE(locked.get() == &other);
    TEST_ENSURE(locked.ownerEquals(owner));

    WeakPtr<Base> movedWeak(mini::moveCast(aliasWeak));
    TEST_ENSURE(movedWeak.lock().get() == &other);
    TEST_ENSURE(movedWeak.ownerEquals(owner));

    WeakPtr<Base> assigned;
    assigned = movedWeak;
    TEST_ENSURE(assigned.lock().get() == &other);

    return 0;
}

constexpr mini::int32 testLockExpiredAlias()
{
    SharedPtr<Derived> owner = makeShared<Derived>();
    Derived other;
    SharedPtr<Base> alias(owner, &other);
    WeakPtr<Base> aliasWeak = alias;

    owner.reset();
    alias.reset();

    SharedPtr<Base> locked = aliasWeak.lock();
    TEST_ENSURE(locked.get() == nullptr);
    TEST_ENSURE(locked.ownerEquals(SharedPtr<Base>()));
    TEST_ENSURE(!aliasWeak.valid());

    return 0;
}

mini::int32 testLockKeepsAlive()
{
    initializeCounter();
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        WeakPtr<CopyableObject> w = p;

        SharedPtr<CopyableObject> locked = w.lock();
        p.reset();
        TEST_ENSURE(dtor == 0);
        TEST_ENSURE(w.valid());
        TEST_ENSURE(locked->value() == 1);

        locked.reset();
        TEST_ENSURE(dtor == 1);
        TEST_ENSURE(!w.valid());
    }
    TEST_ENSURE(ctor == 1 && dtor == 1);

    return 0;
}

constexpr mini::int32 testEmpty()
{
    WeakPtr<CopyableObject> empty;
    TEST_ENSURE(!empty.valid());
    TEST_ENSURE(empty.lock() == nullptr);

    WeakPtr<CopyableObject> copied(empty);
    TEST_ENSURE(!copied.valid());
    TEST_ENSURE(copied.ownerEquals(empty));

    WeakPtr<CopyableObject> moved(mini::moveCast(copied));
    TEST_ENSURE(!moved.valid());
    TEST_ENSURE(moved.ownerEquals(empty));

    WeakPtr<CopyableObject const> converted(empty);
    TEST_ENSURE(!converted.valid());
    TEST_ENSURE(converted.lock() == nullptr);

    moved.reset();
    TEST_ENSURE(!moved.valid());

    moved.swap(copied);
    TEST_ENSURE(!moved.valid());
    TEST_ENSURE(!copied.valid());

    copied = empty;
    TEST_ENSURE(!copied.valid());
    converted = SharedPtr<CopyableObject>();
    TEST_ENSURE(!converted.valid());

    return 0;
}

constexpr mini::int32 testReleaseBlock()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> other = makeShared<CopyableObject>(2);
        WeakPtr<CopyableObject> otherWeak = other;

        WeakPtr<CopyableObject> w = makeExpiredWeak();
        TEST_ENSURE(!w.valid());
        TEST_ENSURE(w.lock() == nullptr);
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
            TEST_ENSURE(dtor == 1);
        }

        w = other;
        TEST_ENSURE(w.ownerEquals(other));
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        w = makeExpiredWeak();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }
        w = otherWeak;
        TEST_ENSURE(w.ownerEquals(other));
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        w = makeExpiredWeak();
        w = WeakPtr<CopyableObject>();
        TEST_ENSURE(!w.valid());
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        WeakPtr<CopyableObject const> constWeak = makeExpiredWeak();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }
        constWeak = otherWeak;
        TEST_ENSURE(constWeak.ownerEquals(other));
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        constWeak = makeExpiredWeak();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }
        constWeak = WeakPtr<CopyableObject>(other);
        TEST_ENSURE(constWeak.ownerEquals(other));
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        constWeak = makeExpiredWeak();
        constWeak.reset();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        {
            WeakPtr<CopyableObject> scoped = makeExpiredWeak();
            if !consteval {
                TEST_ENSURE(aliveAllocCount == 1);
            }
        }
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }

        TEST_ENSURE(other->value() == 2);
        TEST_ENSURE(otherWeak.valid());
    }
    if !consteval {
        TEST_ENSURE(ctor == 8 && dtor == 8);
        TEST_ENSURE(aliveAllocCount == 0);
    }

    return 0;
}

constexpr mini::int32 testMultipleInheritance()
{
    SharedPtr<Both> both = makeShared<Both>();
    WeakPtr<Both> bothWeak = both;

    WeakPtr<Right> right = both;
    TEST_ENSURE(right.lock().get() == static_cast<Right*>(both.get()));
    TEST_ENSURE(right.lock()->right == 4);
    TEST_ENSURE(right.ownerEquals(both));

    WeakPtr<Right> fromWeak = bothWeak;
    TEST_ENSURE(fromWeak.lock().get() == static_cast<Right*>(both.get()));

    WeakPtr<Right> assigned;
    assigned = bothWeak;
    TEST_ENSURE(assigned.lock().get() == static_cast<Right*>(both.get()));

    assigned = both;
    TEST_ENSURE(assigned.lock().get() == static_cast<Right*>(both.get()));

    WeakPtr<Right> moved(mini::moveCast(bothWeak));
    TEST_ENSURE(moved.lock().get() == static_cast<Right*>(both.get()));
    TEST_ENSURE(!bothWeak.valid());

    both.reset();
    TEST_ENSURE(!right.valid());
    TEST_ENSURE(right.lock() == nullptr);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testSharedCtor);
    TEST_CONSTEXPR(testCopyCtor);
    TEST_CONSTEXPR(testMoveCtor);
    TEST_CONSTEXPR(testConvertCtor);

    TEST_CONSTEXPR(testSharedAssign);
    TEST_CONSTEXPR(testCopyAssign);
    TEST_CONSTEXPR(testMoveAssign);
    TEST_CONSTEXPR(testConvertAssign);

    TEST_CONSTEXPR(testLock);
    TEST_CONSTEXPR(testValid);
    TEST_FUNCTION(testExpire);
    TEST_CONSTEXPR(testExpireAllocator);
    TEST_CONSTEXPR(testReset);
    TEST_CONSTEXPR(testSwap);

    TEST_CONSTEXPR(testOwnerEquals);

    TEST_CONSTEXPR(testMoveAssignSelf);
    TEST_CONSTEXPR(testConvertAssignSameOwner);

    TEST_CONSTEXPR(testAliasOwnership);
    TEST_CONSTEXPR(testAliasConvertLock);
    TEST_CONSTEXPR(testLockExpiredAlias);

    TEST_FUNCTION(testLockKeepsAlive);
    TEST_CONSTEXPR(testEmpty);
    TEST_CONSTEXPR(testReleaseBlock);
    TEST_CONSTEXPR(testMultipleInheritance);

    TEST_ENSURE(aliveAllocCount == 0);
    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
