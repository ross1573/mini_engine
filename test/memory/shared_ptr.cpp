#include "test_macro.h"

import mini.test;

using namespace mini;

static constexpr bool ConstexprSharedPtr()
{
    // TODO: MSVC bug
    //  destructor need to be "generated" before constant evaluation
    ConstexprFoo("string");

    SharedPtr<ConstexprFoo> p(new ConstexprFoo("string"));
    SharedPtr<ConstexprFoo> p2(p);
    {
        SharedPtr<ConstexprFoo> p3(p);
    }
    return p.Equals(p2) && p.OwnerEquals(p2);
}

int main()
{
    static_assert(ConstexprSharedPtr());

    SharedPtr<Foo> p;
    SharedPtr<Foo> p2(nullptr);
    SharedPtr<Foo> p3(nullptr, mini::DefaultDeleter<Foo>{});
    SharedPtr<Foo> p4(nullptr, [](NullptrT) {});
    SharedPtr<Foo> p5(nullptr, [](NullptrT) {}, mini::Allocator<NullptrT>{});
    SharedPtr<Foo> p6(new Foo("string1"));
    SharedPtr<Foo> p7(new Foo("string2"), FooDel{});
    SharedPtr<Foo> p10 = MakeShared<Foo>("string5");
    SharedPtr<BasicString<char>> p11(&p7->str, [](auto) {});
    SharedPtr<BasicString<char>> p12(p7, &p7->str);

    {
        auto alloc = FooAlloc{};
        SharedPtr<Foo> p8(new Foo("string3"), FooDel{}, alloc);
        SharedPtr<Foo> p9 = AllocateShared<Foo>(alloc, "string4");
    }
    InitializeCounter();
    {
        SharedPtr<Foo> f1(new Foo("String"));
        SharedPtr<Foo> f2(f1);
        SharedPtr<Foo> f3 = f2;
        f3.Reset();
        f3.Swap(f2);

        TEST_ENSURE(p2 == nullptr && nullptr == p2);
        TEST_ENSURE(!p2);
        TEST_ENSURE((p2 == p3) && (p3 == p4) && (p4 == p5));
        TEST_ENSURE(f1 == f3);

        TEST_ENSURE(p7 == StaticCast<Foo const>(p7));
        TEST_ENSURE(p7.Equals(ReinterpretCast<Foo>(p12)));
        TEST_ENSURE(p7.OwnerEquals(p12));
    }

    TEST_ENSURE(ctor == 1 && dtor == 1);
    TEST_ENSURE(debugAllocCnt == 0);

    return 0;
}