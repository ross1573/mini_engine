module;

#include <chrono>

export module mini.test;

export import mini.core;

using namespace mini;

export
{

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
    LogMessage(Format("\tContainer: {} / {}", msg, time).c_str());
    LogMessage(Format("\t\tConstructor: {}", ctor).c_str());
    LogMessage(Format("\t\tCopy Constructor: {}", copyCtor).c_str());
    LogMessage(Format("\t\tMove Constructor: {}", moveCtor).c_str());
    LogMessage(Format("\t\tCopy Assign: {}", copyAssign).c_str());
    LogMessage(Format("\t\tMove Assign: {}", moveAssign).c_str());
    LogMessage(Format("\t\tDestructor: {}", dtor).c_str());
}

struct Debug
{
    Debug() { LogMessage("[ DEBUG ] constructor"); }
    ~Debug() { LogMessage("[ DEBUG ] destructor"); }
    Debug(Debug const&) { LogMessage("[ DEBUG ] copy constructor"); }
    Debug(Debug&&) noexcept { LogMessage("[ DEBUG ] move constructor"); }
    void operator=(Debug const&) { LogMessage("[ DEBUG ] copy assgin"); }
    void operator=(Debug&&) noexcept { LogMessage("[ DEBUG ] move assign"); }
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
    constexpr ConstExprFoo(String const& s) : str(s) {}
    constexpr ConstExprFoo(String&& s) : str(MoveArg(s)) {}

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

}