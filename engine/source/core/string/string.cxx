module;

#include <string>
#include <string_view>

#if CLANG || GNUC
#  define PACKED_STRUCT_BEGIN(x) __attribute__((packed))
#  define PACKED_STRUCT_END
#elif MSVC
#  define PACKED_STRUCT_BEGIN(x) __pragma(pack(push, x))
#  define PACKED_STRUCT_END      __pragma(pack(pop))
#else
#  define PACKED_STRUCT_BEGIN(x)
#  define PACKED_STRUCT_END
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
concept StringViewLikeT = StringLikeT<T, U> && AllocatorT<AllocU, U> &&
                          !SameAsT<RemoveConstVolatileRefT<T>, BasicString<U, AllocU>>;

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
            size_t size   : (sizeof(size_t) * 8 - 1);
        };
        LargeBuffer buffer;
        PACKED_STRUCT_END
    };

    static constexpr size_t StorageSize = sizeof(LargeStorage);
    static constexpr size_t AllocatedSize = (StorageSize / sizeof(T)) - 2;
    static constexpr size_t SmallCapacity = AllocatedSize > 2 ? AllocatedSize : 2;
    typedef memory::FixedBuffer<T, SmallCapacity + 1> SmallBuffer;

    struct SmallStorage {
        PACKED_STRUCT_BEGIN(1)
        struct {
            byte layout : 1;
            byte size   : 7;
        };
        SmallBuffer buffer;
        PACKED_STRUCT_END
    };

    union Storage {
        SmallStorage s;
        LargeStorage l;
    };

    [[emptyable_address]] AllocT m_alloc;
    Storage m_storage;

    static_assert(SmallCapacity < 128, "small capacity should not excced 127");
    static_assert(sizeof(LargeStorage) == sizeof(SmallStorage));

public:
    constexpr BasicString() noexcept;
    constexpr ~BasicString();
    constexpr BasicString(BasicString const&);
    constexpr BasicString(BasicString const&, AllocT const&);
    constexpr BasicString(BasicString&&);
    constexpr BasicString(BasicString&&, AllocT const&);
    explicit constexpr BasicString(AllocT const&) noexcept;
    explicit constexpr BasicString(AllocT&&) noexcept;
    constexpr BasicString(size_t, AllocT const& = AllocT());
    template <typename U>
    constexpr BasicString(U, size_t, AllocT const& = AllocT())
        requires AnyOfT<U, Value, ConstValue>;
    template <StringViewLikeT<T, AllocT> U>
    constexpr BasicString(U const&, AllocT const& = AllocT());
    template <StringViewLikeT<T, AllocT> U>
    constexpr BasicString(U const&, size_t, AllocT const& = AllocT());
    template <ForwardIteratableByT<T> Iter>
    constexpr BasicString(Iter, Iter, AllocT const& = AllocT());

    constexpr void Assign(BasicString const&);
    constexpr void Assign(BasicString&&);
    template <StringLikeT<T> U>
    constexpr void Assign(U const&);
    template <StringLikeT<T> U>
    constexpr void Assign(U const&, size_t);

    constexpr void Push(Value);
    constexpr void Push(Value, size_t);
    template <StringLikeT<T> U>
    constexpr void Append(U const&);
    template <StringLikeT<T> U>
    constexpr void Append(U const&, size_t);

    constexpr void Insert(size_t, Value);
    constexpr void Insert(size_t, Value, size_t);
    template <StringLikeT<T> U>
    constexpr void Insert(size_t, U const&);
    template <StringLikeT<T> U>
    constexpr void Insert(size_t, U const&, size_t);
    constexpr void Insert(ConstIterator, Value);
    constexpr void Insert(ConstIterator, Value, size_t);
    template <StringLikeT<T> U>
    constexpr void Insert(ConstIterator, U const&);
    template <StringLikeT<T> U>
    constexpr void Insert(ConstIterator, U const&, size_t);

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(size_t, Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator, Iter, Iter);

    constexpr void RemoveLast();
    constexpr void RemoveLast(size_t);
    constexpr void RemoveAt(size_t);
    constexpr void RemoveAt(ConstIterator);
    constexpr void RemoveRange(size_t, size_t);
    constexpr void RemoveRange(ConstIterator, size_t);
    constexpr void RemoveRange(ConstIterator, ConstIterator);

    constexpr void Resize(size_t, Value = Value(0));
    constexpr void Reserve(size_t);
    constexpr void Shrink();
    constexpr void Clear();
    constexpr void Swap(BasicString&);

    constexpr Pointer Data() noexcept;
    constexpr ConstPointer Data() const noexcept;
    constexpr Iterator Begin() noexcept;
    constexpr ConstIterator Begin() const noexcept;
    constexpr Iterator End() noexcept;
    constexpr ConstIterator End() const noexcept;
    constexpr Reference First();
    constexpr ConstReference First() const;
    constexpr Reference Last();
    constexpr ConstReference Last() const;
    constexpr Reference At(size_t);
    constexpr ConstReference At(size_t) const;

    constexpr size_t Size() const noexcept;
    constexpr size_t Capacity() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool ValidIndex(size_t) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Reference operator[](size_t);
    constexpr ConstReference operator[](size_t) const;

    constexpr BasicString& operator=(BasicString const&);
    constexpr BasicString& operator=(BasicString&&);
    template <StringViewLikeT<T, AllocT> U>
    constexpr BasicString& operator=(U const&);

    constexpr BasicString& operator+=(Value);
    template <StringLikeT<T> U>
    constexpr BasicString& operator+=(U const&);

    constexpr operator BasicStringView<T>() const noexcept;
    constexpr operator std::basic_string<T>() const;
    constexpr operator std::basic_string_view<T>() const noexcept;

    template <typename TraitsT, typename StdAllocT>
    constexpr BasicString(std::basic_string<T, TraitsT, StdAllocT> const&, AllocT const& = AllocT());

private:
    BasicString(nullptr_t) = delete;
    BasicString(nullptr_t, size_t) = delete;
    void Assign(nullptr_t) = delete;
    void Assign(nullptr_t, size_t) = delete;
    void Append(nullptr_t) = delete;
    void Append(nullptr_t, size_t) = delete;
    void Insert(size_t, nullptr_t) = delete;
    void Insert(size_t, nullptr_t, size_t) = delete;
    BasicString& operator=(nullptr_t) = delete;

    constexpr bool LargeCapacity() const noexcept;
    constexpr void SetSizeWithNullTerminator(size_t) noexcept;
    constexpr void DestroyBuffer();
    constexpr void ResizeWithAlloc(size_t, Value, size_t);
    constexpr void SwitchToLarge(LargeBuffer&&, size_t);
    constexpr void SwitchToSmall(Pointer, size_t);
    template <StringLikeT<T> U>
    constexpr ConstPointer GetSlicedViewData(U const&, size_t);

    constexpr void InitEmpty();
    constexpr void InitWithCopy(BasicString const&);
    constexpr void InitWithMove(BasicString&&) noexcept;
    constexpr Pointer InitWithSize(size_t);

    constexpr void AssignWithAlloc(ConstPointer, size_t);
    constexpr void AssignWithSource(ConstPointer, size_t);
    constexpr void AssignWithMove(BasicString&&) noexcept;

    template <ForwardIteratableByT<T> Iter>
    constexpr void AppendWithRange(Iter, Iter, size_t);
    constexpr void AppendWithSource(ConstPointer, size_t);
    constexpr void AppendWithAlloc(ConstPointer, size_t, size_t);
    constexpr void AppendWithAlloc(size_t, size_t);
    constexpr Pointer AppendWithSize(size_t);

    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertWithRange(size_t, Iter, Iter, size_t);
    constexpr void InsertWithSource(size_t, ConstPointer, size_t);
    constexpr void InsertWithAlloc(size_t, ConstPointer, size_t, size_t);
    constexpr void InsertWithAlloc(size_t, size_t, size_t);
    constexpr Pointer InsertWithSize(size_t, size_t);

    constexpr void AssertValidIndex(size_t) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString() noexcept
    : m_alloc{ }
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::~BasicString()
{
    if (LargeCapacity()) {
        m_storage.l.buffer.Deallocate(m_alloc);
        m_storage.l.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other)
    : m_alloc{ }
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other, AllocT const& alloc)
    : m_alloc(alloc)
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other)
    : m_alloc{ }
{
    if (other.LargeCapacity() && m_alloc != other.m_alloc) {
        InitWithCopy(other);
    } else {
        InitWithMove(MoveArg(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other, AllocT const& alloc)
    : m_alloc(alloc)
{
    if (other.LargeCapacity() && m_alloc != other.m_alloc) {
        InitWithCopy(other);
    } else {
        InitWithMove(MoveArg(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT const& alloc) noexcept
    : m_alloc(alloc)
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT&& alloc) noexcept
    : m_alloc(MoveArg(alloc))
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(size_t capacity, AllocT const& alloc)
    : m_alloc(alloc)
{
    if consteval {
        memory::ConstructAt(&m_storage.l);
        m_storage.l.buffer.Allocate(capacity + 1, m_alloc);
        m_storage.l.layout = 1;
        SetSizeWithNullTerminator(0);
        return;
    }

    if (capacity > SmallCapacity) {
        memory::ConstructAt(&m_storage.l.buffer, capacity + 1, m_alloc);
        m_storage.l.buffer.Data()[0] = Value(0);
        m_storage.l.layout = 1;
        m_storage.l.size = 0;
    } else {
        m_storage.s.buffer.Data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr BasicString<T, AllocT>::BasicString(U ch, size_t size, AllocT const& alloc)
    requires AnyOfT<U, Value, ConstValue>
    : m_alloc(alloc)
{
    Pointer loc = InitWithSize(size);
    memory::StringFill(loc, ch, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringViewLikeT<T, AllocT> U>
inline constexpr BasicString<T, AllocT>::BasicString(U const& src, AllocT const& alloc)
    : m_alloc(alloc)
{
    BasicStringView<T> view = src;
    Pointer loc = InitWithSize(view.Size());
    memory::MemCopy(loc, view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringViewLikeT<T, AllocT> U>
inline constexpr BasicString<T, AllocT>::BasicString(U const& src, size_t size, AllocT const& alloc)
    : m_alloc(alloc)
{
    ConstPointer data = GetSlicedViewData(src, size);
    Pointer loc = InitWithSize(size);
    memory::MemCopy(loc, data, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr BasicString<T, AllocT>::BasicString(Iter begin, Iter end, AllocT const& alloc)
    : m_alloc(alloc)
{
    size_t size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        InitEmpty();
        return;
    }

    Pointer loc = InitWithSize(size);
    memory::CopyRange(loc, begin, end);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Assign(U const& src)
{
    BasicStringView<T> view = src;
    AssignWithSource(view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Assign(U const& src, size_t size)
{
    ConstPointer data = GetSlicedViewData(src, size);
    AssignWithSource(data, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString const& other)
{
    if (other.LargeCapacity()) {
        AssignWithAlloc(other.m_storage.l.buffer.Data(), other.m_storage.l.size);
    } else {
        DestroyBuffer();
        m_storage = other.m_storage;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString&& other)
{
    if (m_alloc == other.m_alloc) [[likely]] {
        AssignWithMove(MoveArg(other));
    } else {
        AssignWithSource(other.Data(), other.Size());
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Assign(Iter begin, Iter end)
{
    BasicString temp(begin, end, m_alloc);
    AssignWithMove(MoveArg(temp));
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Push(Value ch)
{
    if (ch == Value(0)) [[unlikely]] {
        return;
    }

    Pointer buffer = AppendWithSize(1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Push(Value ch, size_t count)
{
    if (ch == Value(0) || count == 0) [[unlikely]] {
        return;
    }

    Pointer buffer = AppendWithSize(count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Append(U const& src)
{
    BasicStringView<T> view = src;
    AppendWithSource(view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Append(U const& src, size_t size)
{
    ConstPointer data = GetSlicedViewData(src, size);
    AppendWithSource(data, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Append(Iter begin, Iter end)
{
    size_t size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    AppendWithRange(begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(size_t index, Value ch)
{
    if (index == Size()) {
        Push(ch);
        return;
    }

    AssertValidIndex(index);
    Pointer loc = InsertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(size_t index, Value ch, size_t count)
{
    if (index == Size()) {
        Push(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    Pointer loc = InsertWithSize(index, count);
    memory::StringFill(loc, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Insert(size_t index, U const& src)
{
    if (index == Size()) {
        Append(src);
        return;
    }

    BasicStringView<T> view = src;

    AssertValidIndex(index);
    InsertWithSource(index, view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Insert(size_t index, U const& src, size_t len)
{
    if (index == Size()) {
        Append(src, len);
        return;
    }

    ConstPointer data = GetSlicedViewData(src, len);

    AssertValidIndex(index);
    InsertWithSource(index, data, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch)
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    if (index == Size()) {
        Push(ch);
        return;
    }

    AssertValidIterator(iter);
    Pointer loc = InsertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch, size_t count)
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    if (index == Size()) {
        Push(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Pointer buffer = InsertWithSize(index, count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, U const& src)
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    if (index == Size()) {
        Append(src);
        return;
    }

    BasicStringView<T> view = src;

    AssertValidIterator(iter);
    InsertWithSource(index, view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, U const& src, size_t len)
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    if (index == Size()) {
        Append(src, len);
        return;
    }

    ConstPointer data = GetSlicedViewData(src, len);

    AssertValidIterator(iter);
    InsertWithSource(index, data, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertRange(size_t index, Iter begin, Iter end)
{
    if (index == Size()) {
        Append(begin, end);
        return;
    }

    size_t size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    InsertWithRange(index, begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertRange(ConstIterator iter, Iter begin, Iter end)
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    if (index == Size()) {
        Append(begin, end);
        return;
    }

    size_t size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithRange(index, begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast()
{
    size_t oldSize = Size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast(size_t count)
{
    size_t oldSize = Size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = oldSize < count ? oldSize : count;
    SetSizeWithNullTerminator(oldSize - removeCnt);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(size_t index)
{
    size_t oldSize = Size();
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    Pointer loc = Data() + index;
    memory::MemMove(loc, loc + 1, oldSize - index - 1);
    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(ConstIterator iter)
{
    Pointer buffer = Data();
    size_t oldSize = Size();
    size_t index = static_cast<size_t>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Pointer loc = buffer + index;
    memory::MemMove(loc, loc + 1, oldSize - index - 1);
    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(size_t index, size_t count)
{
    size_t oldSize = Size();
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    if (index + count >= oldSize) [[unlikely]] {
        SetSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = Data() + index;
    memory::MemMove(loc, loc + count, oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator iter, size_t count)
{
    Pointer buffer = Data();
    size_t oldSize = Size();
    size_t index = static_cast<size_t>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    if (index + count >= oldSize) [[unlikely]] {
        SetSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = buffer + index;
    memory::MemMove(loc, loc + count, oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator begin, ConstIterator end)
{
    AssertValidRange(begin, end);
    Pointer buffer = Data();
    size_t oldSize = Size();

    size_t count = static_cast<size_t>(end - begin);
    if (count == 0) [[unlikely]] {
        return;
    }

    size_t index = static_cast<size_t>(begin.Address() - buffer);
    size_t endIndex = static_cast<size_t>(end.Address() - buffer);
    if (endIndex == oldSize) [[unlikely]] {
        SetSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = buffer + index;
    memory::MemMove(loc, end.Address(), oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Resize(size_t size, Value ch)
{
    size_t oldSize = Size();
    if (oldSize == size) [[unlikely]] {
        return;
    } else if (size < oldSize) {
        SetSizeWithNullTerminator(size);
        return;
    }

    if (size < Capacity()) {
        memory::StringFill(Data() + oldSize, ch, size - oldSize);
        SetSizeWithNullTerminator(size);
    } else {
        ResizeWithAlloc(size, ch, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::ResizeWithAlloc(size_t size, Value ch, size_t oldSize)
{
    if (!LargeCapacity()) {
        LargeBuffer newBuffer(size + 1, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, m_storage.s.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        SwitchToLarge(MoveArg(newBuffer), size);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.Resize(size + 1, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, m_storage.l.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        m_storage.l.buffer.Data()[size] = Value(0);
        m_storage.l.size = size;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Reserve(size_t size)
{
    if (size < Capacity()) [[unlikely]] {
        return;
    }

    if (!LargeCapacity()) {
        LargeBuffer buffer(size + 1, m_alloc);
        memory::MemCopy(buffer.Data(), m_storage.s.buffer.Data(), SmallCapacity);
        SwitchToLarge(MoveArg(buffer), m_storage.s.size);
    } else {
        LargeBuffer buffer = m_storage.l.buffer.Resize(size + 1, m_alloc);
        memory::MemCopy(buffer.Data(), m_storage.l.buffer.Data(), m_storage.l.size + 1);
        m_storage.l.buffer.Assign(MoveArg(buffer), m_alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Shrink()
{
    if consteval {
        LargeBuffer newBuffer = m_storage.l.buffer.Resize(Size() + 1, m_alloc);
        memory::MemCopy(newBuffer.Data(), Data(), Size() + 1);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        return;
    }

    if (!LargeCapacity()) [[unlikely]] {
        return;
    }

    size_t size = m_storage.l.size;
    if (size <= SmallCapacity) {
        Pointer buffer = m_storage.l.buffer.Data();
        memory::MemCopy(m_storage.s.buffer.Data(), buffer, size + 1);
        m_storage.s.layout = 0;
        m_storage.s.size = static_cast<byte>(size);
        m_alloc.Deallocate(buffer, size);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.Resize(size + 1, m_alloc);
        memory::MemCopy(newBuffer.Data(), m_storage.l.buffer.Data(), size + 1);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Clear()
{
    SetSizeWithNullTerminator(0);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Swap(BasicString& other)
{
    if consteval {
        size_t size = m_storage.l.size;
        m_storage.l.buffer.Swap(other.m_storage.l.buffer);
        m_storage.l.size = other.m_storage.l.size;
        other.m_storage.l.size = size;
        return;
    }

    mini::Swap(m_storage, other.m_storage);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::Begin() noexcept
{
    return Iterator(Data(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::Begin() const noexcept
{
    return ConstIterator(Data(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::End() noexcept
{
    return Iterator(Data() + Size(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::End() const noexcept
{
    return ConstIterator(Data() + Size(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::Last()
{
    size_t size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::Last() const
{
    size_t size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::At(size_t index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::Data() noexcept
{
    return LargeCapacity() ? m_storage.l.buffer.Data() : m_storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::Data() const noexcept
{
    return LargeCapacity() ? m_storage.l.buffer.Data() : m_storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr size_t BasicString<T, AllocT>::Size() const noexcept
{
    return LargeCapacity() ? m_storage.l.size : m_storage.s.size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr size_t BasicString<T, AllocT>::Capacity() const noexcept
{
    return LargeCapacity() ? (m_storage.l.buffer.Capacity() - 1) : SmallCapacity;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::Empty() const noexcept
{
    return LargeCapacity() ? m_storage.l.size == 0 : m_storage.s.size == 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidIndex(size_t index) const noexcept
{
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.Address() - Data());
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    ConstPointer buffer = Data();
    size_t size = Size();
    size_t beginIdx = static_cast<size_t>(begin.Address() - buffer);
    size_t endIdx = static_cast<size_t>(end.Address() - buffer);
    return (beginIdx < size) && (endIdx <= size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::operator[](size_t index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString const& other)
{
    Assign(other);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString&& other)
{
    Assign(MoveArg(other));
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
template <StringViewLikeT<T, AllocT> U>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(U const& src)
{
    Assign(src);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(U const& src)
{
    Append(src);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(Value ch)
{
    Push(ch);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::LargeCapacity() const noexcept
{
    if consteval {
        return m_storage.l.layout == 1;
    }

    return m_storage.s.layout == 1;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SetSizeWithNullTerminator(size_t size) noexcept
{
    Pointer buffer = nullptr;
    if (LargeCapacity()) {
        m_storage.l.size = size;
        buffer = m_storage.l.buffer.Data();
    } else {
        m_storage.s.size = static_cast<byte>(size);
        buffer = m_storage.s.buffer.Data();
    }

    buffer[size] = Value(0);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::DestroyBuffer()
{
    if (!LargeCapacity()) {
        return;
    }

    m_storage.l.buffer.Deallocate(m_alloc);

    if consteval {
        m_storage.l.buffer.Allocate(1, m_alloc);
        SetSizeWithNullTerminator(0);
    } else {
        m_storage.s.buffer.Data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.size = static_cast<byte>(0);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SwitchToLarge(LargeBuffer&& buffer, size_t size)
{
    memory::ConstructAt(&m_storage.l.buffer, MoveArg(buffer));
    m_storage.l.buffer.Data()[size] = Value(0);
    m_storage.l.layout = 1;
    m_storage.l.size = size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SwitchToSmall(Pointer ptr, size_t size)
{
    memory::MemCopy(m_storage.s.buffer.Data(), ptr, size);
    m_storage.s.buffer.Data()[size] = Value(0);
    m_storage.s.layout = 0;
    m_storage.s.size = static_cast<byte>(size);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::GetSlicedViewData(U const& src, size_t size)
{
    if constexpr (ExplicitlyConvertibleToT<U, T const*>) {
        BasicStringView<T> view(static_cast<T const*>(src), size);
        return view.Data();
    } else {
        BasicStringView<T> view = src;
        BasicStringView<T> subView = view.SubFirst(size);
        return subView.Data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitEmpty()
{
    if consteval {
        memory::ConstructAt(&m_storage.l);
        m_storage.l.buffer.Allocate(1, m_alloc);
        m_storage.l.layout = 1;
        SetSizeWithNullTerminator(0);
    } else {
        m_storage.s.buffer.Data()[0] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithCopy(BasicString const& other)
{
    if consteval {
        memory::ConstructAt(&m_storage.l);
    }

    if (other.LargeCapacity()) {
        size_t size = other.m_storage.l.size;
        memory::ConstructAt(&m_storage.l.buffer, size + 1, m_alloc);
        memory::MemCopy(m_storage.l.buffer.Data(), other.m_storage.l.buffer.Data(), size + 1);
        m_storage.l.layout = 1;
        m_storage.l.size = size;
    } else {
        m_storage = other.m_storage;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithMove(BasicString&& other) noexcept
{
    if consteval {
        memory::ConstructAt(&m_storage.l);
        m_storage.l.buffer.Assign(MoveArg(other.m_storage.l.buffer), m_alloc);
        m_storage.l.layout = 1;
        m_storage.l.size = other.m_storage.l.size;
        other.DestroyBuffer();
        return;
    }

    m_storage = other.m_storage;
    other.InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::InitWithSize(size_t size)
{
    if consteval {
        memory::ConstructAt(&m_storage.l);
        m_storage.l.buffer.Allocate(size + 1, m_alloc);
        m_storage.l.layout = 1;
        SetSizeWithNullTerminator(size);
        return m_storage.l.buffer.Data();
    }

    if (size > SmallCapacity) {
        memory::ConstructAt(&m_storage.l.buffer, size + 1, m_alloc);
        m_storage.l.buffer.Data()[size] = Value(0);
        m_storage.l.layout = 1;
        m_storage.l.size = size;
        return m_storage.l.buffer.Data();
    } else {
        m_storage.s.buffer.Data()[size] = Value(0);
        m_storage.s.layout = 0;
        m_storage.s.size = static_cast<byte>(size);
        return m_storage.s.buffer.Data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithMove(BasicString&& other) noexcept
{
    if consteval {
        m_storage.l.buffer.Assign(MoveArg(other.m_storage.l.buffer), m_alloc);
        m_storage.l.size = other.m_storage.l.size;
        other.DestroyBuffer();
        return;
    }

    mini::Swap(m_storage, other.m_storage);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithSource(ConstPointer ptr, size_t len)
{
    if consteval {
        AssignWithAlloc(ptr, len);
        return;
    }

    if (len > SmallCapacity) {
        AssignWithAlloc(ptr, len);
        return;
    }

    memory::MemMove(Data(), ptr, len);
    SetSizeWithNullTerminator(len);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AssignWithAlloc(ConstPointer ptr, size_t len)
{
    if (!LargeCapacity()) {
        LargeBuffer newBuffer(len + 1, m_alloc);
        memory::MemCopy(newBuffer.Data(), ptr, len);
        SwitchToLarge(MoveArg(newBuffer), len);
    } else {
        Pointer buffer;
        if (m_storage.l.buffer.Capacity() < len) {
            LargeBuffer newBuffer = m_storage.l.buffer.Resize(len + 1, m_alloc);
            buffer = newBuffer.Data();
            memory::MemCopy(buffer, ptr, len);
            m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        } else {
            buffer = m_storage.l.buffer.Data();
            memory::MemCopy(buffer, ptr, len);
        }

        buffer[len] = Value(0);
        m_storage.l.size = len;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::AppendWithRange(Iter begin, Iter end, size_t size)
{
    size_t oldSize = Size();
    size_t newSize = oldSize + size;

    if (newSize <= Capacity()) {
        memory::CopyRange(Data() + oldSize, begin, end);
        SetSizeWithNullTerminator(newSize);
    } else {
        if (!LargeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_storage.s.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_storage.l.buffer.Increment(size, m_alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_storage.l.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AppendWithSource(ConstPointer ptr, size_t len)
{
    if consteval {
        AppendWithAlloc(ptr, len, Size());
        return;
    }

    size_t oldSize = Size();
    size_t newSize = oldSize + len;

    if (newSize > Capacity()) {
        AppendWithAlloc(ptr, len, oldSize);
        return;
    }

    memory::MemCopy(Data() + oldSize, ptr, len);
    SetSizeWithNullTerminator(newSize);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(ConstPointer ptr, size_t len, size_t oldSize)
{
    size_t newSize = oldSize + len;
    Pointer oldBuffer = Data();

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.Increment(len, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        m_storage.l.buffer.Data()[newSize] = Value(0);
        m_storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::AppendWithSize(size_t size)
{
    size_t oldSize = Size();
    size_t newSize = oldSize + size;

    if (newSize <= Capacity()) {
        SetSizeWithNullTerminator(newSize);
        return Data() + oldSize;
    } else {
        AppendWithAlloc(size, oldSize);
        return m_storage.l.buffer.Data() + oldSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(size_t size, size_t oldSize)
{
    size_t newSize = oldSize + size;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        memory::MemCopy(newBuffer.Data(), m_storage.s.buffer.Data(), oldSize);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer buffer = m_storage.l.buffer.Increment(size, m_alloc);
        memory::MemCopy(buffer.Data(), m_storage.l.buffer.Data(), oldSize);
        m_storage.l.buffer.Assign(MoveArg(buffer), m_alloc);
        m_storage.l.buffer.Data()[newSize] = Value(0);
        m_storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InsertWithSource(size_t index, ConstPointer ptr, size_t len)
{
    size_t oldSize = Size();
    size_t newSize = oldSize + len;

    if (newSize <= Capacity()) {
        Pointer oldBuffer = Data();
        Pointer loc = oldBuffer + index;

        if (memory::IsPtrOverlapping(ptr, oldBuffer, oldBuffer + oldSize)) {
            BasicString temp(ptr, len, m_alloc);
            ptr = temp.Data();
        }

        memory::MemMove(loc + len, loc, oldSize - index);
        memory::MemCopy(loc, ptr, len);
        SetSizeWithNullTerminator(newSize);
    } else {
        InsertWithAlloc(index, ptr, len, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::
    InsertWithAlloc(size_t index, ConstPointer ptr, size_t len, size_t oldSize)
{
    size_t newSize = oldSize + len;
    Pointer oldBuffer = Data();
    Pointer loc = oldBuffer + index;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.Increment(len, m_alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        m_storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::InsertWithSize(size_t index, size_t size)
{
    size_t oldSize = Size();
    size_t newSize = oldSize + size;

    if (newSize <= Capacity()) {
        Pointer loc = Data() + index;
        memory::MemMove(loc + size, loc, oldSize - index);
        SetSizeWithNullTerminator(newSize);
        return loc;
    } else {
        InsertWithAlloc(index, size, oldSize);
        return m_storage.l.buffer.Data() + index;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::InsertWithAlloc(size_t index, size_t size, size_t oldSize)
{
    size_t newSize = oldSize + size;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_alloc);
        Pointer buffer = newBuffer.Data();
        Pointer oldBuffer = m_storage.s.buffer.Data();

        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + size, oldBuffer + index, oldSize - index);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_storage.l.buffer.Increment(size, m_alloc);
        Pointer buffer = newBuffer.Data();
        Pointer oldBuffer = m_storage.l.buffer.Data();

        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + size, oldBuffer + index, oldSize - index);
        m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
        m_storage.l.buffer.Data()[newSize] = Value(0);
        m_storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertWithRange(size_t index, Iter begin, Iter end, size_t size)
{
    size_t oldSize = Size();
    size_t newSize = oldSize + size;
    Pointer oldBuffer = Data();
    Pointer loc = oldBuffer + index;

    if (newSize <= Capacity()) {
        BasicString temp(size, m_alloc);
        Pointer buffer = temp.Data();
        memory::CopyRange(buffer, begin, end);
        memory::MemMove(loc + size, loc, oldSize - index);
        memory::MemCopy(loc, temp.Data(), size);
        SetSizeWithNullTerminator(newSize);
    } else {
        if (!LargeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, oldBuffer, index);
            memory::MemCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_storage.l.buffer.Increment(size, m_alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, oldBuffer, index);
            memory::MemCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            m_storage.l.buffer.Assign(MoveArg(newBuffer), m_alloc);
            m_storage.l.buffer.Data()[newSize] = Value(0);
            m_storage.l.size = newSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <typename TraitsT, typename StdAllocT>
inline constexpr BasicString<T, AllocT>::BasicString(std::basic_string<T, TraitsT, StdAllocT> const& other,
                                                     AllocT const& alloc)
    : m_alloc(alloc)
{
    size_t size = static_cast<size_t>(other.size());
    Pointer buffer = InitWithSize(size);
    memory::MemCopy(buffer, other.data(), size + 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator std::basic_string<T>() const
{
    return std::basic_string<T>(Data(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator std::basic_string_view<T>() const noexcept
{
    return std::basic_string_view<T>(Data(), Size());
}

export template <CharT T, typename TraitsT, typename StdAllocT, AllocatorT<T> AllocT = mini::Allocator<T>>
inline constexpr BasicString<T, AllocT> ToString(std::basic_string<T, TraitsT, StdAllocT> const& other,
                                                 AllocT const& alloc = AllocT())
{
    return BasicString<T, AllocT>(other.data(), other.size(), alloc);
}

export template <CharT T,
                 AllocatorT<T> AllocT,
                 typename TraitsT = std::char_traits<T>,
                 typename StdAllocT = std::allocator<T>>
inline constexpr std::basic_string<T, TraitsT, StdAllocT> ToStdString(BasicString<T, AllocT> const& other,
                                                                      StdAllocT const& alloc = StdAllocT())
{
    return std::basic_string<T, TraitsT, StdAllocT>(other.Data(), other.Size(), alloc);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator BasicStringView<T>() const noexcept
{
    return BasicStringView(Data(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. current size is {}", index, Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at index {}. current size is {}", iter.Address() - Data(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                               [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}. current size is {}",
           begin.Address() - Data(),
           end.Address() - Data(),
           Size());
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& l, BasicString<T, AllocT> const& r) noexcept
{
    size_t size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    using Pointer = BasicString<T, AllocT>::ConstPointer;
    Pointer lbuf = l.Data();
    Pointer rbuf = r.Data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, AllocatorT<T> AllocT, CharT U, AllocatorT<U> AllocU>
inline constexpr bool operator==(BasicString<T, AllocT> const& l, BasicString<U, AllocU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPointer lbuf = l.Data();
    typename BasicString<U, AllocU>::ConstPointer rbuf = r.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, AllocatorT<T> AllocT, typename StdAllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& l, std::basic_string<T, StdAllocT> const& r) noexcept
{
    size_t size = l.Size();
    if (size != r.size()) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPointer lbuf = l.Data();
    typename std::basic_string<T, StdAllocT>::const_pointer rbuf = r.data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, AllocatorT<T> AllocT, CharT U, typename StdAllocU>
inline constexpr bool operator==(BasicString<T, AllocT> const& l, std::basic_string<U, StdAllocU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = l.Size();
    if (size != r.size()) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPointer lbuf = l.Data();
    typename std::basic_string<T, StdAllocU>::const_pointer rbuf = r.data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, AllocatorT<T> AllocT, StringViewLikeT<T, AllocT> ViewU>
inline constexpr bool operator==(BasicString<T, AllocT> const& s, ViewU const& v) noexcept
{
    BasicStringView<T> l = s;
    BasicStringView<T> r = v;
    return l == r;
}

export template <CharT T, AllocatorT<T> AllocT, CharT U, AllocatorT<U> AllocU, StringViewLikeT<U, AllocU> ViewU>
inline constexpr bool operator==(BasicString<T, AllocT> const& s, ViewU const& v) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<T> l = s;
    BasicStringView<U> r = v;
    return l == r;
}

export template <CharT T, AllocatorT<T> AllocT>
bool operator==(BasicString<T, AllocT> const&, nullptr_t) = delete;

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT> const& l,
                                                  typename BasicString<T, AllocT>::Value r)
{
    BasicString<T, AllocT> s = l;
    s.Push(r);
    return s;
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT>&& l, typename BasicString<T, AllocT>::Value r)
{
    l.Push(r);
    return MoveArg(l);
}

export template <CharT T, AllocatorT<T> AllocT, StringLikeT<T> ViewT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT> const& l, ViewT const& r)
{
    BasicString<T, AllocT> s = l;
    s.Append(r);
    return s;
}

export template <CharT T, AllocatorT<T> AllocT, StringLikeT<T> ViewT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT>&& l, ViewT const& r)
{
    l.Append(r);
    return MoveArg(l);
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr void Swap(BasicString<T, AllocT> const& l, BasicString<T, AllocT> const& r) noexcept
{
    l.Swap(r);
}

} // namespace mini