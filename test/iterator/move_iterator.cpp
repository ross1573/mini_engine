#include <compare>

#include "test_macro.h"

import mini.test;

using mini::MoveIterator;
using mini::PtrIterator;

template <typename T>
class ForwardIter {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;

protected:
    T* m_ptr = nullptr;
    T* m_end = nullptr;

public:
    constexpr ForwardIter() = default;
    constexpr ForwardIter(T* ptr, T* end)
        : m_ptr(ptr)
        , m_end(end)
    {
    }

    template <typename U>
    constexpr ForwardIter(ForwardIter<U> const& other)
        requires mini::ConvertibleToT<U*, T*>
        : m_ptr(other.address())
        , m_end(other.last())
    {
    }

    constexpr T* last() const { return m_end; }
    constexpr bool valid() const { return m_ptr != m_end; }
    constexpr bool validWith(ForwardIter const& other) const { return m_ptr <= other.m_ptr && other.m_ptr <= m_end; }
    constexpr bool increment()
    {
        if (m_ptr + 1 >= m_end) {
            return false;
        }
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

    template <typename U>
    constexpr bool operator==(ForwardIter<U> const& other) const
    {
        return m_ptr == other.address();
    }
};

template <typename T>
class BidirectionalIter : public ForwardIter<T> {
public:
    constexpr BidirectionalIter() = default;
    constexpr BidirectionalIter(T* ptr, T* end)
        : ForwardIter<T>(ptr, end)
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

    constexpr bool operator==(BidirectionalIter const& other) const { return this->m_ptr == other.m_ptr; }
};

struct Base {
    mini::int32 value;
};

typedef MoveIterator<PtrIterator<mini::int32*>> IntMoveIterator;
typedef MoveIterator<PtrIterator<mini::int32 const*>> IntConstMoveIterator;

template <typename T>
[[no_inline]] constexpr mini::int32 testMoveAccess()
{
    T array[] = {
        makeObject<T>(1),
        makeObject<T>(2),
        makeObject<T>(3),
    };

    MoveIterator<PtrIterator<T*>> iter{PtrIterator<T*>(array)};
    TEST_ENSURE(iter.address() == array);
    TEST_ENSURE(iter.operator->() == array);
    TEST_ENSURE(iter.base().address() == array);

    T first = *iter;
    TEST_ENSURE(first == makeObject<T>(1));

    T third = iter[2];
    TEST_ENSURE(third == makeObject<T>(3));

    T second(*(iter + 1));
    TEST_ENSURE(second == makeObject<T>(2));

    if constexpr (TestObjectT<T>) {
        TEST_ENSURE(array[0].resource() == nullptr);
        TEST_ENSURE(array[1].resource() == nullptr);
        TEST_ENSURE(array[2].resource() == nullptr);
    }

    if !consteval {
        TEST_ENSURE(violationCount == 0);
    }

    return 0;
}

[[no_inline]] constexpr mini::int32 testDefault()
{
    IntMoveIterator iter;
    TEST_ENSURE(iter.base() == PtrIterator<mini::int32*>());
    TEST_ENSURE(iter.address() == nullptr);
    TEST_ENSURE(iter == IntMoveIterator());

    return 0;
}

[[no_inline]] constexpr mini::int32 testConstMoveAccess()
{
    CopyableObject array[] = {
        CopyableObject(1),
        CopyableObject(2),
    };

    MoveIterator<PtrIterator<CopyableObject const*>> iter(PtrIterator<CopyableObject const*>(array + 0));
    CopyableObject copied = *iter;
    TEST_ENSURE(copied.value() == 1);
    TEST_ENSURE(array[0].resource() != nullptr);
    TEST_ENSURE(array[0].value() == 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testArrow()
{
    Base array[] = {{1}, {2}};
    MoveIterator<PtrIterator<Base*>> iter(PtrIterator<Base*>(array + 0));
    TEST_ENSURE(iter->value == 1);

    iter->value = 3;
    TEST_ENSURE(array[0].value == 3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testBase()
{
    mini::int32 array[] = {1, 2, 3};
    PtrIterator<mini::int32*> ptr(array + 1);
    IntMoveIterator iter(ptr);

    PtrIterator<mini::int32*> const& ref = iter.base();
    TEST_ENSURE(&ref == &iter.base());
    TEST_ENSURE(ref == ptr);

    PtrIterator<mini::int32*> moved = IntMoveIterator(ptr).base();
    TEST_ENSURE(moved == ptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testValid()
{
    mini::int32 array[] = {1, 2, 3};
    MoveIterator<ForwardIter<mini::int32>> begin(ForwardIter<mini::int32>(array, array + 3));
    MoveIterator<ForwardIter<mini::int32>> end(ForwardIter<mini::int32>(array + 3, array + 3));

    TEST_ENSURE(begin.valid());
    TEST_ENSURE(!end.valid());
    TEST_ENSURE(begin.validWith(end));
    TEST_ENSURE(!end.validWith(begin));

    TEST_ENSURE(begin.increment());
    TEST_ENSURE(begin.address() == array + 1);
    TEST_ENSURE(begin.increment());
    TEST_ENSURE(!begin.increment());
    TEST_ENSURE(begin.address() == array + 2);

    return 0;
}

[[no_inline]] constexpr mini::int32 testIncrementDecrement()
{
    mini::int32 array[] = {1, 2, 3, 4};
    IntMoveIterator iter(PtrIterator<mini::int32*>(array + 0));

    TEST_ENSURE(&++iter == &iter);
    TEST_ENSURE(iter.address() == array + 1);

    IntMoveIterator post = iter++;
    TEST_ENSURE(post.address() == array + 1);
    TEST_ENSURE(iter.address() == array + 2);

    TEST_ENSURE(&--iter == &iter);
    TEST_ENSURE(iter.address() == array + 1);

    post = iter--;
    TEST_ENSURE(post.address() == array + 1);
    TEST_ENSURE(iter.address() == array);

    MoveIterator<ForwardIter<mini::int32>> forward(ForwardIter<mini::int32>(array, array + 4));
    MoveIterator<ForwardIter<mini::int32>> forwardPost = forward++;
    TEST_ENSURE(forwardPost.address() == array);
    TEST_ENSURE((++forward).address() == array + 2);

    MoveIterator<BidirectionalIter<mini::int32>> bidirectional(BidirectionalIter<mini::int32>(array + 3, array + 4));
    MoveIterator<BidirectionalIter<mini::int32>> bidirectionalPost = bidirectional--;
    TEST_ENSURE(bidirectionalPost.address() == array + 3);
    TEST_ENSURE((--bidirectional).address() == array + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testArithmetic()
{
    mini::int32 array[] = {1, 2, 3, 4, 5};
    IntMoveIterator iter(PtrIterator<mini::int32*>(array + 0));

    TEST_ENSURE(&(iter += 3) == &iter);
    TEST_ENSURE(iter.address() == array + 3);
    TEST_ENSURE(&(iter -= 2) == &iter);
    TEST_ENSURE(iter.address() == array + 1);

    IntMoveIterator forward = iter + 2;
    TEST_ENSURE(forward.address() == array + 3);
    TEST_ENSURE(iter.address() == array + 1);

    IntMoveIterator backward = forward - 3;
    TEST_ENSURE(backward.address() == array);

    IntMoveIterator commuted = 3 + iter;
    TEST_ENSURE(commuted.address() == array + 4);

    TEST_ENSURE(forward - iter == 2);
    TEST_ENSURE(iter - forward == -2);

    IntConstMoveIterator citer(PtrIterator<mini::int32 const*>(array + 4));
    TEST_ENSURE(citer - iter == 3);
    TEST_ENSURE(iter - citer == -3);

    return 0;
}

[[no_inline]] constexpr mini::int32 testCompare()
{
    mini::int32 array[] = {1, 2, 3};
    IntMoveIterator first(PtrIterator<mini::int32*>(array + 0));
    IntMoveIterator second(PtrIterator<mini::int32*>(array + 1));
    IntConstMoveIterator cfirst(PtrIterator<mini::int32 const*>(array + 0));

    TEST_ENSURE(first == cfirst);
    TEST_ENSURE(cfirst == first);
    TEST_ENSURE(first != second);
    TEST_ENSURE(second != cfirst);

    TEST_ENSURE(first < second);
    TEST_ENSURE(!(second < first));
    TEST_ENSURE(second > first);
    TEST_ENSURE(!(first > second));
    TEST_ENSURE(first <= second);
    TEST_ENSURE(first <= cfirst);
    TEST_ENSURE(!(second <= first));
    TEST_ENSURE(second >= first);
    TEST_ENSURE(first >= cfirst);
    TEST_ENSURE(!(first >= second));
    TEST_ENSURE(cfirst < second);

    TEST_ENSURE((first <=> second) == std::strong_ordering::less);
    TEST_ENSURE((second <=> first) == std::strong_ordering::greater);
    TEST_ENSURE((first <=> cfirst) == std::strong_ordering::equal);

    MoveIterator<ForwardIter<mini::int32>> forward(ForwardIter<mini::int32>(array, array + 3));
    MoveIterator<ForwardIter<mini::int32 const>> cforward(ForwardIter<mini::int32 const>(array, array + 3));
    TEST_ENSURE(forward == cforward);
    TEST_ENSURE(++forward != cforward);

    return 0;
}

[[no_inline]] constexpr mini::int32 testConversion()
{
    mini::int32 array[] = {1, 2, 3};
    IntMoveIterator iter(PtrIterator<mini::int32*>(array + 1));

    IntConstMoveIterator citer(iter);
    TEST_ENSURE(citer.address() == array + 1);
    TEST_ENSURE(*citer == 2);

    IntConstMoveIterator implicit = iter;
    TEST_ENSURE(implicit == iter);

    IntConstMoveIterator assigned;
    TEST_ENSURE(&(assigned = iter) == &assigned);
    TEST_ENSURE(assigned.address() == array + 1);

    MoveIterator<ForwardIter<mini::int32>> forward(ForwardIter<mini::int32>(array, array + 3));
    MoveIterator<ForwardIter<mini::int32 const>> cforward = forward;
    TEST_ENSURE(cforward.address() == array);
    TEST_ENSURE(cforward.validWith(MoveIterator<ForwardIter<mini::int32 const>>(
        ForwardIter<mini::int32 const>(array + 3, array + 3))));

    return 0;
}

[[no_inline]] constexpr mini::int32 testCheckedMove()
{
    mini::int32 array[] = {1, 2, 3, 4};
    IntMoveIterator iter(PtrIterator<mini::int32*>(array + 0));

    TEST_ENSURE(iter.increment());
    TEST_ENSURE(iter.address() == array + 1);
    TEST_ENSURE(iter.decrement());
    TEST_ENSURE(iter.address() == array);
    TEST_ENSURE(iter.advance(3));
    TEST_ENSURE(iter.address() == array + 3);
    TEST_ENSURE(iter.advance(-2));
    TEST_ENSURE(iter.address() == array + 1);

    MoveIterator<BidirectionalIter<mini::int32>> bidirectional(BidirectionalIter<mini::int32>(array + 2, array + 4));
    TEST_ENSURE(bidirectional.decrement());
    TEST_ENSURE(bidirectional.address() == array + 1);

    return 0;
}

[[no_inline]] constexpr mini::int32 testDereferenceWithoutMove()
{
    MoveOnlyObject array[] = {
        MoveOnlyObject(1),
        MoveOnlyObject(2),
        MoveOnlyObject(3),
    };

    MoveIterator<PtrIterator<MoveOnlyObject*>> iter{PtrIterator<MoveOnlyObject*>(array + 1)};
    MoveOnlyObject&& ref = *iter;
    TEST_ENSURE(&ref == array + 1);
    TEST_ENSURE(ref.value() == 2);
    TEST_ENSURE(array[1].resource() != nullptr);

    MoveOnlyObject&& prev = iter[-1];
    TEST_ENSURE(&prev == array);
    TEST_ENSURE(array[0].resource() != nullptr);

    MoveOnlyObject moved = iter[-1];
    TEST_ENSURE(moved.value() == 1);
    TEST_ENSURE(array[0].resource() == nullptr);
    TEST_ENSURE(array[1].resource() != nullptr);
    TEST_ENSURE(array[2].resource() != nullptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testForwardMove()
{
    MoveOnlyObject source[] = {
        MoveOnlyObject(1),
        MoveOnlyObject(2),
        MoveOnlyObject(3),
    };
    MoveOnlyObject dest[] = {
        MoveOnlyObject(0),
        MoveOnlyObject(0),
        MoveOnlyObject(0),
    };

    MoveIterator<ForwardIter<MoveOnlyObject>> iter(ForwardIter<MoveOnlyObject>(source, source + 3));
    MoveIterator<ForwardIter<MoveOnlyObject>> end(ForwardIter<MoveOnlyObject>(source + 3, source + 3));

    MoveOnlyObject* out = dest;
    while (iter != end) {
        *out++ = *iter++;
    }

    TEST_ENSURE(out == dest + 3);
    TEST_ENSURE(dest[0].value() == 1);
    TEST_ENSURE(dest[1].value() == 2);
    TEST_ENSURE(dest[2].value() == 3);
    TEST_ENSURE(source[0].resource() == nullptr);
    TEST_ENSURE(source[1].resource() == nullptr);
    TEST_ENSURE(source[2].resource() == nullptr);

    return 0;
}

[[no_inline]] constexpr mini::int32 testReverseMove()
{
    MoveOnlyObject source[] = {
        MoveOnlyObject(1),
        MoveOnlyObject(2),
        MoveOnlyObject(3),
    };
    MoveOnlyObject dest[] = {
        MoveOnlyObject(0),
        MoveOnlyObject(0),
        MoveOnlyObject(0),
    };

    MoveIterator<PtrIterator<MoveOnlyObject*>> begin{PtrIterator<MoveOnlyObject*>(source)};
    MoveIterator<PtrIterator<MoveOnlyObject*>> iter{PtrIterator<MoveOnlyObject*>(source + 3)};

    MoveOnlyObject* out = dest;
    while (iter != begin) {
        *out++ = *--iter;
    }

    TEST_ENSURE(dest[0].value() == 3);
    TEST_ENSURE(dest[1].value() == 2);
    TEST_ENSURE(dest[2].value() == 1);
    TEST_ENSURE(source[0].resource() == nullptr);
    TEST_ENSURE(source[2].resource() == nullptr);

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testMoveAccess, CopyableObject);
    TEST_CONSTEXPR_TEMPLATE(testMoveAccess, MoveOnlyObject);

    TEST_CONSTEXPR(testDefault);
    TEST_CONSTEXPR(testConstMoveAccess);
    TEST_CONSTEXPR(testArrow);
    TEST_CONSTEXPR(testBase);
    TEST_CONSTEXPR(testValid);
    TEST_CONSTEXPR(testIncrementDecrement);
    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testCompare);
    TEST_CONSTEXPR(testConversion);

    TEST_CONSTEXPR(testCheckedMove);
    TEST_CONSTEXPR(testDereferenceWithoutMove);
    TEST_CONSTEXPR(testForwardMove);
    TEST_CONSTEXPR(testReverseMove);

    TEST_ENSURE(aliveCount == 0);
    TEST_ENSURE(violationCount == 0);

    return 0;
}
