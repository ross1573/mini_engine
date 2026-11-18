#include "test_macro.h"

import mini.test;

using mini::DefaultDeleter;

#if MSVC
#  undef TEST_CONSTEXPR
#  define TEST_CONSTEXPR(func) TEST_FUNCTION(func)
#endif

struct Base {
    mini::int32 value = 1;

    constexpr virtual ~Base() = default;
};

struct Derived : Base {
    CopyableObject member{4};
};

constexpr mini::int32 testDefaultDeleter()
{
    if !consteval {
        initializeCounter();
    }

    DefaultDeleter<CopyableObject> deleter;
    deleter(new CopyableObject(1));
    deleter(nullptr);

    DefaultDeleter<CopyableObject> copied = deleter;
    copied(new CopyableObject(2));

    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

constexpr mini::int32 testDefaultDeleterPolymorphic()
{
    if !consteval {
        initializeCounter();
    }

    DefaultDeleter<Base> deleter;
    deleter(new Derived());
    deleter(new Base());

    if !consteval {
        TEST_ENSURE(ctor == 1 && dtor == 1);
    }

    return 0;
}

constexpr mini::int32 testDefaultDeleterQualified()
{
    if !consteval {
        initializeCounter();
    }

    DefaultDeleter<CopyableObject const> constDeleter;
    constDeleter(new CopyableObject const(1));

    DefaultDeleter<OveralignedObject> overalignedDeleter;
    overalignedDeleter(new OveralignedObject(2));

    if !consteval {
        TEST_ENSURE(ctor == 2 && dtor == 2);
    }

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testDefaultDeleter);
    TEST_CONSTEXPR(testDefaultDeleterPolymorphic);
    TEST_CONSTEXPR(testDefaultDeleterQualified);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
