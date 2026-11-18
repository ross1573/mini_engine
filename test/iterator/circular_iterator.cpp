#include <compare>

#include "test_macro.h"

import mini.test;

using mini::CircularIterator;

template <typename T>
class MockCircular {
public:
    typedef CircularIterator<T, MockCircular> Iterator;
    typedef CircularIterator<T const, MockCircular const> ConstIterator;

private:
    T* m_buffer;
    mini::size_t m_capacity;
    mini::size_t m_begin;
    mini::size_t m_size;

public:
    constexpr MockCircular(T* buffer, mini::size_t capacity, mini::size_t begin, mini::size_t size)
        : m_buffer(buffer)
        , m_capacity(capacity)
        , m_begin(begin)
        , m_size(size)
    {
    }

    constexpr Iterator begin() { return Iterator(m_begin, m_capacity, m_buffer, this); }
    constexpr Iterator end() { return Iterator(m_begin + m_size, m_capacity, m_buffer, this); }
    constexpr ConstIterator begin() const { return ConstIterator(m_begin, m_capacity, m_buffer, this); }
    constexpr ConstIterator end() const { return ConstIterator(m_begin + m_size, m_capacity, m_buffer, this); }

    constexpr Iterator at(mini::size_t index) { return Iterator(m_begin + index, m_capacity, m_buffer, this); }

    constexpr bool validIterator(ConstIterator iter) const
    {
        return iter.m_offset >= m_begin && iter.m_offset < m_begin + m_size;
    }

    constexpr bool validRange(ConstIterator first, ConstIterator last) const
    {
        return validIterator(first) && last.m_offset >= first.m_offset && last.m_offset <= m_begin + m_size;
    }
};

struct Base {
    mini::int32 value;
};

struct Derived : Base { };

typedef MockCircular<mini::int32>::Iterator IntIterator;
typedef MockCircular<mini::int32>::ConstIterator IntConstIterator;

template <typename T>
[[no_inline]] constexpr mini::int32 testAccess()
{
    T buffer[] = {
        makeObject<T>(3),
        makeObject<T>(4),
        makeObject<T>(9),
        makeObject<T>(1),
        makeObject<T>(2),
    };
    MockCircular<T> circular(buffer, 5, 3, 4);

    typename MockCircular<T>::Iterator iter = circular.begin();
    TEST_ENSURE(iter.address() == buffer + 3);
    TEST_ENSURE(iter.operator->() == buffer + 3);
    TEST_ENSURE(&*iter == buffer + 3);
    TEST_ENSURE(*iter == makeObject<T>(1));
    TEST_ENSURE(*(iter + 0) == makeObject<T>(1));
    TEST_ENSURE(*(iter + 1) == makeObject<T>(2));
    TEST_ENSURE(*(iter + 2) == makeObject<T>(3));
    TEST_ENSURE(*(iter + 3) == makeObject<T>(4));
    TEST_ENSURE(&*(iter + 2) == buffer);
    TEST_ENSURE(&*(iter + 3) == buffer + 1);

    typename MockCircular<T>::Iterator last = circular.at(3);
    TEST_ENSURE(last.address() == buffer + 1);
    TEST_ENSURE(*(last - 1) == makeObject<T>(3));
    TEST_ENSURE(*(last - 3) == makeObject<T>(1));
    TEST_ENSURE(&*(last - 2) == buffer + 4);

    *iter = makeObject<T>(5);
    TEST_ENSURE(buffer[3] == makeObject<T>(5));
    *(iter + 2) = makeObject<T>(6);
    TEST_ENSURE(buffer[0] == makeObject<T>(6));

    MockCircular<T> const& constCircular = circular;
    typename MockCircular<T>::ConstIterator citer = constCircular.begin();
    TEST_ENSURE(*citer == makeObject<T>(5));
    TEST_ENSURE(*(citer + 2) == makeObject<T>(6));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testDefault()
{
    IntIterator iter;
    TEST_ENSURE(!iter.valid());
    TEST_ENSURE(!iter.validWith(iter));
    TEST_ENSURE(!iter.reset());
    TEST_ENSURE(!iter.finish());
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(!iter.advance(0));
    TEST_ENSURE(!iter.advance(1));
    TEST_ENSURE(!iter.advance(-1));
    TEST_ENSURE(iter == IntIterator());

    return 0;
}

[[no_inline]] constexpr mini::int32 testArrow()
{
    Base buffer[] = {{1}, {2}, {3}};
    MockCircular<Base> circular(buffer, 3, 2, 2);

    MockCircular<Base>::Iterator iter = circular.begin();
    TEST_ENSURE(iter->value == 3);
    iter->value = 4;
    TEST_ENSURE(buffer[2].value == 4);

    MockCircular<Base> const& constCircular = circular;
    TEST_ENSURE((constCircular.begin() + 1)->value == 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValid()
{
    mini::int32 buffer[4] = {};
    MockCircular<mini::int32> circular(buffer, 4, 2, 3);

    TEST_ENSURE(circular.begin().valid());
    TEST_ENSURE(circular.at(2).valid());
    TEST_ENSURE(!circular.end().valid());
    TEST_ENSURE(!(circular.begin() - 1).valid());

    TEST_ENSURE(circular.begin().validWith(circular.end()));
    TEST_ENSURE(circular.begin().validWith(circular.begin()));
    TEST_ENSURE(circular.at(1).validWith(circular.at(2)));
    TEST_ENSURE(!circular.end().validWith(circular.begin()));
    TEST_ENSURE(!circular.at(2).validWith(circular.at(1)));
    TEST_ENSURE(!circular.begin().validWith(circular.end() + 1));

    return 0;
}

[[no_inline]] constexpr mini::int32 testResetFinish()
{
    mini::int32 buffer[4] = {};
    MockCircular<mini::int32> circular(buffer, 4, 3, 3);

    IntIterator iter = circular.at(1);
    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter == circular.end());
    TEST_ENSURE(iter.reset());
    TEST_ENSURE(iter == circular.begin());

    return 0;
}

[[no_inline]] constexpr mini::int32 testIncrementDecrement()
{
    mini::int32 buffer[] = {3, 4, 0, 1, 2};
    MockCircular<mini::int32> circular(buffer, 5, 3, 4);
    IntIterator iter = circular.begin();

    TEST_ENSURE(&++iter == &iter);
    TEST_ENSURE(*iter == 2);

    IntIterator post = iter++;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 3);
    TEST_ENSURE(iter.address() == buffer);

    TEST_ENSURE(&--iter == &iter);
    TEST_ENSURE(*iter == 2);

    post = iter--;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 1);

    ++iter;
    ++iter;
    ++iter;
    ++iter;
    TEST_ENSURE(iter == circular.end());
    TEST_ENSURE(iter.address() == buffer + 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testArithmetic()
{
    mini::int32 buffer[] = {4, 5, 1, 2, 3};
    MockCircular<mini::int32> circular(buffer, 5, 2, 5);
    IntIterator iter = circular.begin();

    TEST_ENSURE(&(iter += 3) == &iter);
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(&(iter -= 2) == &iter);
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(*(iter += -1) == 1);
    TEST_ENSURE(*(iter -= -4) == 5);

    iter = circular.at(1);
    IntIterator forward = iter + 2;
    TEST_ENSURE(*forward == 4);
    TEST_ENSURE(*iter == 2);

    IntIterator backward = forward - 3;
    TEST_ENSURE(*backward == 1);

    IntIterator commuted = 3 + iter;
    TEST_ENSURE(*commuted == 5);
    TEST_ENSURE(iter + 4 == circular.end());
    TEST_ENSURE(circular.end() - 5 == circular.begin());
    TEST_ENSURE(circular.end().address() == circular.begin().address());
    TEST_ENSURE(circular.end() != circular.begin());

    TEST_ENSURE(forward - iter == 2);
    TEST_ENSURE(iter - forward == -2);
    TEST_ENSURE(circular.end() - circular.begin() == 5);

    MockCircular<mini::int32> const& constCircular = circular;
    TEST_ENSURE(constCircular.end() - iter == 4);
    TEST_ENSURE(iter - constCircular.end() == -4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCompare()
{
    mini::int32 buffer[] = {3, 1, 2};
    MockCircular<mini::int32> circular(buffer, 3, 1, 3);
    MockCircular<mini::int32> const& constCircular = circular;

    IntIterator first = circular.begin();
    IntIterator third = circular.at(2);
    IntConstIterator cfirst = constCircular.begin();

    TEST_ENSURE(first == cfirst);
    TEST_ENSURE(cfirst == first);
    TEST_ENSURE(first != third);
    TEST_ENSURE(third != cfirst);

    TEST_ENSURE(third.address() < first.address());
    TEST_ENSURE(first < third);
    TEST_ENSURE(first <= third);
    TEST_ENSURE(third > first);
    TEST_ENSURE(third >= cfirst);
    TEST_ENSURE(cfirst < third);

    TEST_ENSURE((first <=> third) == std::strong_ordering::less);
    TEST_ENSURE((third <=> first) == std::strong_ordering::greater);
    TEST_ENSURE((first <=> cfirst) == std::strong_ordering::equal);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConversion()
{
    mini::int32 buffer[] = {3, 1, 2};
    MockCircular<mini::int32> circular(buffer, 3, 1, 3);

    IntIterator iter = circular.at(1);
    IntConstIterator citer(iter);
    TEST_ENSURE(citer.address() == buffer + 2);
    TEST_ENSURE(*citer == 2);
    TEST_ENSURE(citer.valid());
    TEST_ENSURE(citer.increment());
    TEST_ENSURE(*citer == 3);

    IntConstIterator implicit = iter;
    TEST_ENSURE(implicit == iter);
    TEST_ENSURE(implicit.finish());
    TEST_ENSURE(implicit == circular.end());

    IntConstIterator assigned;
    TEST_ENSURE(!assigned.valid());
    TEST_ENSURE(&(assigned = iter) == &assigned);
    TEST_ENSURE(assigned == iter);
    TEST_ENSURE(assigned.valid());

    Derived derived[2] = {};
    derived[0].value = 7;
    MockCircular<Derived> derivedCircular(derived, 2, 1, 2);
    CircularIterator<Base, MockCircular<Derived>> base = derivedCircular.at(1);
    TEST_ENSURE(base.address() == derived);

    return 0;
}

[[no_inline]] mini::int32 testDefaultAddress()
{
    IntIterator iter;
    TEST_ENSURE(iter.address() == nullptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedMove()
{
    mini::int32 buffer[] = {3, 4, 0, 1, 2};
    MockCircular<mini::int32> circular(buffer, 5, 3, 4);

    IntIterator iter = circular.begin();
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(iter == circular.begin());

    TEST_ENSURE(iter.increment());
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(iter.increment());
    TEST_ENSURE(*iter == 3);
    TEST_ENSURE(iter.address() == buffer);
    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(*iter == 2);

    TEST_ENSURE(iter.advance(2));
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(!iter.advance(1));
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(!iter.advance(5));
    TEST_ENSURE(*iter == 4);

    TEST_ENSURE(iter.advance(-3));
    TEST_ENSURE(*iter == 1);
    TEST_ENSURE(!iter.advance(-1));
    TEST_ENSURE(*iter == 1);
    TEST_ENSURE(iter.advance(0));
    TEST_ENSURE(*iter == 1);

    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(*iter == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testLargeOffset()
{
    mini::int32 buffer[] = {3, 4, 0, 1, 2};
    MockCircular<mini::int32> circular(buffer, 5, 13, 4);

    IntIterator iter = circular.begin();
    TEST_ENSURE(iter.address() == buffer + 3);
    TEST_ENSURE(*iter == 1);
    TEST_ENSURE(*(iter + 2) == 3);
    TEST_ENSURE((iter + 2).address() == buffer);
    TEST_ENSURE(!iter.decrement());

    TEST_ENSURE(iter.advance(3));
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(iter.address() == buffer + 1);
    TEST_ENSURE(!iter.increment());

    TEST_ENSURE(circular.end().address() == buffer + 2);
    TEST_ENSURE(circular.end() - circular.begin() == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testZeroBegin()
{
    mini::int32 buffer[] = {1, 2, 3};
    MockCircular<mini::int32> circular(buffer, 3, 0, 3);

    IntIterator iter = circular.begin();
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(iter == circular.begin());
    TEST_ENSURE(!iter.advance(-1));
    TEST_ENSURE(iter == circular.begin());
    TEST_ENSURE(*iter == 1);

    TEST_ENSURE(!iter.advance(3));
    TEST_ENSURE(iter.advance(2));
    TEST_ENSURE(*iter == 3);
    TEST_ENSURE(!iter.increment());

    TEST_ENSURE(circular.end().address() == buffer);
    TEST_ENSURE(circular.end() != circular.begin());
    TEST_ENSURE(circular.begin() < circular.end());

    return 0;
}

[[no_inline]] constexpr mini::int32 testEmpty()
{
    mini::int32 buffer[] = {1, 2, 3};
    MockCircular<mini::int32> circular(buffer, 3, 2, 0);

    IntIterator iter = circular.begin();
    TEST_ENSURE(iter == circular.end());
    TEST_ENSURE(!iter.valid());
    TEST_ENSURE(!iter.validWith(circular.end()));
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(!iter.advance(0));
    TEST_ENSURE(iter.reset());
    TEST_ENSURE(iter == circular.begin());
    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter == circular.end());

    return 0;
}

[[no_inline]] constexpr mini::int32 testDifferentBuffer()
{
    mini::int32 buffer[] = {1, 2, 3};
    mini::int32 other[] = {1, 2, 3};
    MockCircular<mini::int32> circular(buffer, 3, 1, 2);
    MockCircular<mini::int32> otherCircular(other, 3, 1, 2);

    TEST_ENSURE(circular.begin() != otherCircular.begin());
    TEST_ENSURE(circular.end() != otherCircular.end());

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testAccess, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testAccess, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testAccess, MoveOnlyObject);

    TEST_CONSTEXPR(testDefault);
    TEST_CONSTEXPR(testArrow);
    TEST_CONSTEXPR(testValid);
    TEST_CONSTEXPR(testResetFinish);
    TEST_CONSTEXPR(testIncrementDecrement);
    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testCompare);
    TEST_CONSTEXPR(testConversion);

    TEST_FUNCTION(testDefaultAddress);
    TEST_CONSTEXPR(testCheckedMove);
    TEST_CONSTEXPR(testLargeOffset);
    TEST_CONSTEXPR(testZeroBegin);
    TEST_CONSTEXPR(testEmpty);
    TEST_CONSTEXPR(testDifferentBuffer);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
