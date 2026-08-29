export module mini.test;

export import mini.core;
export import :log;

export namespace mini::test {

TEST_API inline size_t ctor = 0;
TEST_API inline size_t dtor = 0;
TEST_API inline size_t copyCtor = 0;
TEST_API inline size_t moveCtor = 0;
TEST_API inline size_t copyAssign = 0;
TEST_API inline size_t moveAssign = 0;
TEST_API inline size_t debugAllocCnt = 0;

template <typename T>
inline constexpr RemoveRefT<T>& MakeLvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&>(value);
}

template <typename T>
inline constexpr RemoveRefT<T>&& MakeRvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&&>(value);
}

TEST_API inline void InitializeCounter()
{
    auto _ = Clock::Now();
    _ = Clock::Now();
    _ = Clock::Now();

    ctor = 0;
    copyCtor = 0;
    moveCtor = 0;
    copyAssign = 0;
    moveAssign = 0;
    dtor = 0;
}

TEST_API inline void PrintCounter(StringView msg, size_t time = 0)
{
    LogInfo("", "\tContainer: {} / {}", msg, time);
    LogInfo("", "\t\tConstructor: {}", ctor);
    LogInfo("", "\t\tCopy Constructor: {}", copyCtor);
    LogInfo("", "\t\tMove Constructor: {}", moveCtor);
    LogInfo("", "\t\tCopy Assign: {}", copyAssign);
    LogInfo("", "\t\tMove Assign: {}", moveAssign);
    LogInfo("", "\t\tDestructor: {}", dtor);
}

struct TEST_API Debug {
    Debug() { LogInfo("DEBUG", "constructor"); }
    ~Debug() { LogInfo("DEBUG", "destructor"); }
    Debug(Debug const&) { LogInfo("DEBUG", "copy constructor"); }
    Debug(Debug&&) noexcept { LogInfo("DEBUG", "move constructor"); }
    void operator=(Debug const&) { LogInfo("DEBUG", "copy assgin"); }
    void operator=(Debug&&) noexcept { LogInfo("DEBUG", "move assign"); }
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

struct TEST_API TestObject {
    BasicString<char> str;
    uint64 a[16];

    TestObject() : a{0, } { ++ctor; }
    TestObject(BasicString<char> const& s) : str(s), a{1, } { ++ctor; }
    TestObject(BasicString<char>&& s) : str(MoveArg(s)), a{1, } { ++ctor; }
    TestObject(TestObject const& o) : str(o.str), a{o.a[0], } { ++copyCtor; }
    TestObject(TestObject&& o) noexcept
        : str(Exchange(o.str, {})), a{1, } 
    { ++moveCtor; }

    ~TestObject() { ++dtor; }

    TestObject& operator=(TestObject const& o)
    {
        ++copyAssign;
        str = o.str;
        return *this;
    }

    TestObject& operator=(TestObject&& o) noexcept
    {
        ++moveAssign;
        str = MoveArg(o.str);
        return *this;
    }

    bool operator==(TestObject const& o) const noexcept { return str == o.str; }
};

struct TEST_API ConstexprObject {
    BasicString<char> str;

    constexpr ConstexprObject() = default;
    constexpr ConstexprObject(BasicString<char> const& s) : str(s) {}
    constexpr ConstexprObject(BasicString<char>&& s) : str(MoveArg(s)) {}

    constexpr bool operator==(ConstexprObject const& o) const noexcept { return str == o.str; }
};

struct TEST_API TestAlloc {
    typedef TestObject Value;
    typedef TestObject* Pointer;
    typedef TestObject const* ConstPointer;

    BasicString<char> str = "TestAlloc";
    Debug debug;

    template <typename U>
    constexpr DebugAlloc<U> Rebind() const noexcept { return DebugAlloc<U>{}; }

    AllocationResult<TestObject> Allocate(size_t s) { return Allocator<TestObject>{}.Allocate(s); }
    void Deallocate(Pointer ptr, size_t s) { Allocator<TestObject>{}.Deallocate(ptr, s); }
};

struct TEST_API FooDel {
    void operator()(TestObject* ptr) { delete ptr; }
};

// clang-format on

} // namespace mini::test