module;

#include <chrono>

export module mini.test;

import mini.memory;
import mini.string;
export import :log;

export namespace mini::test {

TEST_API inline SizeT ctor = 0;
TEST_API inline SizeT dtor = 0;
TEST_API inline SizeT copyCtor = 0;
TEST_API inline SizeT moveCtor = 0;
TEST_API inline SizeT copyAssign = 0;
TEST_API inline SizeT moveAssign = 0;
TEST_API inline SizeT debugAllocCnt = 0;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MilliSecT;
typedef std::chrono::microseconds MicroSecT;
typedef Clock::time_point Time;

TEST_API inline auto microSecondsCast = [](auto diff) -> SizeT {
    return (SizeT)std::chrono::duration_cast<MicroSecT>(diff).count();
};

TEST_API inline auto millisecondsCast = [](auto diff) -> SizeT {
    return (SizeT)std::chrono::duration_cast<MilliSecT>(diff).count();
};

template <typename T>
inline constexpr RemoveRefT<T>& MakeLRef(T&& value)
{
    return static_cast<RemoveRefT<T>&>(value);
}

template <typename T>
inline constexpr RemoveRefT<T>&& MakeRRef(T&& value)
{
    return static_cast<RemoveRefT<T>&&>(value);
}

TEST_API inline void InitializeCounter()
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

TEST_API inline void PrintCounter(StringView msg, SizeT time = 0)
{
    Log("", "\tContainer: {} / {}", msg, time);
    Log("", "\t\tConstructor: {}", ctor);
    Log("", "\t\tCopy Constructor: {}", copyCtor);
    Log("", "\t\tMove Constructor: {}", moveCtor);
    Log("", "\t\tCopy Assign: {}", copyAssign);
    Log("", "\t\tMove Assign: {}", moveAssign);
    Log("", "\t\tDestructor: {}", dtor);
}

struct TEST_API Debug {
    Debug() { Log("DEBUG", "constructor"); }
    ~Debug() { Log("DEBUG", "destructor"); }
    Debug(Debug const&) { Log("DEBUG", "copy constructor"); }
    Debug(Debug&&) noexcept { Log("DEBUG", "move constructor"); }
    void operator=(Debug const&) { Log("DEBUG", "copy assgin"); }
    void operator=(Debug&&) noexcept { Log("DEBUG", "move assign"); }
};

template <typename T>
struct DebugAlloc : public Allocator<T> {
    DebugAlloc() { ++debugAllocCnt; }
    ~DebugAlloc() { --debugAllocCnt; }
    DebugAlloc(DebugAlloc const&) = default;
    DebugAlloc(DebugAlloc&&) noexcept = default;
    DebugAlloc& operator=(DebugAlloc const&) = default;
    DebugAlloc& operator=(DebugAlloc&&) noexcept = default;
};

// clang-format off

struct TEST_API Foo {
    BasicString<char> str;
    uint64 a[16];

    Foo() : a{0, } { ++ctor; }
    Foo(BasicString<char> const& s) : str(s), a{1, } { ++ctor; }
    Foo(BasicString<char>&& s) : str(MoveArg(s)), a{1, } { ++ctor; }
    Foo(Foo const& o) : str(o.str), a{o.a[0], } { ++copyCtor; }
    Foo(Foo&& o) noexcept
        : str(Exchange(o.str, {})), a{1, } 
    { ++moveCtor; }

    ~Foo() { ++dtor; }

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

    bool operator==(Foo const& o) const noexcept { return str == o.str; }
};

struct TEST_API ConstexprFoo {
    BasicString<char> str;

    constexpr ConstexprFoo() = default;
    constexpr ConstexprFoo(BasicString<char> const& s) : str(s) {}
    constexpr ConstexprFoo(BasicString<char>&& s) : str(MoveArg(s)) {}

    constexpr bool operator==(ConstexprFoo const& o) const noexcept { return str == o.str; }
};

struct TEST_API FooAlloc {
    typedef Foo Value;
    typedef Foo* Ptr;

    BasicString<char> str = "FooAlloc";
    Debug debug;

    template <typename U>
    DebugAlloc<U> Rebind() const noexcept { return DebugAlloc<U>{}; }

    AllocResult<Foo> Allocate(SizeT s) { return Allocator<Foo>{}.Allocate(s); }
    AllocResult<Foo> Increment(SizeT o, SizeT s) { return Allocator<Foo>{}.Increment(o, s); }

    void Deallocate(Ptr ptr, SizeT s) { Allocator<Foo>{}.Deallocate(ptr, s); }
};

struct TEST_API FooDel {
    void operator()(Foo* ptr) { delete ptr; }
};

// clang-format on

} // namespace mini::test