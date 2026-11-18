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

template <typename T>
[[no_inline]] constexpr mini::int32 testSubscript()
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
    TEST_ENSURE(iter[0] == makeObject<T>(1));
    TEST_ENSURE(iter[1] == makeObject<T>(2));
    TEST_ENSURE(iter[2] == makeObject<T>(3));
    TEST_ENSURE(iter[3] == makeObject<T>(4));
    TEST_ENSURE(&iter[2] == buffer);
    TEST_ENSURE(&iter[3] == buffer + 1);

    typename MockCircular<T>::Iterator last = circular.at(3);
    TEST_ENSURE(last[-1] == makeObject<T>(3));
    TEST_ENSURE(last[-3] == makeObject<T>(1));
    TEST_ENSURE(&last[-2] == buffer + 4);

    iter[2] = makeObject<T>(6);
    TEST_ENSURE(buffer[0] == makeObject<T>(6));

    MockCircular<T> const& constCircular = circular;
    typename MockCircular<T>::ConstIterator citer = constCircular.begin();
    TEST_ENSURE(citer[2] == makeObject<T>(6));
    TEST_ENSURE(&citer[3] == buffer + 1);

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testSubscriptWrap()
{
    mini::int32 buffer[] = {3, 4, 0, 1, 2};
    MockCircular<mini::int32> circular(buffer, 5, 13, 4);

    MockCircular<mini::int32>::Iterator iter = circular.begin();
    TEST_ENSURE(iter[0] == 1);
    TEST_ENSURE(iter[1] == 2);
    TEST_ENSURE(iter[2] == 3);
    TEST_ENSURE(iter[3] == 4);
    TEST_ENSURE(&iter[2] == buffer);

    MockCircular<mini::int32>::Iterator last = circular.end() - 1;
    TEST_ENSURE(last[0] == 4);
    TEST_ENSURE(last[-1] == 3);
    TEST_ENSURE(last[-3] == 1);
    TEST_ENSURE(&last[-3] == buffer + 3);

    mini::int32 zero[] = {1, 2, 3};
    MockCircular<mini::int32> zeroCircular(zero, 3, 0, 3);
    MockCircular<mini::int32>::Iterator zeroLast = zeroCircular.at(2);
    TEST_ENSURE(zeroLast[-2] == 1);
    TEST_ENSURE(&zeroLast[-2] == zero);
    TEST_ENSURE(zeroLast[-1] == 2);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testSubscript, mini::int32);
    TEST_CONSTEXPR_TEMPLATE(testSubscript, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testSubscript, MoveOnlyObject);

    TEST_CONSTEXPR(testSubscriptWrap);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
