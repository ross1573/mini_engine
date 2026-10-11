#include "test_macro.h"

import mini.test;

using mini::rebind;

template <typename U>
inline auto rebind(mini::Allocator<TestObject>);

template <>
inline auto rebind<TestObject>(mini::Allocator<TestObject> /*unused*/)
{
    return TestAlloc{};
}

mini::int32 main()
{
    auto&& r = rebind<TestObject>(mini::Allocator<TestObject>{});
    auto&& r2 = rebind<ConstexprObject>(mini::Allocator<ConstexprObject>{});
    auto&& r3 = rebind<TestObject>(TestAlloc{});

    static_assert(mini::AllocatorT<mini::Allocator<TestObject>, TestObject>);
    static_assert(mini::AllocatorT<mini::Allocator<mini::int32>, mini::int32>);
    static_assert(mini::RebindableWithT<mini::Allocator<TestObject>, TestObject>);
    static_assert(mini::SameAsT<TestAlloc&&, decltype(r)>);
    static_assert(mini::SameAsT<TestAlloc&&, decltype(r3)>);
    static_assert(mini::SameAsT<mini::Allocator<ConstexprObject>&&, decltype(r2)>);

    static_assert(mini::Allocator<TestObject>{} == mini::Allocator<mini::int32>{});

    return 0;
}