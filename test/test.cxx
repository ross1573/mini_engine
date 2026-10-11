export module mini.test;

export import mini.core;
export import :log;

using namespace mini;

// clang-format off

export {

TEST_API inline size_t ctor = 0;
TEST_API inline size_t dtor = 0;
TEST_API inline size_t copyCtor = 0;
TEST_API inline size_t moveCtor = 0;
TEST_API inline size_t copyAssign = 0;
TEST_API inline size_t moveAssign = 0;
TEST_API inline size_t debugAllocCnt = 0;

template <typename T>
constexpr RemoveRefT<T>& makeLvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&>(value);
}

template <typename T>
constexpr RemoveRefT<T>&& makeRvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&&>(value);
}

TEST_API inline void initializeCounter()
{
    ctor = 0;
    copyCtor = 0;
    moveCtor = 0;
    copyAssign = 0;
    moveAssign = 0;
    dtor = 0;
}

struct TEST_API DebugObject {
    DebugObject() { mini::test::logInfo("DEBUG", "constructor"); }
    ~DebugObject() { mini::test::logInfo("DEBUG", "destructor"); }
    DebugObject(DebugObject const&) { mini::test::logInfo("DEBUG", "copy constructor"); }
    DebugObject(DebugObject&&) noexcept { mini::test::logInfo("DEBUG", "move constructor"); }
    void operator=(DebugObject const&) { mini::test::logInfo("DEBUG", "copy assgin"); }
    void operator=(DebugObject&&) noexcept { mini::test::logInfo("DEBUG", "move assign"); }
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

struct TEST_API TestObject {
    BasicString<char> str;
    uint64 a[16];

    TestObject() : a{0, } { ++ctor; }
    TestObject(BasicString<char> const& s) : str(s), a{1, } { ++ctor; }
    TestObject(BasicString<char>&& s) : str(moveCast(s)), a{1, } { ++ctor; }
    TestObject(TestObject const& o) : str(o.str), a{o.a[0], } { ++copyCtor; }
    TestObject(TestObject&& o) noexcept
        : str(exchange(o.str, {})), a{1, } 
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
        str = moveCast(o.str);
        return *this;
    }

    bool operator==(TestObject const& o) const noexcept { return str == o.str; }
};

struct TEST_API ConstexprObject {
    BasicString<char> str;

    constexpr ConstexprObject() = default;
    constexpr ConstexprObject(BasicString<char> const& s) : str(s) {}
    constexpr ConstexprObject(BasicString<char>&& s) : str(moveCast(s)) {}

    constexpr bool operator==(ConstexprObject const& o) const noexcept { return str == o.str; }
};

struct TEST_API TrivialObject {
public:
    int32 x;
    int32 y;

    constexpr bool operator==(TrivialObject const& o) const noexcept { return x == o.x && y == o.y; }
};

struct TEST_API TestAlloc {
    typedef TestObject Value;
    typedef TestObject* Pointer;
    typedef TestObject const* ConstPointer;

    BasicString<char> str = "TestAlloc";
    DebugObject debugObject;

    template <typename U>
    constexpr DebugAlloc<U> Rebind() const noexcept { return DebugAlloc<U>{}; }

    AllocationResult<TestObject> allocate(size_t s) { return Allocator<TestObject>{}.allocate(s); }
    void deallocate(Pointer ptr, size_t s) { Allocator<TestObject>{}.deallocate(ptr, s); }
};

struct TEST_API FooDel {
    void operator()(TestObject* ptr) { delete ptr; }
};

} // export

// clang-format on