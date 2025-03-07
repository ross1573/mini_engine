import mini.test;

using namespace mini;

template <typename U>
inline auto RebindAllocator(Allocator<Foo>);

template <>
inline auto RebindAllocator<Foo>(Allocator<Foo>)
{
    return FooAlloc{};
}

int main()
{
    [[maybe_unused]] DummyAllocator dummy{};
    auto&& r = RebindAllocator<Foo>(Allocator<Foo>{});
    auto&& r2 = RebindAllocator<ConstExprFoo>(Allocator<ConstExprFoo>{});
    auto&& r3 = RebindAllocator<Foo>(FooAlloc{});

    static_assert(AllocatorT<Allocator<Foo>, Foo>);
    static_assert(AllocatorT<Allocator<int>, int>);
    static_assert(RebindableWithT<Allocator<Foo>, Foo>);
    static_assert(SameAsT<FooAlloc&&, decltype(r)>);
    static_assert(SameAsT<FooAlloc&&, decltype(r3)>);
    static_assert(SameAsT<Allocator<ConstExprFoo>&&, decltype(r2)>);

    static_assert(Allocator<Foo>{} == Allocator<int>{});
    static_assert(DummyAllocator{} == DummyAllocator{});

    return 0;
}