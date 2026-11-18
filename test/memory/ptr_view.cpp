#include "test_macro.h"

import mini.test;

using mini::constCast;
using mini::dynamicCast;
using mini::PtrView;
using mini::reinterpretCast;
using mini::staticCast;

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

struct Other : Base { };

struct ValidatableObject {
    bool alive = true;

    [[nodiscard]] constexpr bool valid() const noexcept { return alive; }
};

constexpr mini::int32 testCtor()
{
    Derived object;

    PtrView<Derived> empty;
    PtrView<Derived> null(nullptr);
    TEST_ENSURE(empty.get() == nullptr && !empty.valid());
    TEST_ENSURE(null.get() == nullptr && !null.valid());

    PtrView<Derived> view(&object);
    TEST_ENSURE(view.get() == &object);
    TEST_ENSURE(view.valid());
    TEST_ENSURE(view->extra == 2);
    TEST_ENSURE((*view).value == 1);

    PtrView<Derived> copied(view);
    TEST_ENSURE(copied.get() == &object);

    PtrView<Base> fromRaw(&object);
    PtrView<Base> fromView(view);
    PtrView<Base const> fromConst(view);
    TEST_ENSURE(fromRaw.get() == &object);
    TEST_ENSURE(fromView.get() == &object);
    TEST_ENSURE(fromConst.get() == &object);

    return 0;
}

constexpr mini::int32 testConversion()
{
    Derived object;
    PtrView<Derived> view(&object);

    Derived* raw = view;
    Base* base = view;
    TEST_ENSURE(raw == &object);
    TEST_ENSURE(base == &object);

    void* opaque = view;
    TEST_ENSURE(opaque == static_cast<void*>(&object));

    PtrView<Derived> empty;
    Base* emptyBase = empty;
    TEST_ENSURE(emptyBase == nullptr);

    Derived const constObject;
    PtrView<Derived const> constView(&constObject);
    Derived const* constRaw = constView;
    TEST_ENSURE(constRaw == &constObject);

    return 0;
}

constexpr mini::int32 testReset()
{
    Derived object;
    PtrView<Base> view(&object);

    view.reset();
    TEST_ENSURE(view.get() == nullptr);
    TEST_ENSURE(!view.valid());

    view.reset(&object);
    TEST_ENSURE(view.get() == &object);

    return 0;
}

constexpr mini::int32 testAssign()
{
    Derived object;
    Derived other;
    PtrView<Base> view;

    view = &object;
    TEST_ENSURE(view.get() == &object);

    view = nullptr;
    TEST_ENSURE(view.get() == nullptr);

    PtrView<Derived> derivedView(&other);
    view = derivedView;
    TEST_ENSURE(view.get() == &other);

    PtrView<Base> copied;
    copied = view;
    TEST_ENSURE(copied.get() == &other);

    copied = makeLvalueReference(copied);
    TEST_ENSURE(copied.get() == &other);

    PtrView<Derived> emptyDerived;
    view = emptyDerived;
    TEST_ENSURE(view.get() == nullptr);

    return 0;
}

constexpr mini::int32 testAddress()
{
    Derived object;
    Derived other;
    PtrView<Derived> view(&object);

    Derived** address = view.address();
    TEST_ENSURE(*address == &object);

    *address = &other;
    TEST_ENSURE(view.get() == &other);

    Derived** overloaded = &view;
    TEST_ENSURE(overloaded == address);

    *overloaded = nullptr;
    TEST_ENSURE(view.get() == nullptr);

    return 0;
}

constexpr mini::int32 testValid()
{
    ValidatableObject object;
    PtrView<ValidatableObject> view(&object);
    TEST_ENSURE(view.valid());

    object.alive = false;
    TEST_ENSURE(!view.valid());
    TEST_ENSURE(view != nullptr);

    view = nullptr;
    TEST_ENSURE(!view.valid());

    return 0;
}

constexpr mini::int32 testEqual()
{
    Derived object;
    Derived other;
    PtrView<Derived> view(&object);
    PtrView<Derived> same(&object);
    PtrView<Derived> different(&other);
    PtrView<Base> base(&object);
    PtrView<Derived> empty;

    TEST_ENSURE(view == same);
    TEST_ENSURE(view != different);
    TEST_ENSURE(view == base);

    TEST_ENSURE(empty == nullptr);
    TEST_ENSURE(view != nullptr);

    return 0;
}

mini::int32 testCompare()
{
    Derived objects[2];
    PtrView<Derived> first(&objects[0]);
    PtrView<Derived> second(&objects[1]);
    PtrView<Base> base(&objects[0]);
    PtrView<Derived> empty;

    TEST_ENSURE(first < second);
    TEST_ENSURE((first <=> base) == 0);

    TEST_ENSURE((empty <=> nullptr) == 0);
    TEST_ENSURE((first <=> nullptr) != 0);

    return 0;
}

constexpr mini::int32 testCast()
{
    Derived derived;
    Other other;
    PtrView<Base> base(&derived);
    PtrView<Base> otherBase(&other);

    PtrView<Derived> down = staticCast<Derived>(base);
    TEST_ENSURE(down.get() == &derived);
    TEST_ENSURE(down->extra == 2);

    TEST_ENSURE(dynamicCast<Derived>(base).get() == &derived);
    TEST_ENSURE(dynamicCast<Derived>(otherBase).get() == nullptr);
    TEST_ENSURE(dynamicCast<Other>(otherBase).get() == &other);

    PtrView<Base const> constBase = base;
    PtrView<Base> mutableBase = constCast<Base>(constBase);
    mutableBase->value = 10;
    TEST_ENSURE(derived.value == 10);

    PtrView<Base const> upcast = staticCast<Base const>(down);
    TEST_ENSURE(upcast.get() == &derived);

    PtrView<Base> empty;
    TEST_ENSURE(dynamicCast<Derived>(empty).get() == nullptr);

    return 0;
}

mini::int32 testReinterpretCast()
{
    Derived derived;
    PtrView<Derived> view(&derived);

    PtrView<mini::uint8> bytes = reinterpretCast<mini::uint8>(view);
    TEST_ENSURE(static_cast<void*>(bytes.get()) == static_cast<void*>(&derived));
    TEST_ENSURE(reinterpretCast<Derived>(bytes) == view);

    return 0;
}

constexpr mini::int32 testNonOwning()
{
    if !consteval {
        initializeCounter();
    }

    CopyableObject object(3);
    {
        PtrView<CopyableObject> view(&object);
        PtrView<CopyableObject> copied = view;

        view.reset();
        TEST_ENSURE(view.get() == nullptr);
        TEST_ENSURE(copied.get() == &object);
        TEST_ENSURE(copied->value() == 3);

        (*copied) = CopyableObject(5);
        TEST_ENSURE(object.value() == 5);

        copied = nullptr;
    }
    TEST_ENSURE(object.value() == 5);
    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 1);
    }

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testCtor);
    TEST_CONSTEXPR(testConversion);
    TEST_CONSTEXPR(testReset);
    TEST_CONSTEXPR(testAssign);
    TEST_CONSTEXPR(testAddress);
    TEST_CONSTEXPR(testValid);

    TEST_CONSTEXPR(testEqual);
    TEST_FUNCTION(testCompare);

    TEST_CONSTEXPR(testCast);
    TEST_FUNCTION(testReinterpretCast);

    TEST_CONSTEXPR(testNonOwning);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
