#include "test_macro.h"

import mini.test;

using namespace mini;
using namespace mini::test;

template <typename U>
inline auto RebindAllocator(Allocator<TestObject>);

template <>
inline auto RebindAllocator<TestObject>(Allocator<TestObject>)
{
    return TestAlloc{};
}

int main()
{
    auto&& r = RebindAllocator<TestObject>(Allocator<TestObject>{});
    auto&& r2 = RebindAllocator<ConstexprObject>(Allocator<ConstexprObject>{});
    auto&& r3 = RebindAllocator<TestObject>(TestAlloc{});

    static_assert(AllocatorT<Allocator<TestObject>, TestObject>);
    static_assert(AllocatorT<Allocator<int>, int>);
    static_assert(RebindableWithT<Allocator<TestObject>, TestObject>);
    static_assert(SameAsT<TestAlloc&&, decltype(r)>);
    static_assert(SameAsT<TestAlloc&&, decltype(r3)>);
    static_assert(SameAsT<Allocator<ConstexprObject>&&, decltype(r2)>);

    static_assert(Allocator<TestObject>{} == Allocator<int>{});

    return 0;
}