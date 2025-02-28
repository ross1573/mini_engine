#pragma once

#include <cassert>
#include <iterator>

#define JOIN_VA_ARGS(...) __VA_ARGS__

#define ITER_CONSTRAINTS(c, stl, x) \
    { \
        typedef x Iter; \
        static_assert(c<Iter>); \
        static_assert(c<MoveIterator<Iter>>); \
        static_assert(stl<Iter>); \
        static_assert(stl<MoveIterator<Iter>>); \
    }

#define FORWARD_ITERATOR_CONSTRAINTS(...) \
    ITER_CONSTRAINTS(ForwardIteratorT, std::forward_iterator, __VA_ARGS__)

#define ITERATOR_CONSTRAINTS(...) \
    ITER_CONSTRAINTS(BidrectionalIteratorT, std::bidirectional_iterator, JOIN_VA_ARGS(__VA_ARGS__))

#define RANDOM_ACCESS_ITERATOR_CONSTRAINTS(...) \
    ITER_CONSTRAINTS(RandomAccessIteratorT, std::random_access_iterator, JOIN_VA_ARGS(__VA_ARGS__))

#define ASSERT_NOTHROW(x) \
    try { x; } catch(...) { assert(false); }

template <typename T>
concept RangeBasedForUsableT = requires(T & c, T const& cc)
{
    begin(c);
    end(c);
    cbegin(cc);
    cend(cc);
};

#define TEST_RANGE_BASED_FOR_SUPPORT(...) \
    static_assert(RangeBasedForUsableT<JOIN_VA_ARGS(__VA_ARGS__)>); \
