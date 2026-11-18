#include <compare>

#include "test_macro.h"

import mini.test;

using mini::PtrIterator;

struct Base {
    mini::int32 value;
};

struct Derived : Base { };

template <typename T>
[[no_inline]] constexpr mini::int32 testAccess()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };

    PtrIterator<T*> iter(array);
    TEST_ENSURE(iter.address() == array);
    TEST_ENSURE(iter.operator->() == array);
    TEST_ENSURE(&*iter == array);
    TEST_ENSURE(*iter == makeObject<T>(1));
    TEST_ENSURE(iter[0] == makeObject<T>(1));
    TEST_ENSURE(iter[3] == makeObject<T>(4));
    TEST_ENSURE(&iter[2] == array + 2);

    PtrIterator<T*> last(array + 3);
    TEST_ENSURE(last[-3] == makeObject<T>(1));
    TEST_ENSURE(&last[-1] == array + 2);

    *iter = makeObject<T>(5);
    TEST_ENSURE(array[0] == makeObject<T>(5));

    iter[1] = makeObject<T>(6);
    TEST_ENSURE(array[1] == makeObject<T>(6));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testDefault()
{
    PtrIterator<mini::int32*> iter;
    TEST_ENSURE(iter.address() == nullptr);
    TEST_ENSURE(iter.valid());
    TEST_ENSURE(iter == PtrIterator<mini::int32*>());
    TEST_ENSURE(iter.validWith(PtrIterator<mini::int32*>()));
    TEST_ENSURE(iter - PtrIterator<mini::int32*>() == 0);
    TEST_ENSURE(iter.advance(0));
    TEST_ENSURE(iter.address() == nullptr);
    TEST_ENSURE(iter == PtrIterator<mini::int32 const*>());

    return 0;
}

[[no_inline]] constexpr mini::int32 testArrow()
{
    Base array[] = {{1}, {2}};
    PtrIterator<Base*> iter(array);
    TEST_ENSURE(iter->value == 1);

    iter->value = 3;
    TEST_ENSURE(array[0].value == 3);

    PtrIterator<Base const*> citer(array + 1);
    TEST_ENSURE(citer->value == 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testIncrementDecrement()
{
    mini::int32 array[] = {1, 2, 3, 4};
    PtrIterator<mini::int32*> iter(array);

    TEST_ENSURE(&++iter == &iter);
    TEST_ENSURE(*iter == 2);

    PtrIterator<mini::int32*> post = iter++;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 3);

    TEST_ENSURE(&--iter == &iter);
    TEST_ENSURE(*iter == 2);

    post = iter--;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testArithmetic()
{
    mini::int32 array[] = {1, 2, 3, 4, 5};
    PtrIterator<mini::int32*> iter(array);

    TEST_ENSURE(&(iter += 3) == &iter);
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(&(iter -= 2) == &iter);
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(*(iter += -1) == 1);
    TEST_ENSURE(*(iter -= -4) == 5);

    iter = PtrIterator<mini::int32*>(array + 1);
    PtrIterator<mini::int32*> forward = iter + 2;
    TEST_ENSURE(*forward == 4);
    TEST_ENSURE(*iter == 2);

    PtrIterator<mini::int32*> backward = forward - 3;
    TEST_ENSURE(*backward == 1);
    TEST_ENSURE(*forward == 4);

    PtrIterator<mini::int32*> commuted = 3 + iter;
    TEST_ENSURE(*commuted == 5);
    TEST_ENSURE(*(iter + -1) == 1);
    TEST_ENSURE(*(iter - -1) == 3);

    TEST_ENSURE(forward - iter == 2);
    TEST_ENSURE(iter - forward == -2);
    TEST_ENSURE(iter - iter == 0);

    PtrIterator<mini::int32 const*> citer(array + 4);
    TEST_ENSURE(citer - iter == 3);
    TEST_ENSURE(iter - citer == -3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCompare()
{
    mini::int32 array[] = {1, 2, 3};
    PtrIterator<mini::int32*> first(array);
    PtrIterator<mini::int32*> second(array + 1);
    PtrIterator<mini::int32 const*> cfirst(array);

    TEST_ENSURE(first == cfirst);
    TEST_ENSURE(cfirst == first);
    TEST_ENSURE(first != second);
    TEST_ENSURE(second != cfirst);

    TEST_ENSURE(first < second);
    TEST_ENSURE(first <= second);
    TEST_ENSURE(second > first);
    TEST_ENSURE(second >= first);
    TEST_ENSURE(cfirst < second);

    TEST_ENSURE((first <=> second) == std::strong_ordering::less);
    TEST_ENSURE((second <=> first) == std::strong_ordering::greater);
    TEST_ENSURE((first <=> cfirst) == std::strong_ordering::equal);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConversion()
{
    mini::int32 array[] = {1, 2, 3};
    PtrIterator<mini::int32*> iter(array + 1);

    PtrIterator<mini::int32 const*> citer(iter);
    TEST_ENSURE(citer.address() == array + 1);
    TEST_ENSURE(*citer == 2);

    PtrIterator<mini::int32 const*> implicit = iter;
    TEST_ENSURE(implicit == iter);

    PtrIterator<mini::int32 const*> assigned;
    assigned = iter + 1;
    TEST_ENSURE(assigned.address() == array + 2);
    TEST_ENSURE(&(assigned = iter) == &assigned);
    TEST_ENSURE(assigned == iter);

    Derived derived[2] = {};
    derived[1].value = 7;
    PtrIterator<Base*> base = PtrIterator<Derived*>(derived + 1);
    TEST_ENSURE(base->value == 7);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedMove()
{
    mini::int32 array[] = {1, 2, 3, 4, 5};
    PtrIterator<mini::int32*> iter(array);

    TEST_ENSURE(iter.increment());
    TEST_ENSURE(iter.address() == array + 1);
    TEST_ENSURE(iter.increment());
    TEST_ENSURE(*iter == 3);

    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(iter.address() == array + 1);

    TEST_ENSURE(iter.advance(3));
    TEST_ENSURE(iter.address() == array + 4);
    TEST_ENSURE(iter.advance(-4));
    TEST_ENSURE(iter.address() == array);
    TEST_ENSURE(iter.advance(0));
    TEST_ENSURE(iter.address() == array);

    TEST_ENSURE(iter.advance(5));
    TEST_ENSURE(iter.address() == array + 5);

    return 0;
}

[[no_inline]] constexpr mini::int32 testDerivedCompare()
{
    Derived derived[2] = {};
    PtrIterator<Derived*> first(derived);
    PtrIterator<Derived*> second(derived + 1);
    PtrIterator<Base*> base(derived);
    PtrIterator<Base const*> cbase(derived + 1);

    TEST_ENSURE(first == base);
    TEST_ENSURE(base == first);
    TEST_ENSURE(second != base);
    TEST_ENSURE(second == cbase);
    TEST_ENSURE(base < second);
    TEST_ENSURE(cbase > first);
    TEST_ENSURE((first <=> base) == std::strong_ordering::equal);
    TEST_ENSURE((second <=> base) == std::strong_ordering::greater);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testAccess, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAccess, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAccess, MoveOnlyObject);

    TEST_CONSTEXPR(testDefault);
    TEST_CONSTEXPR(testArrow);
    TEST_CONSTEXPR(testIncrementDecrement);
    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testCompare);
    TEST_CONSTEXPR(testConversion);

    TEST_CONSTEXPR(testCheckedMove);
    TEST_CONSTEXPR(testDerivedCompare);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
