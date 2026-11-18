export module mini.test;

export import mini.core;
export import :log;

using namespace mini;

export TEST_API inline size_t ctor = 0;
export TEST_API inline size_t dtor = 0;
export TEST_API inline size_t copyCtor = 0;
export TEST_API inline size_t moveCtor = 0;
export TEST_API inline size_t copyAssign = 0;
export TEST_API inline size_t moveAssign = 0;

export TEST_API inline size_t debugAllocCount = 0;
export TEST_API inline size_t aliveAllocCount = 0;
export TEST_API inline size_t aliveCount = 0;
export TEST_API inline size_t violationCount = 0;

export template <typename T>
constexpr RemoveRefT<T>& makeLvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&>(value);
}

export template <typename T>
constexpr RemoveRefT<T>&& makeRvalueReference(T&& value)
{
    return static_cast<RemoveRefT<T>&&>(value);
}

export TEST_API void initializeCounter()
{
    ctor = 0;
    dtor = 0;
    copyCtor = 0;
    moveCtor = 0;
    copyAssign = 0;
    moveAssign = 0;
}

export struct TEST_API TrivialObject {
public:
    int32 x;
    int32 y;

    constexpr bool operator==(TrivialObject const& o) const noexcept { return x == o.x && y == o.y; }
};

export struct TestObjectPolicy {
    bool copyable = true;
    bool defaultConstructible = true;
    size_t alignment = alignof(void*);
};

export template <typename T>
struct DebugAlloc : public Allocator<T> {
    DebugAlloc() { ++debugAllocCount; }
    ~DebugAlloc() { --debugAllocCount; }
    DebugAlloc(DebugAlloc const&) { ++debugAllocCount; }
    DebugAlloc(DebugAlloc&&) noexcept { ++debugAllocCount; }
    DebugAlloc& operator=(DebugAlloc const&) = default;
    DebugAlloc& operator=(DebugAlloc&&) noexcept = default;
};

export struct TEST_API DebugObject {
    DebugObject() { mini::test::logInfo("constructor"); }
    ~DebugObject() { mini::test::logInfo("destructor"); }
    DebugObject(DebugObject const&) { mini::test::logInfo("copy constructor"); }
    DebugObject(DebugObject&&) noexcept { mini::test::logInfo("move constructor"); }
    void operator=(DebugObject const&) { mini::test::logInfo("copy assgin"); }     // NOLINT
    void operator=(DebugObject&&) noexcept { mini::test::logInfo("move assign"); } // NOLINT
};

export template <TestObjectPolicy PolicyT>
struct TestObject;

template <typename T>
struct TestObjectTImpl : FalseT { };

template <TestObjectPolicy PolicyT>
struct TestObjectTImpl<TestObject<PolicyT>> : TrueT { };

export template <typename T>
concept TestObjectT = TestObjectTImpl<T>::value;

export using CopyableObject = TestObject<TestObjectPolicy{}>;
export using MoveOnlyObject = TestObject<TestObjectPolicy{.copyable = false}>;
export using NoDefaultObject = TestObject<TestObjectPolicy{.defaultConstructible = false}>;
export using OveralignedObject = TestObject<TestObjectPolicy{.alignment = 64}>;

template <TestObjectPolicy PolicyT>
struct alignas(PolicyT.alignment) TestObject {
public:
    static constexpr TestObjectPolicy policy = PolicyT;

private:
    TestObject const* m_self;
    int32* m_resource;

public:
    constexpr TestObject()
        requires(PolicyT.defaultConstructible);
    constexpr TestObject(TestObject const& other)
        requires(PolicyT.copyable);
    constexpr TestObject(TestObject&& other) noexcept;
    constexpr explicit TestObject(int32 value);
    constexpr ~TestObject();

    constexpr int32 value() const;
    constexpr int32* resource() const noexcept { return m_resource; }

    constexpr TestObject& operator=(TestObject const& other)
        requires(PolicyT.copyable);
    constexpr TestObject& operator=(TestObject&& other) noexcept;
    constexpr bool operator==(TestObject const& other) const;

private:
    constexpr int32* release();
    constexpr void verifyLifetime() const;
    void reportViolation(char const* reason) const noexcept;
};

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>::TestObject()
    requires(PolicyT.defaultConstructible)
    : m_self(this)
    , m_resource(new int32(0))
{
    if !consteval {
        ++ctor;
        ++aliveCount;
    }
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>::TestObject(int32 value)
    : m_self(this)
    , m_resource(new int32(value))
{
    if !consteval {
        ++ctor;
        ++aliveCount;
    }
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>::TestObject(TestObject const& other)
    requires(PolicyT.copyable)
    : m_self(this)
    , m_resource(new int32(other.value()))
{
    if !consteval {
        ++copyCtor;
        ++aliveCount;
    }
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>::TestObject(TestObject&& other) noexcept
    : m_self(this)
    , m_resource(other.release())
{
    if !consteval {
        ++moveCtor;
        ++aliveCount;
    }
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>::~TestObject()
{
    verifyLifetime();

    delete m_resource;
    m_self = nullptr;
    m_resource = nullptr;

    if !consteval {
        ++dtor;
        --aliveCount;
    }
}

template <TestObjectPolicy PolicyT>
constexpr int32 TestObject<PolicyT>::value() const
{
    verifyLifetime();

    if (m_resource == nullptr) {
        reportViolation("read of moved-from object");
        return 0;
    }

    return *m_resource;
}

template <TestObjectPolicy PolicyT>
constexpr void TestObject<PolicyT>::verifyLifetime() const
{
    if (m_self != this) {
        reportViolation("object accessed outside of its lifetime");
    }
}

template <TestObjectPolicy PolicyT>
constexpr int32* TestObject<PolicyT>::release()
{
    verifyLifetime();

    if (m_resource == nullptr) {
        reportViolation("move from moved-from object");
    }

    return exchange(m_resource, nullptr);
}

template <TestObjectPolicy PolicyT>
void TestObject<PolicyT>::reportViolation(char const* reason) const noexcept
{
    ++violationCount;
    mini::test::logError(reason);
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>& TestObject<PolicyT>::operator=(TestObject const& other)
    requires(PolicyT.copyable)
{
    verifyLifetime();

    if (this != &other) {
        int32 copied = other.value();
        delete m_resource;
        m_resource = new int32(copied);
    }

    if !consteval {
        ++copyAssign;
    }

    return *this;
}

template <TestObjectPolicy PolicyT>
constexpr TestObject<PolicyT>& TestObject<PolicyT>::operator=(TestObject&& other) noexcept
{
    verifyLifetime();

    if (this != &other) {
        int32* released = other.release();
        delete m_resource;
        m_resource = released;
    }

    if !consteval {
        ++moveAssign;
    }

    return *this;
}

template <TestObjectPolicy PolicyT>
constexpr bool TestObject<PolicyT>::operator==(TestObject const& other) const
{
    return value() == other.value();
}

export template <TestObjectT T>
struct TEST_API TestAlloc {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    BasicString<char> str = "TestAlloc";
    DebugObject debugObject;

    AllocationResult<T> allocate(size_t s) { return Allocator<T>{}.allocate(s); }
    void deallocate(Pointer ptr, size_t size) { Allocator<T>{}.deallocate(ptr, size); }
};

export template <TestObjectT T>
struct TEST_API TestDealloc {
    void operator()(T* ptr) { delete ptr; }
};

export template <typename U, TestObjectT T>
inline DebugAlloc<U> rebind(TestAlloc<T> const&)
{
    return DebugAlloc<U>{};
}

export template <typename T>
struct CountingAlloc {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    constexpr AllocationResult<T> allocate(size_t size) const noexcept;
    constexpr void deallocate(Pointer ptr, size_t size) const noexcept;
};

template <typename T>
constexpr AllocationResult<T> CountingAlloc<T>::allocate(size_t size) const noexcept
{
    if !consteval {
        ++aliveAllocCount;
    }

    return Allocator<T>{}.allocate(size);
}

template <typename T>
constexpr void CountingAlloc<T>::deallocate(Pointer ptr, size_t size) const noexcept
{
    if !consteval {
        --aliveAllocCount;
    }

    Allocator<T>{}.deallocate(ptr, size);
}

export template <typename T, typename U>
constexpr bool operator==(CountingAlloc<T> const&, CountingAlloc<U> const&) noexcept
{
    return true;
}

export template <typename U, typename T>
constexpr CountingAlloc<U> rebind(CountingAlloc<T> const&)
{
    return CountingAlloc<U>{};
}

inline TEST_API int32 testPointerPool[256] = {};

export template <typename T>
constexpr T makeObject(int32 value)
{
    if constexpr (PointerT<T>) {
        return &testPointerPool[value];
    } else if constexpr (SameAsT<T, TrivialObject>) {
        return TrivialObject{value, value};
    } else {
        return T(value);
    }
}

export template <typename T>
constexpr auto makeArgument(int32 value)
{
    if constexpr (TestObjectT<T>) {
        return value;
    } else {
        return makeObject<T>(value);
    }
}