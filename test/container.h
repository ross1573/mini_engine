#pragma once

#include "core/define.h"
#include "core/type.h"
#include "core/log.h"
#include "memory/allocator.h"
#include "memory/deleter.h"
#include "container/iterator/common.h"
#include "container/string.h"

#include <chrono>

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

using namespace mini;

inline SizeT ctor = 0;
inline SizeT dtor = 0;
inline SizeT copyCtor = 0;
inline SizeT moveCtor = 0;
inline SizeT copyAssign = 0;
inline SizeT moveAssign = 0;
inline SizeT debugAllocCnt = 0;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MilliSecT;
typedef std::chrono::microseconds MicroSecT;
typedef Clock::time_point Time;

inline auto microSecondsCast = [](auto diff) -> SizeT
    {
        return (SizeT)std::chrono::duration_cast<MicroSecT>(diff).count();
    };

inline auto millisecondsCast = [](auto diff) -> SizeT
    {
        return (SizeT)std::chrono::duration_cast<MilliSecT>(diff).count();
    };

inline void InitializeCounter()
{
    auto _ = Clock::now();
    _ = Clock::now();
    _ = Clock::now();

    ctor = 0;
    copyCtor = 0;
    moveCtor = 0;
    copyAssign = 0;
    moveAssign = 0;
    dtor = 0;
}

inline void PrintCounter(StringView msg, SizeT time = 0)
{
    PrintLine();
    Log::Info("Container: {} / {}", msg, time);
    Log::Info("Constructor: {}", ctor);
    Log::Info("Copy Constructor: {}", copyCtor);
    Log::Info("Move Constructor: {}", moveCtor);
    Log::Info("Copy Assign: {}", copyAssign);
    Log::Info("Move Assign: {}", moveAssign);
    Log::Info("Destructor: {}", dtor);
}

struct Debug
{
    Debug() { PrintLine("[ DEBUG ] constructor"); }
    ~Debug() { PrintLine("[ DEBUG ] destructor"); }
    Debug(Debug const&) { PrintLine("[ DEBUG ] copy constructor"); }
    Debug(Debug&&) noexcept { PrintLine("[ DEBUG ] move constructor"); }
    void operator=(Debug const&) { PrintLine("[ DEBUG ] copy assgin"); }
    void operator=(Debug&&) noexcept { PrintLine("[ DEBUG ] move assign"); }
};

template <typename T>
struct DebugAlloc : public Allocator<T> 
{
    DebugAlloc() { ++debugAllocCnt; }
    ~DebugAlloc() { --debugAllocCnt; }
    DebugAlloc(DebugAlloc const&) = default;
    DebugAlloc(DebugAlloc&&) noexcept = default;
    DebugAlloc& operator=(DebugAlloc const&) = default;
    DebugAlloc& operator=(DebugAlloc&&) noexcept = default;
};

struct Foo
{
    String str;
    uint64 a[16];

    Foo() : a{0, } { ++ctor; }
    ~Foo() { ++dtor; }
    Foo(String const& s) : str(s), a{1, } { ++ctor; }
    Foo(String&& s) : str(MoveArg(s)), a{1, } { ++ctor; }
    Foo(Foo const& o) : str(o.str), a{o.a[0], } { ++copyCtor; }
    Foo(Foo&& o) noexcept : str(Exchange(o.str, {})), a{1, } 
    {
        ++moveCtor;
    }

    Foo& operator=(Foo const& o)
    {
        ++copyAssign;
        str = o.str;
        return *this;
    }

    Foo& operator=(Foo&& o) noexcept
    {
        ++moveAssign;
        str = MoveArg(o.str);
        return *this;
    }

    bool operator==(Foo const& o) const noexcept
    {
        return str == o.str;
    }
};

struct ConstExprFoo
{
    String str;

    constexpr ConstExprFoo() = default;
    constexpr ~ConstExprFoo() = default;
    constexpr ConstExprFoo(ConstExprFoo const&) = default;
    constexpr ConstExprFoo(ConstExprFoo&&) = default;
    constexpr ConstExprFoo(String const& s) : str(s) {}
    constexpr ConstExprFoo(String&& s) : str(MoveArg(s)) {}

    constexpr ConstExprFoo& operator=(ConstExprFoo const&) = default;
    constexpr ConstExprFoo& operator=(ConstExprFoo&&) = default;
    constexpr bool operator==(Foo const& o) const noexcept
    {
        return str == o.str;
    }
};

struct FooAlloc
{
    typedef Foo Value;
    typedef Foo* Ptr;

    String str = "FooAlloc";
    Debug debug;

    template <typename U>
    DebugAlloc<U> Rebind() const noexcept
    {
        return DebugAlloc<U>{};
    }

    AllocResult<Foo> Allocate(SizeT s)
    {
        return Allocator<Foo>{}.Allocate(s);
    }

    AllocResult<Foo> Increment(SizeT o, SizeT s)
    {
        return Allocator<Foo>{}.Increment(o, s);
    }

    void Deallocate(Ptr ptr, SizeT s)
    {
        Allocator<Foo>{}.Deallocate(ptr, s);
    }
};

struct FooDel
{
    void operator()(Foo* ptr)
    {
        delete ptr;
    }
};