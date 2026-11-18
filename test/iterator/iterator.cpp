#include "test_macro.h"

import mini.test;

using mini::PtrIterator;

template <typename T>
class ForwardIter {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

protected:
    T* m_ptr = nullptr;

public:
    constexpr ForwardIter() = default;
    explicit constexpr ForwardIter(T* ptr)
        : m_ptr(ptr)
    {
    }

    constexpr bool valid() const { return m_ptr != nullptr; }
    constexpr bool validWith(ForwardIter const& other) const { return m_ptr <= other.m_ptr; }
    constexpr bool increment()
    {
        ++m_ptr;
        return true;
    }

    constexpr Pointer address() const { return m_ptr; }
    constexpr Pointer operator->() const { return m_ptr; }
    constexpr Reference operator*() const { return *m_ptr; }

    constexpr ForwardIter& operator++()
    {
        ++m_ptr;
        return *this;
    }

    constexpr ForwardIter operator++(mini::int32)
    {
        ForwardIter tmp(*this);
        ++m_ptr;
        return tmp;
    }

    constexpr bool operator==(ForwardIter const&) const = default;
};

template <typename T>
class BidirectionalIter : public ForwardIter<T> {
public:
    constexpr BidirectionalIter() = default;
    explicit constexpr BidirectionalIter(T* ptr)
        : ForwardIter<T>(ptr)
    {
    }

    constexpr bool decrement()
    {
        --this->m_ptr;
        return true;
    }

    constexpr BidirectionalIter& operator++()
    {
        ++this->m_ptr;
        return *this;
    }

    constexpr BidirectionalIter operator++(mini::int32)
    {
        BidirectionalIter tmp(*this);
        ++this->m_ptr;
        return tmp;
    }

    constexpr BidirectionalIter& operator--()
    {
        --this->m_ptr;
        return *this;
    }

    constexpr BidirectionalIter operator--(mini::int32)
    {
        BidirectionalIter tmp(*this);
        --this->m_ptr;
        return tmp;
    }

    constexpr bool operator==(BidirectionalIter const&) const = default;
};

template <typename T>
class Container {
public:
    typedef ForwardIter<T> Iterator;
    typedef ForwardIter<T const> ConstIterator;

    T m_data[3];

    constexpr Iterator begin() { return Iterator(m_data); }
    constexpr Iterator end() { return Iterator(m_data + 3); }
    constexpr ConstIterator begin() const { return ConstIterator(m_data); }
    constexpr ConstIterator end() const { return ConstIterator(m_data + 3); }
};

template <typename T>
class ConstContainer {
public:
    typedef ForwardIter<T const> ConstIterator;

    T m_data[2];

    constexpr ConstIterator begin() const { return ConstIterator(m_data); }
    constexpr ConstIterator end() const { return ConstIterator(m_data + 2); }
};

[[no_inline]] constexpr mini::int32 testBeginEnd()
{
    Container<mini::int32> container{{1, 2, 3}};
    Container<mini::int32> const& constContainer = container;

    TEST_ENSURE(mini::begin(container).address() == container.m_data);
    TEST_ENSURE(mini::end(container).address() == container.m_data + 3);
    TEST_ENSURE(mini::begin(constContainer).address() == container.m_data);
    TEST_ENSURE(mini::end(constContainer).address() == container.m_data + 3);
    TEST_ENSURE(mini::cbegin(container).address() == container.m_data);
    TEST_ENSURE(mini::cend(container).address() == container.m_data + 3);

    *mini::begin(container) = 10;
    TEST_ENSURE(container.m_data[0] == 10);

    ConstContainer<mini::int32> constOnly{{4, 5}};
    TEST_ENSURE(mini::begin(constOnly).address() == constOnly.m_data);
    TEST_ENSURE(mini::end(constOnly).address() == constOnly.m_data + 2);
    TEST_ENSURE(mini::cbegin(constOnly).address() == constOnly.m_data);
    TEST_ENSURE(mini::cend(constOnly).address() == constOnly.m_data + 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testStdIteratorOperations()
{
    mini::int32 data[] = {1, 2, 3, 4};

    ForwardIter<mini::int32> first(data);
    ForwardIter<mini::int32> last(data + 4);
    TEST_ENSURE(std::distance(first, last) == 4);

    std::advance(first, 3);
    TEST_ENSURE(*first == 4);

    BidirectionalIter<mini::int32> bidirectional(data + 3);
    std::advance(bidirectional, -2);
    TEST_ENSURE(*bidirectional == 2);
    TEST_ENSURE(std::prev(bidirectional).address() == data);

    PtrIterator<mini::int32*> ptrFirst(data);
    PtrIterator<mini::int32*> ptrLast(data + 4);
    TEST_ENSURE(std::distance(ptrFirst, ptrLast) == 4);
    TEST_ENSURE(std::distance(ptrLast, ptrFirst) == -4);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testBeginEnd);
    TEST_CONSTEXPR(testStdIteratorOperations);

    return 0;
}
