#include "test_macro.h"

import mini.test;

using mini::DefaultDeleter;
using mini::makeUnique;
using mini::PtrView;
using mini::UniquePtr;

#if MSVC
#  undef TEST_CONSTEXPR
#  define TEST_CONSTEXPR(func) TEST_FUNCTION(func)
#endif

struct Base {
    mini::int32 value = 1;

    virtual ~Base() = default;
};

struct Derived : Base {
    mini::int32 extra = 2;
};

struct CountingDeleter {
    mini::int32* calls = nullptr;

    constexpr void operator()(CopyableObject* ptr) noexcept
    {
        if (ptr == nullptr) {
            return;
        }

        ++*calls;
        delete ptr;
    }
};

struct BaseCountingDeleter {
    mini::int32* calls = nullptr;

    constexpr void operator()(Base* ptr) noexcept
    {
        if (ptr == nullptr) {
            return;
        }

        ++*calls;
        delete ptr;
    }
};

struct ValidatableObject {
    bool alive = true;

    [[nodiscard]] constexpr bool valid() const noexcept { return alive; }
};

constexpr mini::int32 testCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> empty;
        UniquePtr<CopyableObject> null(nullptr);
        TEST_ENSURE(empty.get() == nullptr && !empty.valid());
        TEST_ENSURE(null.get() == nullptr && !null.valid());

        UniquePtr<CopyableObject> raw(new CopyableObject(1));
        TEST_ENSURE(raw.valid());
        TEST_ENSURE(raw->value() == 1);
        TEST_ENSURE((*raw).value() == 1);

        DefaultDeleter<CopyableObject> const deleter;
        UniquePtr<CopyableObject> copiedDeleter(new CopyableObject(2), deleter);
        UniquePtr<CopyableObject> movedDeleter(new CopyableObject(3), DefaultDeleter<CopyableObject>{});
        TEST_ENSURE(copiedDeleter->value() == 2);
        TEST_ENSURE(movedDeleter->value() == 3);
        if !consteval {
            TEST_ENSURE(ctor == 3 && dtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 3 && dtor == 3);
    }

    return 0;
}

constexpr mini::int32 testMakeUnique()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<OveralignedObject> overaligned = makeUnique<OveralignedObject>(3);
        TEST_ENSURE(overaligned->value() == 3);

        if !consteval {
            TEST_ENSURE(reinterpret_cast<mini::size_t>(overaligned.get()) % alignof(OveralignedObject) == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testMakeUniqueForward()
{
    if !consteval {
        initializeCounter();
    }
    {
        CopyableObject source(6);

        UniquePtr<CopyableObject> copied = makeUnique<CopyableObject>(source);
        TEST_ENSURE(copied->value() == 6);
        TEST_ENSURE(source.value() == 6);
        if !consteval {
            TEST_ENSURE(copyCtor == 1 && moveCtor == 0);
        }

        UniquePtr<CopyableObject> moved = makeUnique<CopyableObject>(mini::moveCast(source));
        TEST_ENSURE(moved->value() == 6);
        TEST_ENSURE(source.resource() == nullptr);
        if !consteval {
            TEST_ENSURE(copyCtor == 1 && moveCtor == 1);
        }

        UniquePtr<CopyableObject> defaulted = makeUnique<CopyableObject>();
        TEST_ENSURE(defaulted->value() == 0);
    }
    if !consteval {
        TEST_ENSURE(ctor + copyCtor + moveCtor == dtor);
    }

    return 0;
}

constexpr mini::int32 testDeleter()
{
    mini::int32 calls = 0;
    {
        UniquePtr<CopyableObject, CountingDeleter> p(new CopyableObject(1), CountingDeleter{&calls});
        TEST_ENSURE(p->value() == 1);

        p.reset(new CopyableObject(2));
        TEST_ENSURE(calls == 1);
        TEST_ENSURE(p->value() == 2);

        p.reset();
        TEST_ENSURE(calls == 2);
        TEST_ENSURE(!p.valid());

        CountingDeleter const deleter{&calls};
        UniquePtr<CopyableObject, CountingDeleter> copiedDeleter(new CopyableObject(3), deleter);
        UniquePtr<CopyableObject, CountingDeleter> moved(new CopyableObject(4), CountingDeleter{&calls});
        UniquePtr<CopyableObject, CountingDeleter> target(mini::moveCast(moved));
        TEST_ENSURE(calls == 2);
        TEST_ENSURE(moved.get() == nullptr);

        target = mini::moveCast(copiedDeleter);
        TEST_ENSURE(calls == 3);
        TEST_ENSURE(target->value() == 3);
    }
    TEST_ENSURE(calls == 4);

    return 0;
}

constexpr mini::int32 testMoveCtor()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
        CopyableObject* raw = p.get();

        UniquePtr<CopyableObject> moved(mini::moveCast(p));
        TEST_ENSURE(moved.get() == raw);
        TEST_ENSURE(p.get() == nullptr);
        TEST_ENSURE(!p.valid());

        UniquePtr<CopyableObject> fromEmpty(mini::moveCast(p));
        TEST_ENSURE(fromEmpty.get() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testConvertCtor()
{
    UniquePtr<Derived> derived = makeUnique<Derived>();
    Derived* raw = derived.get();

    UniquePtr<Base> base(mini::moveCast(derived));
    TEST_ENSURE(base.get() == raw);
    TEST_ENSURE(derived.get() == nullptr);
    TEST_ENSURE(base->value == 1);

    UniquePtr<Base const> constBase = mini::moveCast(base);
    TEST_ENSURE(constBase.get() == raw);
    TEST_ENSURE(base.get() == nullptr);

    return 0;
}

constexpr mini::int32 testConvertCtorDeleter()
{
    mini::int32 calls = 0;
    {
        UniquePtr<Derived, BaseCountingDeleter> derived(new Derived(), BaseCountingDeleter{&calls});
        Derived* raw = derived.get();

        UniquePtr<Base, BaseCountingDeleter> base(mini::moveCast(derived));
        TEST_ENSURE(base.get() == raw);
        TEST_ENSURE(derived.get() == nullptr);
        TEST_ENSURE(calls == 0);
    }
    TEST_ENSURE(calls == 1);

    return 0;
}

constexpr mini::int32 testDetach()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
        CopyableObject* raw = p.get();

        CopyableObject* detached = p.detach();
        TEST_ENSURE(detached == raw);
        TEST_ENSURE(p.get() == nullptr);
        TEST_ENSURE(p.detach() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        delete detached;
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
        UniquePtr<CopyableObject> p;
        p.reset(new CopyableObject(1));
        TEST_ENSURE(p->value() == 1);

        p.reset(new CopyableObject(2));
        TEST_ENSURE(p->value() == 2);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p.reset(nullptr);
        TEST_ENSURE(p.get() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 2);
        }

        p.reset(new CopyableObject(3));
        p.reset();
        TEST_ENSURE(!p.valid());
        if !consteval {
            TEST_ENSURE(dtor == 3);
        }
    }
    if !consteval {
        TEST_ENSURE(ctor == 3 && dtor == 3);
    }

    UniquePtr<Base> base = makeUnique<Base>();
    Derived* derived = new Derived();
    base.reset(derived);
    TEST_ENSURE(base.get() == derived);
    TEST_ENSURE(base->value == 1);

    return 0;
}

constexpr mini::int32 testSwap()
{
    mini::int32 calls = 0;
    mini::int32 otherCalls = 0;
    {
        UniquePtr<CopyableObject, CountingDeleter> p(new CopyableObject(1), CountingDeleter{&calls});
        UniquePtr<CopyableObject, CountingDeleter> q(new CopyableObject(2), CountingDeleter{&otherCalls});
        CopyableObject* rawP = p.get();
        CopyableObject* rawQ = q.get();

        p.swap(q);
        TEST_ENSURE(p.get() == rawQ);
        TEST_ENSURE(q.get() == rawP);
        TEST_ENSURE(p->value() == 2);

        p.reset();
        TEST_ENSURE(otherCalls == 1);
        TEST_ENSURE(calls == 0);

        p.swap(q);
        TEST_ENSURE(p->value() == 1);
        TEST_ENSURE(q.get() == nullptr);

        p.swap(p);
        TEST_ENSURE(p.get() == rawP);
        TEST_ENSURE(calls == 0);
    }
    TEST_ENSURE(calls == 1);
    TEST_ENSURE(otherCalls == 1);

    return 0;
}

constexpr mini::int32 testNullAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
        p = nullptr;
        TEST_ENSURE(p.get() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p = nullptr;
        TEST_ENSURE(p == nullptr);
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testMoveAssign()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
        UniquePtr<CopyableObject> q = makeUnique<CopyableObject>(2);
        CopyableObject* raw = q.get();

        p = mini::moveCast(q);
        TEST_ENSURE(p.get() == raw);
        TEST_ENSURE(q.get() == nullptr);
        if !consteval {
            TEST_ENSURE(dtor == 1);
        }

        p = mini::moveCast(q);
        TEST_ENSURE(p.get() == nullptr);
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
    UniquePtr<Base> base = makeUnique<Base>();
    UniquePtr<Derived> derived = makeUnique<Derived>();
    Derived* raw = derived.get();

    base = mini::moveCast(derived);
    TEST_ENSURE(base.get() == raw);
    TEST_ENSURE(derived.get() == nullptr);

    UniquePtr<Base const> constBase;
    constBase = mini::moveCast(base);
    TEST_ENSURE(constBase.get() == raw);
    TEST_ENSURE(base.get() == nullptr);

    return 0;
}

constexpr mini::int32 testConvertAssignDeleter()
{
    mini::int32 calls = 0;
    mini::int32 otherCalls = 0;
    {
        UniquePtr<Base, BaseCountingDeleter> base(new Base(), BaseCountingDeleter{&calls});
        UniquePtr<Derived, BaseCountingDeleter> derived(new Derived(), BaseCountingDeleter{&otherCalls});
        Derived* raw = derived.get();

        base = mini::moveCast(derived);
        TEST_ENSURE(calls == 1);
        TEST_ENSURE(otherCalls == 0);
        TEST_ENSURE(base.get() == raw);
        TEST_ENSURE(derived.get() == nullptr);
        TEST_ENSURE(base->value == 1);

        UniquePtr<Derived, BaseCountingDeleter> empty(nullptr, BaseCountingDeleter{&calls});
        base = mini::moveCast(empty);
        TEST_ENSURE(otherCalls == 1);
        TEST_ENSURE(base.get() == nullptr);
    }
    TEST_ENSURE(calls == 1);
    TEST_ENSURE(otherCalls == 1);

    return 0;
}

constexpr mini::int32 testEqual()
{
    UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
    UniquePtr<CopyableObject> q = makeUnique<CopyableObject>(1);
    UniquePtr<CopyableObject> empty;
    UniquePtr<CopyableObject const> otherEmpty;

    TEST_ENSURE(p == p);
    TEST_ENSURE(p != q);
    TEST_ENSURE(empty == otherEmpty);
    TEST_ENSURE(empty == nullptr);
    TEST_ENSURE(p != nullptr);

    UniquePtr<Derived> derived = makeUnique<Derived>();
    UniquePtr<Base> base = makeUnique<Base>();
    TEST_ENSURE(base != derived);

    return 0;
}

mini::int32 testCompare()
{
    UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(1);
    UniquePtr<CopyableObject> q = makeUnique<CopyableObject>(2);
    UniquePtr<CopyableObject> empty;

    TEST_ENSURE((p <=> p) == 0);
    TEST_ENSURE((p <=> q) != 0);
    TEST_ENSURE((p < q) != (p > q));

    TEST_ENSURE((empty <=> nullptr) == 0);
    TEST_ENSURE((p <=> nullptr) != 0);
    TEST_ENSURE((p < nullptr) != (p > nullptr));

    UniquePtr<Derived> derived = makeUnique<Derived>();
    UniquePtr<Base> base = makeUnique<Base>();
    TEST_ENSURE((base <=> derived) != 0);

    return 0;
}

constexpr mini::int32 testPtrView()
{
    UniquePtr<Derived> p = makeUnique<Derived>();
    PtrView<Derived> view = p;
    PtrView<Base> baseView = p;

    TEST_ENSURE(view.get() == p.get());
    TEST_ENSURE(baseView.get() == p.get());

    UniquePtr<Derived> empty;
    PtrView<Derived> emptyView = empty;
    TEST_ENSURE(emptyView.get() == nullptr);

    return 0;
}

constexpr mini::int32 testValid()
{
    UniquePtr<ValidatableObject> p = makeUnique<ValidatableObject>();
    TEST_ENSURE(p.valid());

    p->alive = false;
    TEST_ENSURE(!p.valid());
    TEST_ENSURE(p != nullptr);

    p.reset();
    TEST_ENSURE(!p.valid());

    return 0;
}

constexpr mini::int32 testMoveAssignSelf()
{
    if !consteval {
        initializeCounter();
    }
    {
        UniquePtr<CopyableObject> p = makeUnique<CopyableObject>(2);
        CopyableObject* raw = p.get();

        p = makeRvalueReference(p);
        TEST_ENSURE(p.get() == raw);
        TEST_ENSURE(p->value() == 2);
        if !consteval {
            TEST_ENSURE(dtor == 0);
        }

        UniquePtr<CopyableObject> empty;
        empty = makeRvalueReference(empty);
        TEST_ENSURE(!empty.valid());
    }
    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testMoveAssignSelfDeleter()
{
    mini::int32 calls = 0;
    {
        UniquePtr<CopyableObject, CountingDeleter> p(new CopyableObject(5), CountingDeleter{&calls});
        CopyableObject* raw = p.get();

        p = makeRvalueReference(p);
        TEST_ENSURE(calls == 0);
        TEST_ENSURE(p.get() == raw);
        TEST_ENSURE(p->value() == 5);

        p.reset();
        TEST_ENSURE(calls == 1);
    }
    TEST_ENSURE(calls == 1);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testCtor);
    TEST_CONSTEXPR(testMakeUnique);
    TEST_CONSTEXPR(testMakeUniqueForward);
    TEST_CONSTEXPR(testDeleter);
    TEST_CONSTEXPR(testMoveCtor);
    TEST_CONSTEXPR(testConvertCtor);
    TEST_CONSTEXPR(testConvertCtorDeleter);

    TEST_CONSTEXPR(testDetach);
    TEST_CONSTEXPR(testReset);
    TEST_CONSTEXPR(testSwap);

    TEST_CONSTEXPR(testNullAssign);
    TEST_CONSTEXPR(testMoveAssign);
    TEST_CONSTEXPR(testConvertAssign);
    TEST_CONSTEXPR(testConvertAssignDeleter);

    TEST_CONSTEXPR(testEqual);
    TEST_FUNCTION(testCompare);

    TEST_CONSTEXPR(testPtrView);
    TEST_CONSTEXPR(testValid);

    TEST_CONSTEXPR(testMoveAssignSelf);
    TEST_CONSTEXPR(testMoveAssignSelfDeleter);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
