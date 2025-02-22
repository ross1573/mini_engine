#include "mini_engine.h"
#include "container.h"
#include "memory/shared_ptr.h"

void SharedPtrTest()
{
    SharedPtr<Foo> p;
    SharedPtr<Foo> p2(nullptr);
    SharedPtr<Foo> p3(nullptr, mini::DefaultDeleter<Foo>{});
    SharedPtr<Foo> p4(nullptr, [](NullptrT){});
    SharedPtr<Foo> p5(nullptr, [](NullptrT){}, mini::Allocator<NullptrT>{});
    SharedPtr<Foo> p6(new Foo("string1"));
    SharedPtr<Foo> p7(new Foo("string2"), FooDel{});
    SharedPtr<Foo> p10 = MakeShared<Foo>("string5");
    SharedPtr<String> p11(&p7->str, [](auto){});
    SharedPtr<String> p12(p7, &p7->str);

    PrintLine();
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

        ASSERT(p2 == nullptr && nullptr == p2);
        ASSERT(!p2);
        ASSERT((p2 == p3) && (p3 == p4) && (p4 == p5));
        ASSERT(f1 == f3);

        ASSERT(p7 == StaticCast<Foo const>(p7));
        ASSERT(p7.Equals(ReinterpretCast<Foo>(p12)));
        ASSERT(p7.OwnerEquals(p12));
    }
    PrintCounter("SharedPtr");

    ASSERT(ctor == 1 && dtor == 1);
    ASSERT(debugAllocCnt == 0);

    PrintLine();
}