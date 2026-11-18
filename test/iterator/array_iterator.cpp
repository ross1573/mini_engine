#include <compare>

#include "test_macro.h"

import mini.test;

using mini::ArrayIterator;

template <typename T>
class MockArray {
public:
    typedef ArrayIterator<T, MockArray> Iterator;
    typedef ArrayIterator<T const, MockArray const> ConstIterator;

private:
    T* m_data;
    mini::size_t m_size;

public:
    constexpr MockArray(T* data, mini::size_t size)
        : m_data(data)
        , m_size(size)
    {
    }

    constexpr Iterator begin() { return Iterator(m_data, this); }
    constexpr Iterator end() { return Iterator(m_data + m_size, this); }
    constexpr ConstIterator begin() const { return ConstIterator(m_data, this); }
    constexpr ConstIterator end() const { return ConstIterator(m_data + m_size, this); }

    constexpr Iterator at(mini::size_t index) { return Iterator(m_data + index, this); }

    constexpr bool validIterator(ConstIterator iter) const
    {
        return iter.address() >= m_data && iter.address() < m_data + m_size;
    }

    constexpr bool validRange(ConstIterator first, ConstIterator last) const
    {
        return validIterator(first) && last.address() >= first.address() && last.address() <= m_data + m_size;
    }
};

struct Base {
    mini::int32 value;
};

struct Derived : Base { };

typedef MockArray<mini::int32>::Iterator IntIterator;
typedef MockArray<mini::int32>::ConstIterator IntConstIterator;

template <typename T>
[[no_inline]] constexpr mini::int32 testAccess()
{
    T data[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
        makeObject<T>(4),
    };
    MockArray<T> array(data, 4);

    typename MockArray<T>::Iterator iter = array.begin();
    TEST_ENSURE(iter.address() == data);
    TEST_ENSURE(iter.operator->() == data);
    TEST_ENSURE(&*iter == data);
    TEST_ENSURE(*iter == makeObject<T>(1));
    TEST_ENSURE(iter[0] == makeObject<T>(1));
    TEST_ENSURE(iter[3] == makeObject<T>(4));
    TEST_ENSURE(&iter[2] == data + 2);

    typename MockArray<T>::Iterator last = array.at(3);
    TEST_ENSURE(last[-3] == makeObject<T>(1));
    TEST_ENSURE(&last[-1] == data + 2);

    *iter = makeObject<T>(5);
    TEST_ENSURE(data[0] == makeObject<T>(5));
    iter[1] = makeObject<T>(6);
    TEST_ENSURE(data[1] == makeObject<T>(6));

    MockArray<T> const& constArray = array;
    typename MockArray<T>::ConstIterator citer = constArray.begin();
    TEST_ENSURE(*citer == makeObject<T>(5));
    TEST_ENSURE(citer[3] == makeObject<T>(4));

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testDefault()
{
    IntIterator iter;
    TEST_ENSURE(iter.address() == nullptr);
    TEST_ENSURE(!iter.valid());
    TEST_ENSURE(!iter.validWith(iter));
    TEST_ENSURE(!iter.reset());
    TEST_ENSURE(!iter.finish());
    TEST_ENSURE(iter.address() == nullptr);
    TEST_ENSURE(iter == IntIterator());

    return 0;
}

[[no_inline]] constexpr mini::int32 testArrow()
{
    Base data[] = {{1}, {2}};
    MockArray<Base> array(data, 2);

    MockArray<Base>::Iterator iter = array.begin();
    TEST_ENSURE(iter->value == 1);
    iter->value = 3;
    TEST_ENSURE(data[0].value == 3);

    MockArray<Base> const& constArray = array;
    TEST_ENSURE((constArray.begin() + 1)->value == 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValid()
{
    mini::int32 data[] = {0, 1, 2, 3, 0};
    MockArray<mini::int32> array(data + 1, 3);

    TEST_ENSURE(array.begin().valid());
    TEST_ENSURE(array.at(2).valid());
    TEST_ENSURE(!array.end().valid());
    TEST_ENSURE(!(array.begin() - 1).valid());

    TEST_ENSURE(array.begin().validWith(array.end()));
    TEST_ENSURE(array.begin().validWith(array.begin()));
    TEST_ENSURE(array.at(1).validWith(array.at(2)));
    TEST_ENSURE(!array.end().validWith(array.begin()));
    TEST_ENSURE(!array.at(2).validWith(array.at(1)));
    TEST_ENSURE(!array.begin().validWith(array.end() + 1));

    return 0;
}

[[no_inline]] constexpr mini::int32 testResetFinish()
{
    mini::int32 data[] = {1, 2, 3};
    MockArray<mini::int32> array(data, 3);

    IntIterator iter = array.at(1);
    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter == array.end());
    TEST_ENSURE(iter.reset());
    TEST_ENSURE(iter == array.begin());

    return 0;
}

[[no_inline]] constexpr mini::int32 testIncrementDecrement()
{
    mini::int32 data[] = {1, 2, 3, 4};
    MockArray<mini::int32> array(data, 4);
    IntIterator iter = array.begin();

    TEST_ENSURE(&++iter == &iter);
    TEST_ENSURE(*iter == 2);

    IntIterator post = iter++;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 3);

    TEST_ENSURE(&--iter == &iter);
    TEST_ENSURE(*iter == 2);

    post = iter--;
    TEST_ENSURE(*post == 2);
    TEST_ENSURE(*iter == 1);

    ++iter;
    ++iter;
    ++iter;
    ++iter;
    TEST_ENSURE(iter == array.end());

    return 0;
}

[[no_inline]] constexpr mini::int32 testArithmetic()
{
    mini::int32 data[] = {1, 2, 3, 4, 5};
    MockArray<mini::int32> array(data, 5);
    IntIterator iter = array.begin();

    TEST_ENSURE(&(iter += 3) == &iter);
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(&(iter -= 2) == &iter);
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(*(iter += -1) == 1);
    TEST_ENSURE(*(iter -= -4) == 5);

    iter = array.at(1);
    IntIterator forward = iter + 2;
    TEST_ENSURE(*forward == 4);
    TEST_ENSURE(*iter == 2);

    IntIterator backward = forward - 3;
    TEST_ENSURE(*backward == 1);

    IntIterator commuted = 3 + iter;
    TEST_ENSURE(*commuted == 5);
    TEST_ENSURE(iter + 4 == array.end());
    TEST_ENSURE(array.end() - 5 == array.begin());

    TEST_ENSURE(forward - iter == 2);
    TEST_ENSURE(iter - forward == -2);
    TEST_ENSURE(array.end() - array.begin() == 5);

    MockArray<mini::int32> const& constArray = array;
    TEST_ENSURE(constArray.end() - iter == 4);
    TEST_ENSURE(iter - constArray.end() == -4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCompare()
{
    mini::int32 data[] = {1, 2, 3};
    MockArray<mini::int32> array(data, 3);
    MockArray<mini::int32> const& constArray = array;

    IntIterator first = array.begin();
    IntIterator second = array.at(1);
    IntConstIterator cfirst = constArray.begin();

    TEST_ENSURE(first == cfirst);
    TEST_ENSURE(cfirst == first);
    TEST_ENSURE(first != second);
    TEST_ENSURE(second != cfirst);

    TEST_ENSURE(first < second);
    TEST_ENSURE(first <= second);
    TEST_ENSURE(second > first);
    TEST_ENSURE(second >= cfirst);
    TEST_ENSURE(cfirst < second);

    TEST_ENSURE((first <=> second) == std::strong_ordering::less);
    TEST_ENSURE((second <=> first) == std::strong_ordering::greater);
    TEST_ENSURE((first <=> cfirst) == std::strong_ordering::equal);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConversion()
{
    mini::int32 data[] = {1, 2, 3};
    MockArray<mini::int32> array(data, 3);

    IntIterator iter = array.at(1);
    IntConstIterator citer(iter);
    TEST_ENSURE(citer.address() == data + 1);
    TEST_ENSURE(*citer == 2);
    TEST_ENSURE(citer.valid());
    TEST_ENSURE(citer.increment());
    TEST_ENSURE(*citer == 3);

    IntConstIterator implicit = iter;
    TEST_ENSURE(implicit == iter);
    TEST_ENSURE(implicit.finish());
    TEST_ENSURE(implicit == array.end());

    IntConstIterator assigned;
    TEST_ENSURE(!assigned.valid());
    TEST_ENSURE(&(assigned = iter) == &assigned);
    TEST_ENSURE(assigned == iter);
    TEST_ENSURE(assigned.valid());

    Derived derived[2] = {};
    derived[1].value = 7;
    MockArray<Derived> derivedArray(derived, 2);
    ArrayIterator<Base, MockArray<Derived>> base = derivedArray.at(1);
    TEST_ENSURE(base.address() == derived + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testDefaultMove()
{
    IntIterator iter;
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(!iter.advance(0));
    TEST_ENSURE(!iter.advance(2));
    TEST_ENSURE(iter.address() == nullptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedMove()
{
    mini::int32 data[] = {0, 0, 1, 2, 3, 4, 0, 0};
    MockArray<mini::int32> array(data + 2, 4);

    IntIterator iter = array.begin();
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(iter == array.begin());

    TEST_ENSURE(iter.increment());
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(*iter == 1);

    TEST_ENSURE(iter.advance(3));
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(*iter == 4);
    TEST_ENSURE(!iter.advance(1));
    TEST_ENSURE(*iter == 4);

    TEST_ENSURE(iter.advance(-2));
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(!iter.advance(-2));
    TEST_ENSURE(*iter == 2);
    TEST_ENSURE(iter.advance(0));
    TEST_ENSURE(*iter == 2);

    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(*iter == 4);

    return 0;
}

[[no_inline]] constexpr mini::int32 testEmpty()
{
    mini::int32 data[] = {1, 2, 3};
    MockArray<mini::int32> array(data + 1, 0);

    IntIterator iter = array.begin();
    TEST_ENSURE(iter == array.end());
    TEST_ENSURE(!iter.valid());
    TEST_ENSURE(!iter.validWith(array.end()));
    TEST_ENSURE(!iter.increment());
    TEST_ENSURE(!iter.decrement());
    TEST_ENSURE(!iter.advance(0));
    TEST_ENSURE(iter.address() == data + 1);
    TEST_ENSURE(iter.reset());
    TEST_ENSURE(iter == array.begin());
    TEST_ENSURE(iter.finish());
    TEST_ENSURE(iter == array.end());

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

    TEST_CONSTEXPR(testDefaultMove);
    TEST_CONSTEXPR(testCheckedMove);
    TEST_CONSTEXPR(testEmpty);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
