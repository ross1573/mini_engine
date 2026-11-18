#include "test_macro.h"

import mini.test;

using mini::allocateShared;
using mini::constCast;
using mini::dynamicCast;
using mini::makeShared;
using mini::PtrView;
using mini::reinterpretCast;
using mini::SharedPtr;
using mini::staticCast;
using mini::WeakPtr;

#if MSVC
#  undef TEST_CONSTEXPR
#  define TEST_CONSTEXPR(func) TEST_FUNCTION(func)
#endif

struct CountingDeleter {
    static inline mini::int32 calls = 0;

    void operator()(CopyableObject* ptr) noexcept
    {
        ++calls;
        delete ptr;
    }
};

struct Base {
    mini::int32 value = 1;

    virtual ~Base() = default;
};

struct Derived : Base {
    mini::int32 extra = 2;
};

struct Other : Base { };

struct BaseDeleter {
    mini::int32* calls = nullptr;

    constexpr void operator()(Base* ptr) noexcept
    {
        ++*calls;
        delete ptr;
    }
};

struct ValidatableObject {
    bool alive = true;

    [[nodiscard]] constexpr bool valid() const noexcept { return alive; }
};

struct Node {
    SharedPtr<Node> next;
    mini::int32 value = 0;
};

struct Left {
    mini::int32 left = 3;
};

struct Right {
    mini::int32 right = 4;
};

struct Both
    : Left
    , Right { };

constexpr mini::int32 testNullCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        auto nullDeleter = [](mini::nullptr_t) noexcept { };
        auto nullAllocator = mini::Allocator<mini::nullptr_t>{};

        SharedPtr<CopyableObject> p;
        SharedPtr<CopyableObject> p2(nullptr);
        SharedPtr<CopyableObject> p3(nullptr, mini::DefaultDeleter<CopyableObject>{});
        SharedPtr<CopyableObject> p4(nullptr, nullDeleter);
        SharedPtr<CopyableObject> p5(nullptr, nullDeleter, nullAllocator);

        TEST_ENSURE(p == nullptr && nullptr == p);
        TEST_ENSURE(p.get() == nullptr);
        TEST_ENSURE((p2 == p3) && (p3 == p4) && (p4 == p5));
        TEST_ENSURE(!p.valid() && !p2.valid() && !p3.valid() && !p4.valid() && !p5.valid());
    }
    if !consteval {
        TEST_ENSURE(ctor == 0 && dtor == 0);
    }

    return 0;
}

constexpr mini::int32 testPointerCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p(new CopyableObject(2));
        SharedPtr<CopyableObject> p2(new CopyableObject(3), mini::DefaultDeleter<CopyableObject>{});
        TEST_ENSURE(p->value() == 2);
        TEST_ENSURE((*p).value() == 2);
        TEST_ENSURE(p2->value() == 3);
        TEST_ENSURE(p.valid() && p2.valid());
        TEST_ENSURE(!p.ownerEquals(p2));
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testDerivedDeleter()
{
    mini::int32 calls = 0;
    {
        SharedPtr<Base> base(new Derived(), BaseDeleter{&calls});
        TEST_ENSURE(base->value == 1);
        TEST_ENSURE(dynamicCast<Derived>(base).valid());

        base.reset(new Other(), BaseDeleter{&calls});
        TEST_ENSURE(calls == 1);
        TEST_ENSURE(dynamicCast<Other>(base).valid());
    }
    TEST_ENSURE(calls == 2);

    return 0;
}

constexpr mini::int32 testMakeShared()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        TEST_ENSURE(p.valid());
        TEST_ENSURE((*p).value() == 1);
        TEST_ENSURE(p->value() == 1);
        if !consteval {
            TEST_ENSURE(ctor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testMakeSharedPolicy()
{
    SharedPtr<NoDefaultObject> noDefault = makeShared<NoDefaultObject>(3);
    TEST_ENSURE(noDefault->value() == 3);

    return 0;
}

mini::int32 testMakeSharedAlignment()
{
    SharedPtr<OveralignedObject> overaligned = makeShared<OveralignedObject>(4);
    SharedPtr<OveralignedObject> allocated = allocateShared<OveralignedObject>(CountingAlloc<OveralignedObject>{}, 5);

    TEST_ENSURE(overaligned->value() == 4);
    TEST_ENSURE(allocated->value() == 5);
    TEST_ENSURE(reinterpret_cast<mini::size_t>(overaligned.get()) % alignof(OveralignedObject) == 0);
    TEST_ENSURE(reinterpret_cast<mini::size_t>(allocated.get()) % alignof(OveralignedObject) == 0);

    return 0;
}

mini::int32 testMakeSharedForwarding()
{
    initializeCounter();
    {
        SharedPtr<MoveOnlyObject> moveOnly = makeShared<MoveOnlyObject>(MoveOnlyObject(3));
        TEST_ENSURE(moveOnly->value() == 3);
        TEST_ENSURE(moveCtor == 1);
        TEST_ENSURE(copyCtor == 0);

        CopyableObject source(4);
        SharedPtr<CopyableObject> copied = makeShared<CopyableObject>(source);
        TEST_ENSURE(copied->value() == 4);
        TEST_ENSURE(copyCtor == 1);

        SharedPtr<CopyableObject> moved = makeShared<CopyableObject>(mini::moveCast(source));
        TEST_ENSURE(moved->value() == 4);
        TEST_ENSURE(moveCtor == 2);
        TEST_ENSURE(copyCtor == 1);
    }
    TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);

    return 0;
}

constexpr mini::int32 testCopyCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        SharedPtr<CopyableObject> copied(p);
        {
            SharedPtr<CopyableObject> scoped(p);
            TEST_ENSURE(scoped.ownerEquals(p));
        }
        TEST_ENSURE(copied.equals(p) && copied.ownerEquals(p));
        TEST_ENSURE(copied->value() == 1);
        if !consteval {
            TEST_ENSURE(copyCtor == 0 && moveCtor == 0 && dtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testMoveCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        SharedPtr<CopyableObject> copied(p);

        SharedPtr<CopyableObject> moved(mini::moveCast(copied));
        TEST_ENSURE(moved.equals(p) && moved.ownerEquals(p));
        TEST_ENSURE(!copied.valid());
        TEST_ENSURE(copied == nullptr);
        if !consteval {
            TEST_ENSURE(copyCtor == 0 && moveCtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testConvertCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        SharedPtr<CopyableObject> moved(p);

        SharedPtr<CopyableObject const> converted(p);
        TEST_ENSURE(converted.ownerEquals(p));
        TEST_ENSURE(converted.get() == p.get());

        SharedPtr<CopyableObject const> convertedMove(mini::moveCast(moved));
        TEST_ENSURE(convertedMove.ownerEquals(p));
        TEST_ENSURE(!moved.valid());
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testRefCount()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        {
            SharedPtr<CopyableObject> p2 = p;
            SharedPtr<CopyableObject> p3 = p2;
            p3.reset();
            TEST_ENSURE(!p3.valid());
            TEST_ENSURE(p2->value() == 1);
            if !consteval {
                TEST_ENSURE(dtor == 0);
            }
        }
        TEST_ENSURE(p->value() == 1);

        SharedPtr<CopyableObject> p4 = mini::moveCast(p);
        TEST_ENSURE(p4->value() == 1);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        p4.reset();
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testReset()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(5);

        p.reset(new CopyableObject(4));
        TEST_ENSURE(p->value() == 4);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p.reset();
        TEST_ENSURE(!p.valid());
        if !consteval {
            TEST_ENSURE(dtor == 2);
        }

        p.reset();
        TEST_ENSURE(!p.valid());
        if !consteval {
            TEST_ENSURE(dtor == 2);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

mini::int32 testResetDeleter()
{
    initializeCounter();
    CountingDeleter::calls = 0;
    {
        SharedPtr<CopyableObject> p(new CopyableObject(5), CountingDeleter{});
        SharedPtr<CopyableObject> p2 = p;

        p.reset(new CopyableObject(4), CountingDeleter{});
        TEST_ENSURE(CountingDeleter::calls == 0);
        TEST_ENSURE(p->value() == 4);
        TEST_ENSURE(p2->value() == 5);

        p2.reset();
        TEST_ENSURE(CountingDeleter::calls == 1);
        TEST_ENSURE(dtor == 1);

        p.reset(new CopyableObject(6));
        TEST_ENSURE(CountingDeleter::calls == 2);
        TEST_ENSURE(p->value() == 6);
    }
    TEST_ENSURE(ctor == 3 && dtor == 3);
    TEST_ENSURE(CountingDeleter::calls == 2);

    return 0;
}

constexpr mini::int32 testNullAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);

        p = nullptr;
        TEST_ENSURE(!p.valid());
        TEST_ENSURE(p == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testCopyAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
        SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);
        SharedPtr<CopyableObject> old = p;

        p = p2;
        TEST_ENSURE(p.equals(p2));
        TEST_ENSURE(p.ownerEquals(p2));
        TEST_ENSURE(!p.ownerEquals(old));
        TEST_ENSURE(p->value() == 8);

        old.reset();
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p = makeLvalueReference(p);
        TEST_ENSURE(p.valid());
        TEST_ENSURE(p->value() == 8);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testMoveAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
        SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);

        p = mini::moveCast(p2);
        TEST_ENSURE(p->value() == 8);
        TEST_ENSURE(!p2.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p = mini::moveCast(p2);
        TEST_ENSURE(!p.valid());
        if !consteval {
            TEST_ENSURE(dtor == 2);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testConvertAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
        SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);

        SharedPtr<CopyableObject const> constPtr;
        constPtr = p;
        TEST_ENSURE(constPtr.ownerEquals(p));

        constPtr = mini::moveCast(p2);
        TEST_ENSURE(!p2.valid());
        TEST_ENSURE(constPtr->value() == 8);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        SharedPtr<CopyableObject> shared = p;
        SharedPtr<CopyableObject const> sameOwner = p;
        sameOwner = mini::moveCast(shared);
        TEST_ENSURE(!shared.valid());
        TEST_ENSURE(sameOwner.equals(p));

        p.reset();
        TEST_ENSURE(sameOwner->value() == 7);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        sameOwner.reset();
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testSwap()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
        SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);
        SharedPtr<CopyableObject> lhs = p;
        SharedPtr<CopyableObject> rhs = p2;

        lhs.swap(rhs);
        TEST_ENSURE(lhs.ownerEquals(p2));
        TEST_ENSURE(rhs.ownerEquals(p));
        TEST_ENSURE(lhs->value() == 8);
        TEST_ENSURE(rhs->value() == 7);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testAllocatorCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        CountingAlloc<CopyableObject> alloc;
        SharedPtr<CopyableObject> p(new CopyableObject(1), mini::DefaultDeleter<CopyableObject>{}, alloc);
        TEST_ENSURE(p->value() == 1);
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }

        WeakPtr<CopyableObject> weak = p;
        SharedPtr<CopyableObject> copied = p;
        p.reset();
        TEST_ENSURE(copied->value() == 1);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        copied.reset();
        TEST_ENSURE(!weak.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
            TEST_ENSURE(aliveAllocCount == 1);
        }

        weak.reset();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testAllocateShared()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = allocateShared<CopyableObject>(CountingAlloc<CopyableObject>{}, 1);
        TEST_ENSURE(p->value() == 1);
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 1);
        }

        WeakPtr<CopyableObject> weak = p;
        SharedPtr<CopyableObject> copied = p;
        p.reset();
        TEST_ENSURE(copied->value() == 1);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        copied.reset();
        TEST_ENSURE(!weak.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
            TEST_ENSURE(aliveAllocCount == 1);
        }

        weak.reset();
        if !consteval {
            TEST_ENSURE(aliveAllocCount == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testResetAllocator()
{
    if !consteval {
        initializeCounter();
    }
    {
        CountingAlloc<CopyableObject> alloc;
        SharedPtr<CopyableObject> p(new CopyableObject(5), mini::DefaultDeleter<CopyableObject>{}, alloc);
        WeakPtr<CopyableObject> weak = p;

        p.reset(new CopyableObject(4), mini::DefaultDeleter<CopyableObject>{}, alloc);
        TEST_ENSURE(p->value() == 4);
        TEST_ENSURE(!weak.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
            TEST_ENSURE(aliveAllocCount == 2);
        }

        weak.reset();
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

mini::int32 testCustomAllocator()
{
    initializeCounter();
    {
        auto alloc = TestAlloc<CopyableObject>{};
        SharedPtr<CopyableObject> p(new CopyableObject(1), TestDealloc<CopyableObject>{}, alloc);
        SharedPtr<CopyableObject> q = allocateShared<CopyableObject>(alloc, 2);
        TEST_ENSURE(p->value() == 1);
        TEST_ENSURE(q->value() == 2);

        p.reset(new CopyableObject(3), TestDealloc<CopyableObject>{}, alloc);
        TEST_ENSURE(p->value() == 3);
        TEST_ENSURE(dtor == 1);
    }
    TEST_ENSURE(ctor == 3 && dtor == 3);

    return 0;
}

mini::int32 testAliasCtor()
{
    initializeCounter();
    {
        auto noopDeleter = [](auto) noexcept { };

        SharedPtr<CopyableObject> owner(new CopyableObject(1), TestDealloc<CopyableObject>{});
        SharedPtr<mini::int32> member(owner, owner->resource());
        SharedPtr<mini::int32> unowned(owner->resource(), mini::moveCast(noopDeleter));

        TEST_ENSURE(member.ownerEquals(owner));
        TEST_ENSURE(member.get() == unowned.get());
        TEST_ENSURE(!member.ownerEquals(unowned));
        TEST_ENSURE(*member == 1);

        owner.reset();
        TEST_ENSURE(dtor == 0);
        TEST_ENSURE(*member == 1);

        member.reset();
        TEST_ENSURE(dtor == 1);
    }
    TEST_ENSURE(ctor == 1 && dtor == 1);

    return 0;
}

constexpr mini::int32 testAliasConstCtor()
{
    SharedPtr<Derived> const owner = makeShared<Derived>();
    Derived other;

    SharedPtr<Base> alias(owner, &other);
    TEST_ENSURE(alias.get() == &other);
    TEST_ENSURE(alias.ownerEquals(owner));
    TEST_ENSURE(!alias.equals(owner));
    TEST_ENSURE(alias != owner);

    SharedPtr<Base const> constAlias(owner, owner.get());
    TEST_ENSURE(constAlias.equals(owner));
    TEST_ENSURE(constAlias == owner);

    return 0;
}

mini::int32 testAliasMoveCtor()
{
    initializeCounter();
    {
        SharedPtr<CopyableObject> empty;
        SharedPtr<mini::int32> fromEmpty(mini::moveCast(empty), nullptr);
        TEST_ENSURE(!fromEmpty.valid());

        SharedPtr<CopyableObject> owner = makeShared<CopyableObject>(9);
        mini::int32* resource = owner->resource();
        SharedPtr<mini::int32> movedAlias(mini::moveCast(owner), resource);
        TEST_ENSURE(!owner.valid());
        TEST_ENSURE(movedAlias.get() == resource);
        TEST_ENSURE(*movedAlias == 9);
        TEST_ENSURE(dtor == 0);

        movedAlias.reset();
        TEST_ENSURE(dtor == 1);
    }
    TEST_ENSURE(ctor == 1 && dtor == 1);

    return 0;
}

constexpr mini::int32 testStaticCast()
{
    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;
    TEST_ENSURE(base.ownerEquals(derived));
    TEST_ENSURE(base->value == 1);

    SharedPtr<Derived> down = staticCast<Derived>(base);
    TEST_ENSURE(down.equals(derived));
    TEST_ENSURE(down->extra == 2);

    TEST_ENSURE(base == staticCast<Base const>(base));

    SharedPtr<Base> up = staticCast<Base>(mini::moveCast(down));
    TEST_ENSURE(up.equals(base));
    TEST_ENSURE(!down.valid());

    return 0;
}

constexpr mini::int32 testDynamicCast()
{
    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;

    SharedPtr<Derived> down = dynamicCast<Derived>(base);
    TEST_ENSURE(down.equals(derived));
    TEST_ENSURE(down.ownerEquals(base));

    SharedPtr<Other> wrongDown = dynamicCast<Other>(base);
    TEST_ENSURE(!wrongDown.valid());
    TEST_ENSURE(wrongDown == nullptr);

    SharedPtr<Base> baseCopy = base;
    SharedPtr<Derived> movedDown = dynamicCast<Derived>(mini::moveCast(baseCopy));
    TEST_ENSURE(movedDown.equals(derived));
    TEST_ENSURE(!baseCopy.valid());

    SharedPtr<Base> kept = base;
    SharedPtr<Other> failedMove = dynamicCast<Other>(mini::moveCast(kept));
    TEST_ENSURE(!failedMove.valid());
    TEST_ENSURE(kept.equals(base));

    return 0;
}

constexpr mini::int32 testConstCast()
{
    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;

    SharedPtr<Base const> constBase = base;
    SharedPtr<Base> mutableBase = constCast<Base>(constBase);
    TEST_ENSURE(mutableBase.equals(base));
    mutableBase->value = 10;
    TEST_ENSURE(derived->value == 10);

    SharedPtr<Base const> constMoved = base;
    SharedPtr<Base> movedBase = constCast<Base>(mini::moveCast(constMoved));
    TEST_ENSURE(movedBase.equals(base));
    TEST_ENSURE(!constMoved.valid());

    return 0;
}

mini::int32 testReinterpretCast()
{
    initializeCounter();
    {
        SharedPtr<CopyableObject> owner(new CopyableObject(1), TestDealloc<CopyableObject>{});
        SharedPtr<mini::uint8> member(owner, reinterpret_cast<mini::uint8*>(owner.get()));
        TEST_ENSURE(owner.equals(reinterpretCast<CopyableObject>(member)));

        SharedPtr<Derived> derived = makeShared<Derived>();
        SharedPtr<Base> base = derived;
        SharedPtr<Derived> moved = reinterpretCast<Derived>(mini::moveCast(base));
        TEST_ENSURE(moved.equals(derived));
        TEST_ENSURE(!base.valid());
    }
    TEST_ENSURE(ctor == 1 && dtor == 1);

    return 0;
}

constexpr mini::int32 testDerivedPointer()
{
    SharedPtr<Base> base(new Derived());
    TEST_ENSURE(base->value == 1);
    TEST_ENSURE(dynamicCast<Derived>(base).valid());

    base.reset(new Other());
    TEST_ENSURE(!dynamicCast<Derived>(base).valid());
    TEST_ENSURE(dynamicCast<Other>(base).valid());

    return 0;
}

constexpr mini::int32 testEqual()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
    SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);
    SharedPtr<CopyableObject> empty;

    TEST_ENSURE(p == p);
    TEST_ENSURE(p != p2);
    TEST_ENSURE(empty == nullptr);
    TEST_ENSURE(nullptr == empty);
    TEST_ENSURE(p != nullptr);
    TEST_ENSURE(nullptr != p);
    TEST_ENSURE(p.equals(p));
    TEST_ENSURE(!p.equals(p2));

    SharedPtr<CopyableObject const> constP = p;
    TEST_ENSURE(constP == p);
    TEST_ENSURE(constP != p2);
    TEST_ENSURE(constP.equals(p));

    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;
    SharedPtr<Base> otherBase = makeShared<Derived>();
    TEST_ENSURE(base == derived);
    TEST_ENSURE(derived == base);
    TEST_ENSURE(otherBase != derived);
    TEST_ENSURE(derived != otherBase);

    return 0;
}

constexpr mini::int32 testOwnerEquals()
{
    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
    SharedPtr<CopyableObject> p2 = makeShared<CopyableObject>(8);
    SharedPtr<CopyableObject> copied = p;

    TEST_ENSURE(p.ownerEquals(copied));
    TEST_ENSURE(!p.ownerEquals(p2));
    TEST_ENSURE(SharedPtr<CopyableObject>().ownerEquals(SharedPtr<CopyableObject>()));

    return 0;
}

mini::int32 testNullCompare()
{
    SharedPtr<CopyableObject> empty;
    TEST_ENSURE((empty <=> nullptr) == 0);
    TEST_ENSURE((nullptr <=> empty) == 0);
    TEST_ENSURE(!(empty < nullptr));
    TEST_ENSURE(!(nullptr < empty));
    TEST_ENSURE(empty <= nullptr);
    TEST_ENSURE(nullptr >= empty);

    SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
    TEST_ENSURE(p != nullptr);
    TEST_ENSURE((nullptr <=> p) != 0);
    TEST_ENSURE((p < nullptr) != (p > nullptr));
    TEST_ENSURE((nullptr < p) == (p > nullptr));
    TEST_ENSURE(((nullptr <=> p) < 0) == ((p <=> nullptr) > 0));

    return 0;
}

mini::int32 testCompare()
{
    SharedPtr<Derived> derived = makeShared<Derived>();
    SharedPtr<Base> base = derived;
    SharedPtr<Base> otherBase = makeShared<Derived>();

    TEST_ENSURE((base <=> derived) == 0);
    TEST_ENSURE((derived <=> base) == 0);
    TEST_ENSURE((otherBase <=> derived) != 0);
    TEST_ENSURE((otherBase < derived) != (otherBase > derived));
    TEST_ENSURE((otherBase < derived) == (derived > otherBase));

    SharedPtr<Base const> constBase = base;
    TEST_ENSURE((constBase <=> derived) == 0);
    TEST_ENSURE(constBase <= derived && constBase >= derived);

    return 0;
}

constexpr mini::int32 testPtrView()
{
    SharedPtr<Derived> p = makeShared<Derived>();
    PtrView<Derived> view = p;
    PtrView<Base> baseView = p;
    PtrView<Base const> constView = p;

    TEST_ENSURE(view.get() == p.get());
    TEST_ENSURE(baseView.get() == p.get());
    TEST_ENSURE(constView.get() == p.get());
    TEST_ENSURE(view.valid());

    SharedPtr<Derived> empty;
    PtrView<Derived> emptyView = empty;
    TEST_ENSURE(!emptyView.valid());
    TEST_ENSURE(emptyView.get() == nullptr);

    return 0;
}

constexpr mini::int32 testValid()
{
    SharedPtr<ValidatableObject> p = makeShared<ValidatableObject>();
    TEST_ENSURE(p.valid());
    TEST_ENSURE(p != nullptr);

    p->alive = false;
    TEST_ENSURE(!p.valid());
    TEST_ENSURE(p != nullptr);
    TEST_ENSURE(p.get() != nullptr);

    p.reset();
    TEST_ENSURE(!p.valid());
    TEST_ENSURE(p == nullptr);

    return 0;
}

constexpr mini::int32 testNullPointerOwned()
{
    mini::int32 calls = 0;
    {
        SharedPtr<Base> p(static_cast<Base*>(nullptr), BaseDeleter{&calls});
        TEST_ENSURE(!p.valid());
        TEST_ENSURE(p == nullptr);
        TEST_ENSURE(!p.ownerEquals(SharedPtr<Base>()));

        SharedPtr<Base> copied = p;
        TEST_ENSURE(copied.ownerEquals(p));

        p.reset();
        TEST_ENSURE(calls == 0);
    }
    TEST_ENSURE(calls == 1);

    SharedPtr<CopyableObject> owned(static_cast<CopyableObject*>(nullptr));
    TEST_ENSURE(!owned.valid());
    TEST_ENSURE(owned.get() == nullptr);
    owned.reset();
    TEST_ENSURE(!owned.valid());

    return 0;
}

constexpr mini::int32 testEmptyCopyMove()
{
    SharedPtr<CopyableObject> empty;

    SharedPtr<CopyableObject> copied(empty);
    TEST_ENSURE(!copied.valid());
    TEST_ENSURE(copied.ownerEquals(empty));

    SharedPtr<CopyableObject> moved(mini::moveCast(copied));
    TEST_ENSURE(!moved.valid());
    TEST_ENSURE(!copied.valid());

    SharedPtr<CopyableObject const> converted(empty);
    TEST_ENSURE(converted == nullptr);

    SharedPtr<CopyableObject const> convertedMove(mini::moveCast(moved));
    TEST_ENSURE(convertedMove == nullptr);
    TEST_ENSURE(convertedMove.ownerEquals(empty));

    return 0;
}

constexpr mini::int32 testAssignEmpty()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> empty;

        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(1);
        p = empty;
        TEST_ENSURE(!p.valid());
        TEST_ENSURE(p.ownerEquals(empty));
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        SharedPtr<CopyableObject const> constPtr = makeShared<CopyableObject>(2);
        constPtr = empty;
        TEST_ENSURE(!constPtr.valid());
        if !consteval {
            TEST_ENSURE(dtor == 2);
        }

        constPtr = makeShared<CopyableObject>(3);
        constPtr = SharedPtr<CopyableObject>();
        TEST_ENSURE(!constPtr.valid());
        if !consteval {
            TEST_ENSURE(dtor == 3);
        }

        SharedPtr<CopyableObject> other;
        other = empty;
        TEST_ENSURE(!other.valid());
        constPtr = empty;
        TEST_ENSURE(!constPtr.valid());
    }
    if !consteval {
        TEST_ENSURE(ctor == 3 && dtor == 3);
    }

    return 0;
}

constexpr mini::int32 testSwapEmpty()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(7);
        SharedPtr<CopyableObject> empty;

        empty.swap(p);
        TEST_ENSURE(!p.valid());
        TEST_ENSURE(p == nullptr);
        TEST_ENSURE(empty->value() == 7);

        empty.swap(p);
        TEST_ENSURE(!empty.valid());
        TEST_ENSURE(p->value() == 7);

        SharedPtr<CopyableObject> other;
        empty.swap(other);
        TEST_ENSURE(!empty.valid());
        TEST_ENSURE(!other.valid());
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

mini::int32 testResetEmpty()
{
    initializeCounter();
    {
        SharedPtr<CopyableObject> p;
        p.reset(new CopyableObject(1));
        TEST_ENSURE(p->value() == 1);
        TEST_ENSURE(p.valid());

        SharedPtr<CopyableObject> deleted;
        deleted.reset(new CopyableObject(2), TestDealloc<CopyableObject>{});
        TEST_ENSURE(deleted->value() == 2);

        SharedPtr<CopyableObject> counted;
        counted.reset(new CopyableObject(3), TestDealloc<CopyableObject>{}, CountingAlloc<CopyableObject>{});
        TEST_ENSURE(counted->value() == 3);
        TEST_ENSURE(aliveAllocCount == 1);

        SharedPtr<CopyableObject> empty;
        empty = nullptr;
        TEST_ENSURE(!empty.valid());
        empty.reset();
        TEST_ENSURE(!empty.valid());
        TEST_ENSURE(dtor == 0);
    }
    TEST_ENSURE(ctor == 3 && dtor == 3);
    TEST_ENSURE(aliveAllocCount == 0);

    return 0;
}

constexpr mini::int32 testCastEmpty()
{
    SharedPtr<Derived> empty;
    SharedPtr<Base> emptyBase;
    SharedPtr<Base const> emptyConst;

    TEST_ENSURE(dynamicCast<Derived>(emptyBase) == nullptr);
    TEST_ENSURE(staticCast<Base>(empty) == nullptr);
    TEST_ENSURE(constCast<Base>(emptyConst) == nullptr);
    TEST_ENSURE(reinterpretCast<Derived>(emptyBase) == nullptr);

    return 0;
}

constexpr mini::int32 testMoveAssignSelf()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(8);

        p = makeRvalueReference(p);
        TEST_ENSURE(p.valid());
        TEST_ENSURE(p->value() == 8);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        SharedPtr<CopyableObject> other = p;
        CopyableObject* raw = p.get();
        p = makeRvalueReference(p);
        TEST_ENSURE(p.get() == raw);
        TEST_ENSURE(p.ownerEquals(other));

        other.reset();
        TEST_ENSURE(p.valid());
        TEST_ENSURE(p->value() == 8);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        SharedPtr<CopyableObject> empty;
        empty = makeRvalueReference(empty);
        TEST_ENSURE(!empty.valid());

        WeakPtr<CopyableObject> weak = p;
        p = makeRvalueReference(p);
        TEST_ENSURE(weak.valid());
        p.reset();
        TEST_ENSURE(!weak.valid());
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testConvertAssignSameOwner()
{
    SharedPtr<Derived> owner = makeShared<Derived>();
    SharedPtr<Base> base = owner;
    Derived other;
    SharedPtr<Derived> alias(owner, &other);

    base = alias;
    TEST_ENSURE(base.get() == &other);
    TEST_ENSURE(base.equals(alias));
    TEST_ENSURE(base.ownerEquals(owner));

    base = owner;
    TEST_ENSURE(base.equals(owner));

    return 0;
}

constexpr mini::int32 testCopyAssignAlias()
{
    SharedPtr<Derived> owner = makeShared<Derived>();
    Derived first;
    Derived second;

    SharedPtr<Base> firstAlias(owner, &first);
    SharedPtr<Base> secondAlias(owner, &second);

    firstAlias = secondAlias;
    TEST_ENSURE(firstAlias.get() == &second);
    TEST_ENSURE(firstAlias.equals(secondAlias));

    SharedPtr<Base> thirdAlias(owner, &first);
    thirdAlias = mini::moveCast(secondAlias);
    TEST_ENSURE(thirdAlias.get() == &second);
    TEST_ENSURE(thirdAlias.ownerEquals(owner));
    TEST_ENSURE(!secondAlias.valid());

    return 0;
}

constexpr mini::int32 testCopyAssignFromOwnedMember()
{
    SharedPtr<Node> head = makeShared<Node>();
    head->value = 1;
    head->next = makeShared<Node>();
    head->next->value = 2;

    head = head->next;
    TEST_ENSURE(head.valid());
    TEST_ENSURE(head->value == 2);
    TEST_ENSURE(!head->next.valid());

    return 0;
}

constexpr mini::int32 testMoveAssignFromOwnedMember()
{
    SharedPtr<Node> head = makeShared<Node>();
    head->value = 1;
    head->next = makeShared<Node>();
    head->next->value = 2;

    head = mini::moveCast(head->next);
    TEST_ENSURE(head.valid());
    TEST_ENSURE(head->value == 2);
    TEST_ENSURE(!head->next.valid());

    return 0;
}

constexpr mini::int32 testLvalueDeleter()
{
    mini::int32 calls = 0;
    {
        BaseDeleter deleter{&calls};
        SharedPtr<Base> p(new Derived(), deleter);
        TEST_ENSURE(p->value == 1);

        p.reset(new Other(), deleter);
        TEST_ENSURE(calls == 1);

        BaseDeleter const constDeleter{&calls};
        SharedPtr<Base> q(new Derived(), constDeleter);
        TEST_ENSURE(q.valid());
    }
    TEST_ENSURE(calls == 3);

    return 0;
}

constexpr mini::int32 testMultipleInheritance()
{
    SharedPtr<Both> both = makeShared<Both>();

    SharedPtr<Right> right = both;
    TEST_ENSURE(right.get() == static_cast<Right*>(both.get()));
    TEST_ENSURE(right->right == 4);
    TEST_ENSURE(right == both);
    TEST_ENSURE(right.equals(both));
    TEST_ENSURE(right.ownerEquals(both));

    SharedPtr<Both> back = staticCast<Both>(right);
    TEST_ENSURE(back.equals(both));
    TEST_ENSURE(back->left == 3);

    SharedPtr<Right> moved;
    moved = mini::moveCast(back);
    TEST_ENSURE(moved.equals(right));
    TEST_ENSURE(!back.valid());

    both.reset();
    right.reset();
    TEST_ENSURE(moved->right == 4);

    return 0;
}

constexpr mini::int32 testCastOwnership()
{
    if !consteval {
        initializeCounter();
    }
    {
        SharedPtr<CopyableObject> p = makeShared<CopyableObject>(5);
        SharedPtr<CopyableObject const> constPtr = staticCast<CopyableObject const>(p);
        SharedPtr<CopyableObject> mutablePtr = constCast<CopyableObject>(constPtr);
        TEST_ENSURE(mutablePtr.equals(p));

        p.reset();
        constPtr.reset();
        TEST_ENSURE(mutablePtr->value() == 5);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        mutablePtr.reset();
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testNullCtor);
    TEST_CONSTEXPR(testPointerCtor);
    TEST_CONSTEXPR(testDerivedDeleter);
    TEST_CONSTEXPR(testMakeShared);
    TEST_CONSTEXPR(testMakeSharedPolicy);
    TEST_FUNCTION(testMakeSharedAlignment);
    TEST_FUNCTION(testMakeSharedForwarding);
    TEST_CONSTEXPR(testCopyCtor);
    TEST_CONSTEXPR(testMoveCtor);
    TEST_CONSTEXPR(testConvertCtor);

    TEST_CONSTEXPR(testRefCount);
    TEST_CONSTEXPR(testReset);
    TEST_FUNCTION(testResetDeleter);

    TEST_CONSTEXPR(testNullAssign);
    TEST_CONSTEXPR(testCopyAssign);
    TEST_CONSTEXPR(testMoveAssign);
    TEST_CONSTEXPR(testConvertAssign);
    TEST_CONSTEXPR(testSwap);

    TEST_CONSTEXPR(testAllocatorCtor);
    TEST_CONSTEXPR(testAllocateShared);
    TEST_CONSTEXPR(testResetAllocator);
    TEST_FUNCTION(testCustomAllocator);

    TEST_FUNCTION(testAliasCtor);
    TEST_CONSTEXPR(testAliasConstCtor);
    TEST_FUNCTION(testAliasMoveCtor);

    TEST_CONSTEXPR(testStaticCast);
    TEST_CONSTEXPR(testDynamicCast);
    TEST_CONSTEXPR(testConstCast);
    TEST_FUNCTION(testReinterpretCast);
    TEST_CONSTEXPR(testDerivedPointer);

    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testOwnerEquals);
    TEST_FUNCTION(testNullCompare);
    TEST_FUNCTION(testCompare);

    TEST_CONSTEXPR(testPtrView);
    TEST_CONSTEXPR(testValid);

    TEST_CONSTEXPR(testNullPointerOwned);
    TEST_CONSTEXPR(testEmptyCopyMove);
    TEST_CONSTEXPR(testAssignEmpty);
    TEST_CONSTEXPR(testSwapEmpty);
    TEST_FUNCTION(testResetEmpty);
    TEST_CONSTEXPR(testCastEmpty);

    TEST_CONSTEXPR(testMoveAssignSelf);
    TEST_CONSTEXPR(testConvertAssignSameOwner);
    TEST_CONSTEXPR(testCopyAssignAlias);
    TEST_CONSTEXPR(testCopyAssignFromOwnedMember);
    TEST_CONSTEXPR(testMoveAssignFromOwnedMember);

    TEST_CONSTEXPR(testLvalueDeleter);
    TEST_CONSTEXPR(testMultipleInheritance);
    TEST_CONSTEXPR(testCastOwnership);

    TEST_ENSURE(aliveAllocCount == 0);
    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
