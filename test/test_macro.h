#pragma once

#include <cassert>
#include <iterator>
#include <iostream>
#include <format>
#include "assertion.h"

#define JOIN_VA_ARGS(...) __VA_ARGS__

#define TEST_ENSURE_EXPR(expr, var) ENSURE_EVAL(expr, var) ENSURE_EXPR(var)
#define TEST_ENSURE_INNER(expr, var, ...) \
    TEST_ENSURE_EXPR(expr, var) { EnsureHelper(#expr __VA_OPT__(, ) __VA_ARGS__); return -1; }

#define TEST_ENSURE(expr, ...) \
    TEST_ENSURE_INNER(expr, CONCAT(ensure_, __COUNTER__) __VA_OPT__(,) __VA_ARGS__)

#define TEST_ENSURE_NOTHROW(expr, ...) \
    try { expr; } catch(...) { ENSURE_LOG(expr __VA_OPT__(,) __VA_ARGS__) return -1; }

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

#define TEST_RANGE_BASED_FOR_SUPPORT(...) \
    static_assert(RangeBasedForUsableT<JOIN_VA_ARGS(__VA_ARGS__)>);

template <typename T>
concept RangeBasedForUsableT = requires(T & c, T const& cc)
{
    begin(c);
    end(c);
    cbegin(cc);
    cend(cc);
};

void EnsureHelper(char const* expr, char const* msg = nullptr,
                  std::source_location const& loc = std::source_location::current())
{
    char locBuffer[512] = {0, };
    mini::detail::SourceLocationToString(locBuffer, sizeof(locBuffer), loc);
    std::cout << std::format("\nEnsure failed!\n"
                             "  Expression: {0}\n"
                             "  Message: {1}\n"
                             "  Function: {2}\n\n",
                             expr, (msg == nullptr ? "" : msg), locBuffer);
}
    