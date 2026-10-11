module;

#if CLANG || GNUC
#  define PACKED_STRUCT_BEGIN(x) __attribute__((packed))
#  define PACKED_STRUCT_END()
#elif MSVC
#  define PACKED_STRUCT_BEGIN(x) __pragma(pack(push, x))
#  define PACKED_STRUCT_END()    __pragma(pack(pop))
#else
#  define PACKED_STRUCT_BEGIN(x)
#  define PACKED_STRUCT_END()
#endif

export module mini.core:string;

import :type;
import :assert;
import :numeric;
import :cmemory;
import :utility_operation;
import :memory_operation;
import :allocator;
import :trivial_buffer;
import :fixed_buffer;
import :array_iterator;
import :string_memory;
import :string_view;

namespace mini {

export template <CharT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class BasicString;

export using String = BasicString<char>;
export using WString = BasicString<wchar>;
export using U8String = BasicString<char8>;
export using U16String = BasicString<char16>;
export using U32String = BasicString<char32>;

template <typename T, typename U, typename AllocU>
concept StringLikeExceptStringT =
    StringLikeT<T, U> && AllocatorT<AllocU, U> && !SameAsT<RemoveConstVolatileRefT<T>, BasicString<U, AllocU>>;

template <CharT T, AllocatorT<T> AllocT>
class BasicString {
private:
    template <CharT U, AllocatorT<U> AllocU>
    friend class BasicString;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    using Iterator = ArrayIterator<Value, BasicString>;
    using ConstIterator = ArrayIterator<ConstValue, BasicString const>;

private:
    typedef memory::TrivialBuffer<T> LargeBuffer;

    struct LargeStorage {
        PACKED_STRUCT_BEGIN(1)
        struct {
            size_t layout : 1;
            size_t len    : ((sizeof(size_t) * 8) - 1);
        };
        LargeBuffer buffer;
        PACKED_STRUCT_END()
    };

    static constexpr size_t maxCapacity = 128;
    static constexpr size_t storageSize = sizeof(LargeStorage);
    static constexpr size_t allocatedSize = (storageSize / sizeof(T)) - 2;
    static constexpr size_t smallCapacity = allocatedSize > 2 ? allocatedSize : 2;
    typedef memory::FixedBuffer<T, smallCapacity + 1> SmallBuffer;

    struct SmallStorage {
        PACKED_STRUCT_BEGIN(1)
        struct {
            byte layout : 1;
            byte len    : 7;
        };
        SmallBuffer buffer;
        PACKED_STRUCT_END()
    };

    union Storage {
        SmallStorage s;
        LargeStorage l;
    };

    [[emptyable_address]] AllocT m_alloc;
    Storage m_storage;

    static_assert(smallCapacity < maxCapacity, "string maximum capacity exceeded");
    static_assert(sizeof(LargeStorage) == sizeof(SmallStorage));

public:
    constexpr BasicString() noexcept;
    constexpr ~BasicString();
    constexpr BasicString(BasicString const& other);
    constexpr BasicString(BasicString const& other, AllocT const& alloc);
    constexpr BasicString(BasicString&& other) noexcept;
    constexpr BasicString(BasicString&& other, AllocT const& alloc);
    explicit constexpr BasicString(AllocT const& alloc) noexcept;
    explicit constexpr BasicString(AllocT&& alloc) noexcept;
    explicit constexpr BasicString(size_t initCapacity, AllocT const& alloc = AllocT());
    template <typename U>
    constexpr BasicString(U ch, size_t count, AllocT const& alloc = AllocT())
        requires AnyOfT<U, Value, ConstValue>;
    template <StringLikeExceptStringT<T, AllocT> U>
    constexpr BasicString(U const& src, AllocT const& alloc = AllocT());
    template <StringLikeExceptStringT<T, AllocT> U>
    constexpr BasicString(U const& src, size_t len, AllocT const& alloc = AllocT());
    template <ForwardIteratableByT<T> Iter>
    constexpr BasicString(Iter rangeBegin, Iter rangeEnd, AllocT const& alloc = AllocT());

    constexpr void assign(BasicString const& other);
    constexpr void assign(BasicString&& other);
    template <StringLikeT<T> U>
    constexpr void assign(U const& src);
    template <StringLikeT<T> U>
    constexpr void assign(U const& src, size_t len);

    constexpr void pushBack(Value ch);
    constexpr void pushBack(Value ch, size_t count);
    template <StringLikeT<T> U>
    constexpr void append(U const& src);
    template <StringLikeT<T> U>
    constexpr void append(U const& src, size_t len);

    constexpr void insert(size_t index, Value ch);
    constexpr void insert(size_t index, Value ch, size_t count);
    template <StringLikeT<T> U>
    constexpr void insert(size_t index, U const& src);
    template <StringLikeT<T> U>
    constexpr void insert(size_t index, U const& src, size_t len);
    constexpr void insert(ConstIterator iter, Value ch);
    constexpr void insert(ConstIterator iter, Value ch, size_t count);
    template <StringLikeT<T> U>
    constexpr void insert(ConstIterator iter, U const& src);
    template <StringLikeT<T> U>
    constexpr void insert(ConstIterator iter, U const& src, size_t len);

    template <ForwardIteratableByT<T> Iter>
    constexpr void assign(Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void append(Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void insertRange(size_t index, Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void insertRange(ConstIterator iter, Iter rangeBegin, Iter rangeEnd);

    constexpr void popBack();
    constexpr void popBack(size_t count);
    constexpr void remove(size_t index);
    constexpr void remove(ConstIterator iter);
    constexpr void removeRange(size_t index, size_t count);
    constexpr void removeRange(ConstIterator iter, size_t count);
    constexpr void removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd);

    constexpr void resize(size_t len, Value ch = Value(0));
    constexpr void reserve(size_t len);
    constexpr void shrink();
    constexpr void clear();
    constexpr void swap(BasicString& other);

    [[nodiscard]] constexpr Pointer data() noexcept;
    [[nodiscard]] constexpr ConstPointer data() const noexcept;
    [[nodiscard]] constexpr Iterator begin() noexcept;
    [[nodiscard]] constexpr ConstIterator begin() const noexcept;
    [[nodiscard]] constexpr Iterator end() noexcept;
    [[nodiscard]] constexpr ConstIterator end() const noexcept;
    [[nodiscard]] constexpr Reference first();
    [[nodiscard]] constexpr ConstReference first() const;
    [[nodiscard]] constexpr Reference last();
    [[nodiscard]] constexpr ConstReference last() const;
    [[nodiscard]] constexpr Reference at(size_t index);
    [[nodiscard]] constexpr ConstReference at(size_t index) const;

    [[nodiscard]] constexpr size_t size() const noexcept;
    [[nodiscard]] constexpr size_t capacity() const noexcept;
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr bool validIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool validIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;

    [[nodiscard]] constexpr Reference operator[](size_t index);
    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    constexpr BasicString& operator=(BasicString const& other);
    constexpr BasicString& operator=(BasicString&& other) noexcept;
    template <StringLikeExceptStringT<T, AllocT> U>
    constexpr BasicString& operator=(U const& src);

    constexpr BasicString& operator+=(Value ch);
    template <StringLikeT<T> U>
    constexpr BasicString& operator+=(U const& src);

    constexpr operator BasicStringView<T>() const noexcept;

public:
    BasicString(nullptr_t) = delete;
    BasicString(nullptr_t, size_t) = delete;
    void assign(nullptr_t) = delete;
    void assign(nullptr_t, size_t) = delete;
    void append(nullptr_t) = delete;
    void append(nullptr_t, size_t) = delete;
    void insert(size_t, nullptr_t) = delete;
    void insert(size_t, nullptr_t, size_t) = delete;
    BasicString& operator=(nullptr_t) = delete;

private:
    [[nodiscard]] constexpr bool largeCapacity() const noexcept;
    constexpr void setSizeWithNullTerminator(size_t len) noexcept;
    constexpr void destroyBuffer();
    constexpr void resizeWithAlloc(size_t len, Value ch, size_t oldSize);
    constexpr void switchToLarge(LargeBuffer&& buffer, size_t len);
    constexpr void switchToSmall(Pointer ptr, size_t len);
    template <StringLikeT<T> U>
    constexpr ConstPointer slicedViewData(U const& src, size_t len);

    constexpr void initEmpty();
    constexpr void initWithCopy(BasicString const& other);
    constexpr void initWithMove(BasicString&& other) noexcept;
    constexpr Pointer initWithSize(size_t len);

    constexpr void assignWithAlloc(ConstPointer ptr, size_t len);
    constexpr void assignWithSource(ConstPointer ptr, size_t len);
    constexpr void assignWithMove(BasicString&& other) noexcept;

    template <ForwardIteratableByT<T> Iter>
    constexpr void appendWithRange(Iter rangeBegin, Iter rangeEnd, size_t len);
    constexpr void appendWithSource(ConstPointer ptr, size_t len);
    constexpr void appendWithAlloc(ConstPointer ptr, size_t len, size_t oldSize);
    constexpr void appendWithAlloc(size_t len, size_t oldSize);
    constexpr Pointer appendWithSize(size_t len);

    template <ForwardIteratableByT<T> Iter>
    constexpr void insertWithRange(size_t index, Iter rangeBegin, Iter rangeEnd, size_t len);
    constexpr void insertWithSource(size_t index, ConstPointer ptr, size_t len);
    constexpr void insertWithAlloc(size_t index, ConstPointer ptr, size_t len, size_t oldSize);
    constexpr void insertWithAlloc(size_t index, size_t len, size_t oldSize);
    constexpr Pointer insertWithSize(size_t index, size_t len);

    constexpr void assertValidIndex(size_t index) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
};

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString() noexcept
    : m_alloc{}
{
    initEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::~BasicString()
{
    if (largeCapacity()) {
        m_storage.l.buffer.deallocate(m_alloc);
        m_storage.l.len = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(BasicString const& other)
    : m_alloc{}
{
    initWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(BasicString const& other, AllocT const& alloc)
    : m_alloc(alloc)
{
    initWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(BasicString&& other) noexcept
    : m_alloc{}
{
    if (other.largeCapacity() && m_alloc != other.m_alloc) {
        initWithCopy(other);
    } else {
        initWithMove(moveCast(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(BasicString&& other, AllocT const& alloc)
    : m_alloc(alloc)
{
    if (other.largeCapacity() && m_alloc != other.m_alloc) {
        initWithCopy(other);
    } else {
        initWithMove(moveCast(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(AllocT const& alloc) noexcept
    : m_alloc(alloc)
{
    initEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(AllocT&& alloc) noexcept
    : m_alloc(moveCast(alloc))
{
    initEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::BasicString(size_t initCapacity, AllocT const& alloc)
    : m_alloc(alloc)
{
    if consteval {
        memory::constructAt(&m_storage.l);
        m_storage.l.buffer.allocate(initCapacity + 1, m_alloc);
        m_storage.l.layout = 1;
        setSizeWithNullTerminator(0);
        return;
    }

    if (initCapacity > smallCapacity) {
        memory::constructAt(&m_storage.l.buffer, initCapacity + 1, m_alloc);
        m_storage.l.buffer.data()[0] = Value(0);
        m_storage.l.layout = 1;
        m_storage.l.len = 0;
    } else {
        m_storage.s.buffer.data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.len = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <typename U>
constexpr BasicString<T, AllocT>::BasicString(U ch, size_t count, AllocT const& alloc)
    requires AnyOfT<U, Value, ConstValue>
    : m_alloc(alloc)
{
    Pointer loc = initWithSize(count);
    memory::strfill(loc, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeExceptStringT<T, AllocT> U>
constexpr BasicString<T, AllocT>::BasicString(U const& src, AllocT const& alloc)
    : m_alloc(alloc)
{
    BasicStringView<T> view = src;
    Pointer loc = initWithSize(view.size());
    memory::memcopy(loc, view.data(), view.size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeExceptStringT<T, AllocT> U>
constexpr BasicString<T, AllocT>::BasicString(U const& src, size_t len, AllocT const& alloc)
    : m_alloc(alloc)
{
    ConstPointer viewData = slicedViewData(src, len);
    Pointer loc = initWithSize(len);
    memory::memcopy(loc, viewData, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr BasicString<T, AllocT>::BasicString(Iter rangeBegin, Iter rangeEnd, AllocT const& alloc)
    : m_alloc(alloc)
{
    size_t len = distance(rangeBegin, rangeEnd);
    if (len == 0) [[unlikely]] {
        initEmpty();
        return;
    }

    Pointer loc = initWithSize(len);
    memory::copyRange(loc, rangeBegin, rangeEnd);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::assign(U const& src)
{
    BasicStringView<T> view = src;
    assignWithSource(view.data(), view.size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::assign(U const& src, size_t len)
{
    ConstPointer viewData = slicedViewData(src, len);
    assignWithSource(viewData, len);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assign(BasicString const& other)
{
    if (other.largeCapacity()) {
        assignWithAlloc(other.m_storage.l.buffer.data(), other.m_storage.l.len);
    } else {
        destroyBuffer();
        m_storage = other.m_storage;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assign(BasicString&& other)
{
    if (m_alloc == other.m_alloc) [[likely]] {
        assignWithMove(moveCast(other));
    } else {
        assignWithSource(other.data(), other.size());
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::assign(Iter rangeBegin, Iter rangeEnd)
{
    BasicString temp(rangeBegin, rangeEnd, m_alloc);
    assignWithMove(moveCast(temp));
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::pushBack(Value ch)
{
    if (ch == Value(0)) [[unlikely]] {
        return;
    }

    Pointer buffer = appendWithSize(1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::pushBack(Value ch, size_t count)
{
    if (ch == Value(0) || count == 0) [[unlikely]] {
        return;
    }

    Pointer buffer = appendWithSize(count);
    memory::strfill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::append(U const& src)
{
    BasicStringView<T> view = src;
    appendWithSource(view.data(), view.size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::append(U const& src, size_t len)
{
    ConstPointer viewData = slicedViewData(src, len);
    appendWithSource(viewData, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::append(Iter rangeBegin, Iter rangeEnd)
{
    size_t len = distance(rangeBegin, rangeEnd);
    if (len == 0) [[unlikely]] {
        return;
    }

    appendWithRange(rangeBegin, rangeEnd, len);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::insert(size_t index, Value ch)
{
    if (index == size()) {
        pushBack(ch);
        return;
    }

    assertValidIndex(index);
    Pointer loc = insertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::insert(size_t index, Value ch, size_t count)
{
    if (index == size()) {
        pushBack(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    assertValidIndex(index);
    Pointer loc = insertWithSize(index, count);
    memory::strfill(loc, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::insert(size_t index, U const& src)
{
    if (index == size()) {
        append(src);
        return;
    }

    BasicStringView<T> view = src;

    assertValidIndex(index);
    insertWithSource(index, view.data(), view.size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::insert(size_t index, U const& src, size_t len)
{
    if (index == size()) {
        append(src, len);
        return;
    }

    ConstPointer viewData = slicedViewData(src, len);

    assertValidIndex(index);
    insertWithSource(index, viewData, len);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::insert(ConstIterator iter, Value ch)
{
    size_t index = static_cast<size_t>(iter.address() - data());
    if (index == size()) {
        pushBack(ch);
        return;
    }

    assertValidIterator(iter);
    Pointer loc = insertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::insert(ConstIterator iter, Value ch, size_t count)
{
    size_t index = static_cast<size_t>(iter.address() - data());
    if (index == size()) {
        pushBack(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    assertValidIterator(iter);
    Pointer buffer = insertWithSize(index, count);
    memory::strfill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::insert(ConstIterator iter, U const& src)
{
    size_t index = static_cast<size_t>(iter.address() - data());
    if (index == size()) {
        append(src);
        return;
    }

    BasicStringView<T> view = src;

    assertValidIterator(iter);
    insertWithSource(index, view.data(), view.size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr void BasicString<T, AllocT>::insert(ConstIterator iter, U const& src, size_t len)
{
    size_t index = static_cast<size_t>(iter.address() - data());
    if (index == size()) {
        append(src, len);
        return;
    }

    ConstPointer viewData = slicedViewData(src, len);

    assertValidIterator(iter);
    insertWithSource(index, viewData, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::insertRange(size_t index, Iter rangeBegin, Iter rangeEnd)
{
    if (index == size()) {
        append(rangeBegin, rangeEnd);
        return;
    }

    size_t len = distance(rangeBegin, rangeEnd);
    if (len == 0) [[unlikely]] {
        return;
    }

    assertValidIndex(index);
    insertWithRange(index, rangeBegin, rangeEnd, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::insertRange(ConstIterator iter, Iter rangeBegin, Iter rangeEnd)
{
    size_t index = static_cast<size_t>(iter.address() - data());
    if (index == size()) {
        append(rangeBegin, rangeEnd);
        return;
    }

    size_t len = distance(rangeBegin, rangeEnd);
    if (len == 0) [[unlikely]] {
        return;
    }

    assertValidIterator(iter);
    insertWithRange(index, rangeBegin, rangeEnd, len);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::popBack()
{
    size_t oldSize = size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    setSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::popBack(size_t count)
{
    size_t oldSize = size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = oldSize < count ? oldSize : count;
    setSizeWithNullTerminator(oldSize - removeCnt);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::remove(size_t index)
{
    size_t oldSize = size();
    if (index == oldSize) [[unlikely]] {
        return;
    }

    assertValidIndex(index);
    Pointer loc = data() + index;
    memory::memmove(loc, loc + 1, oldSize - index - 1);
    setSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::remove(ConstIterator iter)
{
    Pointer buffer = data();
    size_t oldSize = size();
    size_t index = static_cast<size_t>(iter.address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    assertValidIterator(iter);
    Pointer loc = buffer + index;
    memory::memmove(loc, loc + 1, oldSize - index - 1);
    setSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::removeRange(size_t index, size_t count)
{
    size_t oldSize = size();
    if (index == oldSize) [[unlikely]] {
        return;
    }

    assertValidIndex(index);
    if (index + count >= oldSize) [[unlikely]] {
        setSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = data() + index;
    memory::memmove(loc, loc + count, oldSize - index - count);
    setSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::removeRange(ConstIterator iter, size_t count)
{
    Pointer buffer = data();
    size_t oldSize = size();
    size_t index = static_cast<size_t>(iter.address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    assertValidIterator(iter);
    if (index + count >= oldSize) [[unlikely]] {
        setSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = buffer + index;
    memory::memmove(loc, loc + count, oldSize - index - count);
    setSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd)
{
    assertValidRange(rangeBegin, rangeEnd);
    Pointer buffer = data();
    size_t oldSize = size();

    size_t count = static_cast<size_t>(rangeEnd - rangeBegin);
    if (count == 0) [[unlikely]] {
        return;
    }

    size_t index = static_cast<size_t>(rangeBegin.address() - buffer);
    size_t endIndex = static_cast<size_t>(rangeEnd.address() - buffer);
    if (endIndex == oldSize) [[unlikely]] {
        setSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = buffer + index;
    memory::memmove(loc, rangeEnd.address(), oldSize - index - count);
    setSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::resize(size_t len, Value ch)
{
    size_t oldSize = size();
    if (oldSize == len) [[unlikely]] {
        return;
    }

    if (len < oldSize) {
        setSizeWithNullTerminator(len);
        return;
    }

    if (len < capacity()) {
        memory::strfill(data() + oldSize, ch, len - oldSize);
        setSizeWithNullTerminator(len);
    } else {
        resizeWithAlloc(len, ch, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::resizeWithAlloc(size_t len, Value ch, size_t oldSize)
{
    if (!largeCapacity()) {
        LargeBuffer newBuffer(len + 1, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, m_storage.s.buffer.data(), oldSize);
        memory::strfill(buffer + oldSize, ch, len - oldSize);
        switchToLarge(moveCast(newBuffer), len);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.resize(len + 1, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, m_storage.l.buffer.data(), oldSize);
        memory::strfill(buffer + oldSize, ch, len - oldSize);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        m_storage.l.buffer.data()[len] = Value(0);
        m_storage.l.len = len;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::reserve(size_t len)
{
    if (len < capacity()) [[unlikely]] {
        return;
    }

    if (!largeCapacity()) {
        LargeBuffer buffer(len + 1, m_alloc);
        memory::memcopy(buffer.data(), m_storage.s.buffer.data(), smallCapacity);
        switchToLarge(moveCast(buffer), m_storage.s.len);
    } else {
        LargeBuffer buffer = m_storage.l.buffer.resize(len + 1, m_alloc);
        memory::memcopy(buffer.data(), m_storage.l.buffer.data(), m_storage.l.len + 1);
        m_storage.l.buffer.assign(moveCast(buffer), m_alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::shrink()
{
    if consteval {
        LargeBuffer newBuffer = m_storage.l.buffer.resize(size() + 1, m_alloc);
        memory::memcopy(newBuffer.data(), data(), size() + 1);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        return;
    }

    if (!largeCapacity()) [[unlikely]] {
        return;
    }

    size_t len = m_storage.l.len;
    if (len <= smallCapacity) {
        Pointer buffer = m_storage.l.buffer.data();
        memory::memcopy(m_storage.s.buffer.data(), buffer, len + 1);
        m_storage.s.layout = 0;
        m_storage.s.len = static_cast<byte>(len);
        m_alloc.deallocate(buffer, len);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.resize(len + 1, m_alloc);
        memory::memcopy(newBuffer.data(), m_storage.l.buffer.data(), len + 1);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::clear()
{
    setSizeWithNullTerminator(0);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::swap(BasicString& other)
{
    if consteval {
        size_t len = m_storage.l.len;
        m_storage.l.buffer.swap(other.m_storage.l.buffer);
        m_storage.l.len = other.m_storage.l.len;
        other.m_storage.l.len = len;
        return;
    }

    mini::swap(m_storage, other.m_storage);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::begin() noexcept
{
    return Iterator(data(), this);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::begin() const noexcept
{
    return ConstIterator(data(), this);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::end() noexcept
{
    return Iterator(data() + size(), this);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::end() const noexcept
{
    return ConstIterator(data() + size(), this);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::first()
{
    assertValidIndex(0);
    return *data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::first() const
{
    assertValidIndex(0);
    return *data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::last()
{
    size_t len = size();
    assertValidIndex(len - 1);
    return *(data() + len - 1);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::last() const
{
    size_t len = size();
    assertValidIndex(len - 1);
    return *(data() + len - 1);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::at(size_t index)
{
    assertValidIndex(index);
    return *(data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::at(size_t index) const
{
    assertValidIndex(index);
    return *(data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::data() noexcept
{
    return largeCapacity() ? m_storage.l.buffer.data() : m_storage.s.buffer.data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::data() const noexcept
{
    return largeCapacity() ? m_storage.l.buffer.data() : m_storage.s.buffer.data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr size_t BasicString<T, AllocT>::size() const noexcept
{
    return largeCapacity() ? m_storage.l.len : m_storage.s.len;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr size_t BasicString<T, AllocT>::capacity() const noexcept
{
    return largeCapacity() ? (m_storage.l.buffer.capacity() - 1) : smallCapacity;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr bool BasicString<T, AllocT>::empty() const noexcept
{
    return largeCapacity() ? m_storage.l.len == 0 : m_storage.s.len == 0;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr bool BasicString<T, AllocT>::validIndex(size_t index) const noexcept
{
    return index < size();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr bool BasicString<T, AllocT>::validIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.address() - data());
    return index < size();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr bool BasicString<T, AllocT>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    ConstPointer buffer = data();
    size_t len = size();
    size_t beginIdx = static_cast<size_t>(rangeBegin.address() - buffer);
    size_t endIdx = static_cast<size_t>(rangeEnd.address() - buffer);
    return (beginIdx < len) && (endIdx <= len);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::operator[](size_t index)
{
    assertValidIndex(index);
    return *(data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::operator[](size_t index) const
{
    assertValidIndex(index);
    return *(data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString const& other)
{
    assign(other);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString&& other) noexcept
{
    assign(moveCast(other));
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeExceptStringT<T, AllocT> U>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(U const& src)
{
    assign(src);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(U const& src)
{
    append(src);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(Value ch)
{
    pushBack(ch);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr bool BasicString<T, AllocT>::largeCapacity() const noexcept
{
    if consteval {
        return m_storage.l.layout == 1;
    }

    return m_storage.s.layout == 1;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::setSizeWithNullTerminator(size_t len) noexcept
{
    Pointer buffer = nullptr;
    if (largeCapacity()) {
        m_storage.l.len = len;
        buffer = m_storage.l.buffer.data();
    } else {
        m_storage.s.len = static_cast<byte>(len);
        buffer = m_storage.s.buffer.data();
    }

    buffer[len] = Value(0);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::destroyBuffer()
{
    if (!largeCapacity()) {
        return;
    }

    m_storage.l.buffer.deallocate(m_alloc);

    if consteval {
        m_storage.l.buffer.allocate(1, m_alloc);
        setSizeWithNullTerminator(0);
    } else {
        m_storage.s.buffer.data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.len = static_cast<byte>(0);
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::switchToLarge(LargeBuffer&& buffer, size_t len)
{
    memory::constructAt(&m_storage.l.buffer, moveCast(buffer));
    m_storage.l.buffer.data()[len] = Value(0);
    m_storage.l.layout = 1;
    m_storage.l.len = len;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::switchToSmall(Pointer ptr, size_t len)
{
    memory::memcopy(m_storage.s.buffer.data(), ptr, len);
    m_storage.s.buffer.data()[len] = Value(0);
    m_storage.s.layout = 0;
    m_storage.s.len = static_cast<byte>(len);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::slicedViewData(U const& src, size_t len)
{
    if constexpr (ExplicitlyConvertibleToT<U, T const*>) {
        BasicStringView<T> view(static_cast<T const*>(src), len);
        return view.data();
    } else {
        BasicStringView<T> view = src;
        BasicStringView<T> subview = view.subFront(len);
        return subview.data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::initEmpty()
{
    if consteval {
        memory::constructAt(&m_storage.l);
        m_storage.l.buffer.allocate(1, m_alloc);
        m_storage.l.layout = 1;
        setSizeWithNullTerminator(0);
    } else {
        m_storage.s.buffer.data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.len = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::initWithCopy(BasicString const& other)
{
    if consteval {
        memory::constructAt(&m_storage.l);
    }

    if (other.largeCapacity()) {
        size_t len = other.m_storage.l.len;
        memory::constructAt(&m_storage.l.buffer, len + 1, m_alloc);
        memory::memcopy(m_storage.l.buffer.data(), other.m_storage.l.buffer.data(), len + 1);
        m_storage.l.layout = 1;
        m_storage.l.len = len;
    } else {
        m_storage = other.m_storage;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::initWithMove(BasicString&& other) noexcept
{
    if consteval {
        memory::constructAt(&m_storage.l);
        m_storage.l.buffer.assign(moveCast(other.m_storage.l.buffer), m_alloc);
        m_storage.l.layout = 1;
        m_storage.l.len = other.m_storage.l.len;
        other.destroyBuffer();
        return;
    }

    m_storage = other.m_storage;
    other.initEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::initWithSize(size_t len)
{
    if consteval {
        memory::constructAt(&m_storage.l);
        m_storage.l.buffer.allocate(len + 1, m_alloc);
        m_storage.l.layout = 1;
        setSizeWithNullTerminator(len);
        return m_storage.l.buffer.data();
    }

    if (len > smallCapacity) {
        memory::constructAt(&m_storage.l.buffer, len + 1, m_alloc);
        m_storage.l.buffer.data()[len] = Value(0);
        m_storage.l.layout = 1;
        m_storage.l.len = len;
        return m_storage.l.buffer.data();
    }

    m_storage.s.buffer.data()[len] = Value(0);
    m_storage.s.layout = 0;
    m_storage.s.len = static_cast<byte>(len);
    return m_storage.s.buffer.data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assignWithMove(BasicString&& other) noexcept
{
    if consteval {
        m_storage.l.buffer.assign(moveCast(other.m_storage.l.buffer), m_alloc);
        m_storage.l.len = other.m_storage.l.len;
        other.destroyBuffer();
        return;
    }

    mini::swap(m_storage, other.m_storage);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assignWithSource(ConstPointer ptr, size_t len)
{
    if consteval {
        assignWithAlloc(ptr, len);
        return;
    }

    if (len > smallCapacity) {
        assignWithAlloc(ptr, len);
        return;
    }

    memory::memmove(data(), ptr, len);
    setSizeWithNullTerminator(len);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::assignWithAlloc(ConstPointer ptr, size_t len)
{
    if (!largeCapacity()) {
        LargeBuffer newBuffer(len + 1, m_alloc);
        memory::memcopy(newBuffer.data(), ptr, len);
        switchToLarge(moveCast(newBuffer), len);
    } else {
        Pointer buffer;
        if (m_storage.l.buffer.capacity() < len) {
            LargeBuffer newBuffer = m_storage.l.buffer.resize(len + 1, m_alloc);
            buffer = newBuffer.data();
            memory::memcopy(buffer, ptr, len);
            m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        } else {
            buffer = m_storage.l.buffer.data();
            memory::memcopy(buffer, ptr, len);
        }

        buffer[len] = Value(0);
        m_storage.l.len = len;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::appendWithRange(Iter rangeBegin, Iter rangeEnd, size_t len)
{
    size_t oldSize = size();
    size_t newSize = oldSize + len;

    if (newSize <= capacity()) {
        memory::copyRange(data() + oldSize, rangeBegin, rangeEnd);
        setSizeWithNullTerminator(newSize);
    } else {
        if (!largeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_alloc);
            Pointer buffer = newBuffer.data();
            memory::memcopy(buffer, m_storage.s.buffer.data(), oldSize);
            memory::copyRange(buffer + oldSize, rangeBegin, rangeEnd);
            switchToLarge(moveCast(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_storage.l.buffer.increment(len, m_alloc);
            Pointer buffer = newBuffer.data();
            memory::memcopy(buffer, m_storage.l.buffer.data(), oldSize);
            memory::copyRange(buffer + oldSize, rangeBegin, rangeEnd);
            m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::appendWithSource(ConstPointer ptr, size_t len)
{
    if consteval {
        appendWithAlloc(ptr, len, size());
        return;
    }

    size_t oldSize = size();
    size_t newSize = oldSize + len;

    if (newSize > capacity()) {
        appendWithAlloc(ptr, len, oldSize);
        return;
    }

    memory::memcopy(data() + oldSize, ptr, len);
    setSizeWithNullTerminator(newSize);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::appendWithAlloc(ConstPointer ptr, size_t len, size_t oldSize)
{
    size_t newSize = oldSize + len;
    Pointer oldBuffer = data();

    if (!largeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, oldBuffer, oldSize);
        memory::memcopy(buffer + oldSize, ptr, len);
        switchToLarge(moveCast(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.increment(len, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, oldBuffer, oldSize);
        memory::memcopy(buffer + oldSize, ptr, len);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        m_storage.l.buffer.data()[newSize] = Value(0);
        m_storage.l.len = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::appendWithSize(size_t len)
{
    size_t oldSize = size();
    size_t newSize = oldSize + len;

    if (newSize <= capacity()) {
        setSizeWithNullTerminator(newSize);
        return data() + oldSize;
    }

    appendWithAlloc(len, oldSize);
    return m_storage.l.buffer.data() + oldSize;
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::appendWithAlloc(size_t len, size_t oldSize)
{
    size_t newSize = oldSize + len;

    if (!largeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        memory::memcopy(newBuffer.data(), m_storage.s.buffer.data(), oldSize);
        switchToLarge(moveCast(newBuffer), newSize);
    } else {
        LargeBuffer buffer = m_storage.l.buffer.increment(len, m_alloc);
        memory::memcopy(buffer.data(), m_storage.l.buffer.data(), oldSize);
        m_storage.l.buffer.assign(moveCast(buffer), m_alloc);
        m_storage.l.buffer.data()[newSize] = Value(0);
        m_storage.l.len = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::insertWithSource(size_t index, ConstPointer ptr, size_t len)
{
    size_t oldSize = size();
    size_t newSize = oldSize + len;

    if (newSize <= capacity()) {
        Pointer oldBuffer = data();
        Pointer loc = oldBuffer + index;

        if (pointerOverlapping(ptr, oldBuffer, oldBuffer + oldSize)) {
            BasicString temp(ptr, len, m_alloc);
            ptr = temp.data();
        }

        memory::memmove(loc + len, loc, oldSize - index);
        memory::memcopy(loc, ptr, len);
        setSizeWithNullTerminator(newSize);
    } else {
        insertWithAlloc(index, ptr, len, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::insertWithAlloc(size_t index,
                                                                     ConstPointer ptr,
                                                                     size_t len,
                                                                     size_t oldSize)
{
    size_t newSize = oldSize + len;
    Pointer oldBuffer = data();
    Pointer loc = oldBuffer + index;

    if (!largeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, oldBuffer, index);
        memory::memcopy(buffer + index + len, loc, oldSize - index + 1);
        memory::memcopy(buffer + index, ptr, len);
        switchToLarge(moveCast(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.increment(len, m_alloc);
        Pointer buffer = newBuffer.data();
        memory::memcopy(buffer, oldBuffer, index);
        memory::memcopy(buffer + index + len, loc, oldSize - index + 1);
        memory::memcopy(buffer + index, ptr, len);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        m_storage.l.len = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::insertWithSize(size_t index, size_t len)
{
    size_t oldSize = size();
    size_t newSize = oldSize + len;

    if (newSize <= capacity()) {
        Pointer loc = data() + index;
        memory::memmove(loc + len, loc, oldSize - index);
        setSizeWithNullTerminator(newSize);
        return loc;
    }

    insertWithAlloc(index, len, oldSize);
    return m_storage.l.buffer.data() + index;
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::insertWithAlloc(size_t index, size_t len, size_t oldSize)
{
    size_t newSize = oldSize + len;

    if (!largeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.data();
        Pointer oldBuffer = m_storage.s.buffer.data();

        memory::memcopy(buffer, oldBuffer, index);
        memory::memcopy(buffer + index + len, oldBuffer + index, oldSize - index);
        switchToLarge(moveCast(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.increment(len, m_alloc);
        Pointer buffer = newBuffer.data();
        Pointer oldBuffer = m_storage.l.buffer.data();

        memory::memcopy(buffer, oldBuffer, index);
        memory::memcopy(buffer + index + len, oldBuffer + index, oldSize - index);
        m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
        m_storage.l.buffer.data()[newSize] = Value(0);
        m_storage.l.len = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void BasicString<T, AllocT>::insertWithRange(size_t index, Iter rangeBegin, Iter rangeEnd, size_t len)
{
    size_t oldSize = size();
    size_t newSize = oldSize + len;
    Pointer oldBuffer = data();
    Pointer loc = oldBuffer + index;

    if (newSize <= capacity()) {
        BasicString temp(len, m_alloc);
        Pointer buffer = temp.data();
        memory::copyRange(buffer, rangeBegin, rangeEnd);
        memory::memmove(loc + len, loc, oldSize - index);
        memory::memcopy(loc, temp.data(), len);
        setSizeWithNullTerminator(newSize);
    } else {
        if (!largeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_alloc);
            Pointer buffer = newBuffer.data();
            memory::memcopy(buffer, oldBuffer, index);
            memory::memcopy(buffer + index + len, loc, oldSize - index);
            memory::copyRange(buffer + index, rangeBegin, rangeEnd);
            switchToLarge(moveCast(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_storage.l.buffer.increment(len, m_alloc);
            Pointer buffer = newBuffer.data();
            memory::memcopy(buffer, oldBuffer, index);
            memory::memcopy(buffer + index + len, loc, oldSize - index);
            memory::copyRange(buffer + index, rangeBegin, rangeEnd);
            m_storage.l.buffer.assign(moveCast(newBuffer), m_alloc);
            m_storage.l.buffer.data()[newSize] = Value(0);
            m_storage.l.len = newSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>::operator BasicStringView<T>() const noexcept
{
    return BasicStringView(data(), size());
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(validIndex(index), "invalid index {}. current len is {}", index, size());
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter), "invalid iterator at index {}. current len is {}", iter.address() - data(), size());
}

template <CharT T, AllocatorT<T> AllocT>
constexpr void BasicString<T, AllocT>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                                        [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd),
           "invalid range from {} to {}. current len is {}",
           rangeBegin.address() - data(),
           rangeEnd.address() - data(),
           size());
}

export template <CharT T, AllocatorT<T> AllocT>
constexpr bool operator==(BasicString<T, AllocT> const& lhs, BasicString<T, AllocT> const& rhs) noexcept
{
    size_t len = lhs.size();
    if (len != rhs.size()) {
        return false;
    }

    using Pointer = BasicString<T, AllocT>::ConstPointer;
    Pointer lbuf = lhs.data();
    Pointer rbuf = rhs.data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::strcomp(lbuf, rbuf, len) == 0;
}

export template <CharT T, AllocatorT<T> AllocT, CharT U, AllocatorT<U> AllocU>
constexpr bool operator==(BasicString<T, AllocT> const& lhs, BasicString<U, AllocU> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t len = lhs.size();
    if (len != rhs.size()) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPointer lbuf = lhs.data();
    typename BasicString<U, AllocU>::ConstPointer rbuf = rhs.data();
    return memory::equalRange(lbuf, rbuf, rbuf + len);
}

export template <CharT T, AllocatorT<T> AllocT, StringLikeExceptStringT<T, AllocT> ViewU>
constexpr bool operator==(BasicString<T, AllocT> const& str, ViewU const& view) noexcept
{
    BasicStringView<T> lhs = str;
    BasicStringView<T> rhs = view;
    return lhs == rhs;
}

export template <CharT T, AllocatorT<T> AllocT, CharT U, AllocatorT<U> AllocU, StringLikeExceptStringT<U, AllocU> ViewU>
constexpr bool operator==(BasicString<T, AllocT> const& str, ViewU const& view) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<T> lhs = str;
    BasicStringView<U> rhs = view;
    return lhs == rhs;
}

export template <CharT T, AllocatorT<T> AllocT>
bool operator==(BasicString<T, AllocT> const&, nullptr_t) = delete;

export template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT> const& lhs,
                                           typename BasicString<T, AllocT>::Value rhs)
{
    BasicString<T, AllocT> str = lhs;
    str.pushBack(rhs);
    return str;
}

export template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT>&& lhs, typename BasicString<T, AllocT>::Value rhs)
{
    lhs.pushBack(rhs);
    return moveCast(lhs);
}

export template <CharT T, AllocatorT<T> AllocT, StringLikeT<T> ViewT>
constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT> const& lhs, ViewT const& rhs)
{
    BasicString<T, AllocT> str = lhs;
    str.append(rhs);
    return str;
}

export template <CharT T, AllocatorT<T> AllocT, StringLikeT<T> ViewT>
constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT>&& lhs, ViewT const& rhs)
{
    lhs.append(rhs);
    return moveCast(lhs);
}

export template <CharT T, AllocatorT<T> AllocT>
constexpr void swap(BasicString<T, AllocT> const& lhs, BasicString<T, AllocT> const& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mini