#include "test_macro.h"

import mini.test;

using mini::allocateShared;
using mini::makeShared;
using mini::SharedPtr;

constexpr bool ConstexprSharedPtr()
{
    SharedPtr<ConstexprObject> p(new ConstexprObject("string"));
    SharedPtr<ConstexprObject> p2(p);
    {
        SharedPtr<ConstexprObject> p3(p);
    }

    SharedPtr<ConstexprObject> p4 = makeShared<ConstexprObject>("string");
    SharedPtr<ConstexprObject> p5(p4);
    {
        SharedPtr<ConstexprObject> p6(p4);
    }

    return (p.equals(p2) && p.ownerEquals(p2)) && (p4.equals(p5) && p4.ownerEquals(p5));
}

int main()
{
#if !MSVC // fuck you ms, this piece of crap cannot handle constexpr properly
    static_assert(ConstexprSharedPtr());
#endif

    SharedPtr<TestObject> p;
    SharedPtr<TestObject> p2(nullptr);
    SharedPtr<TestObject> p3(nullptr, mini::DefaultDeleter<TestObject>{});
    SharedPtr<TestObject> p4(nullptr, [](nullptr_t) { });
    SharedPtr<TestObject> p5(nullptr, [](nullptr_t) { }, mini::Allocator<nullptr_t>{});
    SharedPtr<TestObject> p6(new TestObject("string1"));
    SharedPtr<TestObject> p7(new TestObject("string2"), FooDel{});
    SharedPtr<TestObject> p10 = makeShared<TestObject>("string5");
    SharedPtr<mini::BasicString<char>> p11(&p7->str, [](auto) { });
    SharedPtr<mini::BasicString<char>> p12(p7, &p7->str);

    {
        auto alloc = TestAlloc{};
        SharedPtr<TestObject> p8(new TestObject("string3"), FooDel{}, alloc);
        SharedPtr<TestObject> p9 = allocateShared<TestObject>(alloc, "string4");
    }
    initializeCounter();
    {
        SharedPtr<TestObject> f1(new TestObject("String"));
        SharedPtr<TestObject> f2(f1);
        SharedPtr<TestObject> f3 = f2;
        f3.reset();
        f3.swap(f2);

        TEST_ENSURE(p2 == nullptr && nullptr == p2);
        TEST_ENSURE((p2 == p3) && (p3 == p4) && (p4 == p5));
        TEST_ENSURE(f1 == f3);

        TEST_ENSURE(p7 == staticCast<TestObject const>(p7));
        TEST_ENSURE(p7.equals(reinterpretCast<TestObject>(p12)));
        TEST_ENSURE(p7.ownerEquals(p12));
    }

    TEST_ENSURE(ctor == 1 && dtor == 1);
    TEST_ENSURE(debugAllocCnt == 0);

    return 0;
}