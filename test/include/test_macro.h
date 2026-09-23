#pragma once

#include <cstring>
#include <iterator>

#include "config.h"
#include "debug/assert.h"

#if MSVC
#  define TEST_ENSURE(expr, ...)                        \
      if consteval {                                    \
          if (::mini::debug::Evaluate(expr) == false) { \
              memcpy(nullptr, nullptr, 0);              \
          }                                             \
      } else {                                          \
          if (::mini::debug::Evaluate(expr) == false) { \
              return -1;                                \
          }                                             \
      }
#else
#  define TEST_ENSURE(expr, ...)                                          \
      if consteval {                                                      \
          if (::mini::debug::Evaluate(expr) == false) {                   \
              memcpy(nullptr, nullptr, 0);                                \
          }                                                               \
      } else {                                                            \
          if (::mini::debug::Evaluate(expr) == false) {                   \
              ::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__); \
              return -1;                                                  \
          }                                                               \
      }
#endif

#define TEST_ENSURE_NOTHROW(expr, ...)                              \
    try {                                                           \
        expr;                                                       \
    } catch (...) {                                                 \
        ::mini::debug::LogEnsure(#expr __VA_OPT__(, ) __VA_ARGS__); \
        return -1;                                                  \
    }

#define ITER_CONSTRAINTS(c, stl, x)             \
    {                                           \
        typedef x Iter;                         \
        static_assert(c<Iter>);                 \
        static_assert(c<MoveIterator<Iter>>);   \
        static_assert(stl<Iter>);               \
        static_assert(stl<MoveIterator<Iter>>); \
    }

#define FORWARD_ITERATOR_CONSTRAINTS(...) ITER_CONSTRAINTS(ForwardIteratorT, std::forward_iterator, __VA_ARGS__)

#define ITERATOR_CONSTRAINTS(...)                                                                   \
    ITER_CONSTRAINTS(BidrectionalIteratorT, std::bidirectional_iterator, JOIN_VA_ARGS(__VA_ARGS__))

#define RANDOM_ACCESS_ITERATOR_CONSTRAINTS(...)                                                     \
    ITER_CONSTRAINTS(RandomAccessIteratorT, std::random_access_iterator, JOIN_VA_ARGS(__VA_ARGS__))

#define TEST_RANGE_BASED_FOR_SUPPORT(...)                           \
    static_assert(requires(JOIN_VA_ARGS(__VA_ARGS__) & container) { \
        begin(container);                                           \
        end(container);                                             \
    });

#define TEST_CONST_RANGE_BASED_FOR_SUPPORT(...)                          \
    static_assert(requires(JOIN_VA_ARGS(__VA_ARGS__) const& container) { \
        begin(container);                                                \
        end(container);                                                  \
        cbegin(container);                                               \
        cend(container);                                                 \
    });