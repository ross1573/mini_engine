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
import :numeric;
import :utility_operation;
import :memory_operation;
import :allocator;
import :trivial_buffer;
import :static_buffer;
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
            SizeT layout : 1;
            SizeT size   : (sizeof(SizeT) * 8 - 1);
        };
        LargeBuffer buffer;
        PACKED_STRUCT_END
    };

    static constexpr SizeT AllocatedSize = (sizeof(LargeStorage) / sizeof(T)) - 2;
    static constexpr SizeT SmallCapacity = AllocatedSize > 2 ? AllocatedSize : 2;
    typedef memory::StaticBuffer<T, SmallCapacity + 1> SmallBuffer;

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

    [[emptyable_address]] AllocT m_Alloc;
    Storage m_Storage;

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
    constexpr BasicString(SizeT, AllocT const& = AllocT());
    template <typename U>
    constexpr BasicString(U, SizeT, AllocT const& = AllocT())
        requires AnyOfT<U, Value, ConstValue>;
    template <StringViewLikeT<T, AllocT> U>
    constexpr BasicString(U const&, AllocT const& = AllocT());
    template <StringViewLikeT<T, AllocT> U>
    constexpr BasicString(U const&, SizeT, AllocT const& = AllocT());
    template <ForwardIteratableByT<T> Iter>
    constexpr BasicString(Iter, Iter, AllocT const& = AllocT());

    constexpr void Assign(BasicString&&);
    template <StringLikeT<T> U>
    constexpr void Assign(U const&);
    template <StringLikeT<T> U>
    constexpr void Assign(U const&, SizeT);

    constexpr void Push(Value);
    constexpr void Push(Value, SizeT);
    template <StringLikeT<T> U>
    constexpr void Append(U const&);
    template <StringLikeT<T> U>
    constexpr void Append(U const&, SizeT);

    constexpr void Insert(SizeT, Value);
    constexpr void Insert(SizeT, Value, SizeT);
    template <StringLikeT<T> U>
    constexpr void Insert(SizeT, U const&);
    template <StringLikeT<T> U>
    constexpr void Insert(SizeT, U const&, SizeT);
    constexpr void Insert(ConstIterator, Value);
    constexpr void Insert(ConstIterator, Value, SizeT);
    template <StringLikeT<T> U>
    constexpr void Insert(ConstIterator, U const&);
    template <StringLikeT<T> U>
    constexpr void Insert(ConstIterator, U const&, SizeT);

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(SizeT, Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator, Iter, Iter);

    constexpr void RemoveLast();
    constexpr void RemoveLast(SizeT);
    constexpr void RemoveAt(SizeT);
    constexpr void RemoveAt(ConstIterator);
    constexpr void RemoveRange(SizeT, SizeT);
    constexpr void RemoveRange(ConstIterator, SizeT);
    constexpr void RemoveRange(ConstIterator, ConstIterator);

    constexpr void Resize(SizeT, Value = Value(0));
    constexpr void Reserve(SizeT);
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
    constexpr Reference At(SizeT);
    constexpr ConstReference At(SizeT) const;

    constexpr SizeT Size() const noexcept;
    constexpr SizeT Capacity() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool ValidIndex(SizeT) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Reference operator[](SizeT);
    constexpr ConstReference operator[](SizeT) const;

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
    constexpr BasicString(std::basic_string<T, TraitsT, StdAllocT> const&,
                          AllocT const& = AllocT());

private:
    BasicString(NullptrT) = delete;
    BasicString(NullptrT, SizeT) = delete;
    void Assign(NullptrT) = delete;
    void Assign(NullptrT, SizeT) = delete;
    void Append(NullptrT) = delete;
    void Append(NullptrT, SizeT) = delete;
    void Insert(SizeT, NullptrT) = delete;
    void Insert(SizeT, NullptrT, SizeT) = delete;
    BasicString& operator=(NullptrT) = delete;

    constexpr bool LargeCapacity() const noexcept;
    constexpr void SetSizeWithNullTerminator(SizeT) noexcept;
    constexpr void DestroyBuffer();
    constexpr void ResizeWithAlloc(SizeT, Value, SizeT);
    constexpr void SwitchToLarge(LargeBuffer&&, SizeT);
    constexpr LargeBuffer SwitchToSmall(Pointer, SizeT);
    template <StringLikeT<T> U>
    constexpr ConstPointer GetViewData(U const&, SizeT);

    constexpr void InitEmpty();
    constexpr void InitWithCopy(BasicString const&);
    constexpr void InitWithMove(BasicString&&) noexcept;
    constexpr Pointer InitWithSize(SizeT);

    constexpr void AssignWithAlloc(ConstPointer, SizeT);
    constexpr void AssignWithSource(ConstPointer, SizeT);
    constexpr void AssignWithMove(BasicString&&) noexcept;

    template <ForwardIteratableByT<T> Iter>
    constexpr void AppendWithRange(Iter, Iter, SizeT);
    constexpr void AppendWithSource(ConstPointer, SizeT);
    constexpr void AppendWithAlloc(ConstPointer, SizeT, SizeT);
    constexpr void AppendWithAlloc(SizeT, SizeT);
    constexpr Pointer AppendWithSize(SizeT);

    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertWithRange(SizeT, Iter, Iter, SizeT);
    constexpr void InsertWithSource(SizeT, ConstPointer, SizeT);
    constexpr void InsertWithAlloc(SizeT, ConstPointer, SizeT, SizeT);
    constexpr void InsertWithAlloc(SizeT, SizeT, SizeT);
    constexpr Pointer InsertWithSize(SizeT, SizeT);

    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString() noexcept
    : m_Alloc{}
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::~BasicString()
{
    if (LargeCapacity()) {
        m_Storage.l.buffer.Deallocate(m_Alloc);
        m_Storage.l.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other)
    : m_Alloc{}
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other)
    : m_Alloc{}
{
    if (other.LargeCapacity() && m_Alloc != other.m_Alloc) {
        InitWithCopy(other);
    } else {
        InitWithMove(MoveArg(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    if (other.LargeCapacity() && m_Alloc != other.m_Alloc) {
        InitWithCopy(other);
    } else {
        InitWithMove(MoveArg(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT const& alloc) noexcept
    : m_Alloc(alloc)
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT&& alloc) noexcept
    : m_Alloc(MoveArg(alloc))
{
    InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(SizeT capacity, AllocT const& alloc)
    : m_Alloc(alloc)
{
    if consteval {
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Allocate(capacity + 1, m_Alloc);
        m_Storage.l.layout = 1;
        SetSizeWithNullTerminator(0);
        return;
    }

    if (capacity > SmallCapacity) {
        memory::ConstructAt(&m_Storage.l.buffer, capacity + 1, m_Alloc);
        m_Storage.l.buffer.Data()[0] = Value(0);
        m_Storage.l.layout = 1;
        m_Storage.l.size = 0;
    } else {
        m_Storage.s.buffer.Data()[0] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr BasicString<T, AllocT>::BasicString(U ch, SizeT size, AllocT const& alloc)
    requires AnyOfT<U, Value, ConstValue>
    : m_Alloc(alloc)
{
    Pointer loc = InitWithSize(size);
    memory::StringFill(loc, ch, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <StringViewLikeT<T, AllocT> U>
inline constexpr BasicString<T, AllocT>::BasicString(U const& src, AllocT const& alloc)
    : m_Alloc(alloc)
{
    BasicStringView<T> view = src;
    Pointer loc = InitWithSize(view.Size());
    memory::MemCopy(loc, view.Data(), view.Size());
}

template <CharT T, AllocatorT<T> AllocT>
template <StringViewLikeT<T, AllocT> U>
inline constexpr BasicString<T, AllocT>::BasicString(U const& src, SizeT size, AllocT const& alloc)
    : m_Alloc(alloc)
{
    ConstPointer data = GetViewData(src, size);
    Pointer loc = InitWithSize(size);
    memory::MemCopy(loc, data, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr BasicString<T, AllocT>::BasicString(Iter begin, Iter end, AllocT const& alloc)
    : m_Alloc(alloc)
{
    SizeT size = Distance(begin, end);
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
inline constexpr void BasicString<T, AllocT>::Assign(U const& src, SizeT size)
{
    ConstPointer data = GetViewData(src, size);
    AssignWithSource(data, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString&& other)
{
    if (m_Alloc == other.m_Alloc) [[likely]] {
        AssignWithMove(MoveArg(other));
    } else {
        AssignWithSource(other.Data(), other.Size());
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Assign(Iter begin, Iter end)
{
    BasicString temp(begin, end, m_Alloc);
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
inline constexpr void BasicString<T, AllocT>::Push(Value ch, SizeT count)
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
inline constexpr void BasicString<T, AllocT>::Append(U const& src, SizeT size)
{
    ConstPointer data = GetViewData(src, size);
    AppendWithSource(data, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Append(Iter begin, Iter end)
{
    SizeT size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    AppendWithRange(begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, Value ch)
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
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, Value ch, SizeT count)
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
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, U const& src)
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
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, U const& src, SizeT len)
{
    if (index == Size()) {
        Append(src, len);
        return;
    }

    ConstPointer data = GetViewData(src, len);

    AssertValidIndex(index);
    InsertWithSource(index, data, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch)
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    if (index == Size()) {
        Push(ch);
        return;
    }

    AssertValidIterator(iter);
    Pointer loc = InsertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch, SizeT count)
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
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
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
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
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, U const& src, SizeT len)
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    if (index == Size()) {
        Append(src, len);
        return;
    }

    ConstPointer data = GetViewData(src, len);

    AssertValidIterator(iter);
    InsertWithSource(index, data, len);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertRange(SizeT index, Iter begin, Iter end)
{
    if (index == Size()) {
        Append(begin, end);
        return;
    }

    SizeT size = Distance(begin, end);
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
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    if (index == Size()) {
        Append(begin, end);
        return;
    }

    SizeT size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithRange(index, begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast()
{
    SizeT oldSize = Size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast(SizeT count)
{
    SizeT oldSize = Size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = oldSize < count ? oldSize : count;
    SetSizeWithNullTerminator(oldSize - removeCnt);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(SizeT index)
{
    SizeT oldSize = Size();
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
    SizeT oldSize = Size();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Pointer loc = buffer + index;
    memory::MemMove(loc, loc + 1, oldSize - index - 1);
    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(SizeT index, SizeT count)
{
    SizeT oldSize = Size();
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
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator iter, SizeT count)
{
    Pointer buffer = Data();
    SizeT oldSize = Size();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
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
    SizeT oldSize = Size();

    SizeT count = static_cast<SizeT>(end - begin);
    if (count == 0) [[unlikely]] {
        return;
    }

    SizeT index = static_cast<SizeT>(begin.Address() - buffer);
    SizeT endIndex = static_cast<SizeT>(end.Address() - buffer);
    if (endIndex == oldSize) [[unlikely]] {
        SetSizeWithNullTerminator(index);
        return;
    }

    Pointer loc = buffer + index;
    memory::MemMove(loc, end.Address(), oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Resize(SizeT size, Value ch)
{
    SizeT oldSize = Size();
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
[[no_inline]] constexpr void BasicString<T, AllocT>::ResizeWithAlloc(SizeT size, Value ch,
                                                                     SizeT oldSize)
{
    if (!LargeCapacity()) {
        LargeBuffer newBuffer(size + 1, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, m_Storage.s.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        SwitchToLarge(MoveArg(newBuffer), size);
    } else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Resize(size + 1, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, m_Storage.l.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.buffer.Data()[size] = Value(0);
        m_Storage.l.size = size;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Reserve(SizeT size)
{
    if (size < Capacity()) [[unlikely]] {
        return;
    }

    if (!LargeCapacity()) {
        LargeBuffer buffer(size + 1, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.s.buffer.Data(), m_Storage.s.size);
        SwitchToLarge(MoveArg(buffer), m_Storage.s.size);
    } else {
        LargeBuffer buffer = m_Storage.l.buffer.Resize(size + 1, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.l.buffer.Data(), m_Storage.l.size + 1);
        m_Storage.l.buffer.Assign(MoveArg(buffer), m_Alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Shrink()
{
    if consteval {
        LargeBuffer newBuffer = m_Storage.l.buffer.Resize(Size() + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), Data(), Size() + 1);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        return;
    }

    if (!LargeCapacity()) [[unlikely]] {
        return;
    }

    SizeT size = m_Storage.l.size;
    if (size <= SmallCapacity) {
        Pointer buffer = m_Storage.l.buffer.Data();
        memory::MemCopy(m_Storage.s.buffer.Data(), buffer, size + 1);
        m_Storage.s.layout = 0;
        m_Storage.s.size = static_cast<byte>(size);
        m_Alloc.Deallocate(buffer, size);
    } else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Resize(size + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), m_Storage.l.buffer.Data(), size + 1);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
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
        SizeT size = m_Storage.l.size;
        m_Storage.l.buffer.Swap(other.m_Storage.l.buffer);
        m_Storage.l.size = other.m_Storage.l.size;
        other.m_Storage.l.size = size;
        return;
    }

    mini::Swap(m_Storage, other.m_Storage);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::Begin() noexcept
{
    return Iterator(Data(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator
BasicString<T, AllocT>::Begin() const noexcept
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
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference BasicString<T, AllocT>::Last() const
{
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference
BasicString<T, AllocT>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::Data() noexcept
{
    return LargeCapacity() ? m_Storage.l.buffer.Data() : m_Storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::Data() const noexcept
{
    return LargeCapacity() ? m_Storage.l.buffer.Data() : m_Storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Size() const noexcept
{
    return LargeCapacity() ? m_Storage.l.size : m_Storage.s.size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Capacity() const noexcept
{
    return LargeCapacity() ? (m_Storage.l.buffer.Capacity() - 1) : SmallCapacity;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::Empty() const noexcept
{
    return LargeCapacity() ? m_Storage.l.size == 0 : m_Storage.s.size == 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidIndex(SizeT index) const noexcept
{
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::ValidRange(ConstIterator begin,
                                                         ConstIterator end) const noexcept
{
    ConstPointer buffer = Data();
    SizeT size = Size();
    SizeT beginIdx = static_cast<SizeT>(begin.Address() - buffer);
    SizeT endIdx = static_cast<SizeT>(end.Address() - buffer);
    return (beginIdx < size) && (endIdx <= size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Reference BasicString<T, AllocT>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstReference
BasicString<T, AllocT>::operator[](SizeT index) const
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
        return m_Storage.l.layout == 1;
    }

    return m_Storage.s.layout == 1;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SetSizeWithNullTerminator(SizeT size) noexcept
{
    Pointer buffer = nullptr;
    if (LargeCapacity()) {
        m_Storage.l.size = size;
        buffer = m_Storage.l.buffer.Data();
    } else {
        m_Storage.s.size = static_cast<byte>(size);
        buffer = m_Storage.s.buffer.Data();
    }

    buffer[size] = Value(0);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::DestroyBuffer()
{
    if (!LargeCapacity()) {
        return;
    }

    m_Storage.l.buffer.Deallocate(m_Alloc);

    if consteval {
        m_Storage.l.buffer.Allocate(1, m_Alloc);
        SetSizeWithNullTerminator(0);
    } else {
        m_Storage.s.buffer.Data()[0] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = static_cast<byte>(0);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SwitchToLarge(LargeBuffer&& buffer, SizeT size)
{
    memory::ConstructAt(&m_Storage.l.buffer, MoveArg(buffer));
    m_Storage.l.buffer.Data()[size] = Value(0);
    m_Storage.l.layout = 1;
    m_Storage.l.size = size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::LargeBuffer
BasicString<T, AllocT>::SwitchToSmall(Pointer ptr, SizeT size)
{
    LargeBuffer tmpBuffer(MoveArg(m_Storage.l.buffer));
    memory::MemCopy(m_Storage.s.buffer.Data(), ptr, size);
    m_Storage.s.buffer.Data()[size] = Value(0);
    m_Storage.s.layout = 0;
    m_Storage.s.size = static_cast<byte>(size);
    return tmpBuffer;
}

template <CharT T, AllocatorT<T> AllocT>
template <StringLikeT<T> U>
constexpr BasicString<T, AllocT>::ConstPointer BasicString<T, AllocT>::GetViewData(U const& src,
                                                                                   SizeT size)
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
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Allocate(1, m_Alloc);
        m_Storage.l.layout = 1;
        SetSizeWithNullTerminator(0);
    } else {
        m_Storage.s.buffer.Data()[0] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithCopy(BasicString const& other)
{
    if consteval {
        memory::ConstructAt(&m_Storage.l);
    }

    if (other.LargeCapacity()) {
        SizeT size = other.m_Storage.l.size;
        memory::ConstructAt(&m_Storage.l.buffer, size + 1, m_Alloc);
        memory::MemCopy(m_Storage.l.buffer.Data(), other.m_Storage.l.buffer.Data(), size + 1);
        m_Storage.l.layout = 1;
        m_Storage.l.size = size;
    } else {
        m_Storage = other.m_Storage;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithMove(BasicString&& other) noexcept
{
    if consteval {
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Assign(MoveArg(other.m_Storage.l.buffer), m_Alloc);
        m_Storage.l.layout = 1;
        m_Storage.l.size = other.m_Storage.l.size;
        other.DestroyBuffer();
        return;
    }

    m_Storage = other.m_Storage;
    other.InitEmpty();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::InitWithSize(SizeT size)
{
    if consteval {
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Allocate(size + 1, m_Alloc);
        m_Storage.l.layout = 1;
        SetSizeWithNullTerminator(size);
        return m_Storage.l.buffer.Data();
    }

    if (size > SmallCapacity) {
        memory::ConstructAt(&m_Storage.l.buffer, size + 1, m_Alloc);
        m_Storage.l.buffer.Data()[size] = Value(0);
        m_Storage.l.layout = 1;
        m_Storage.l.size = size;
        return m_Storage.l.buffer.Data();
    } else {
        m_Storage.s.buffer.Data()[size] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = static_cast<byte>(size);
        return m_Storage.s.buffer.Data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithMove(BasicString&& other) noexcept
{
    if consteval {
        m_Storage.l.buffer.Assign(MoveArg(other.m_Storage.l.buffer), m_Alloc);
        m_Storage.l.size = other.m_Storage.l.size;
        other.DestroyBuffer();
        return;
    }

    mini::Swap(m_Storage, other.m_Storage);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithSource(ConstPointer ptr, SizeT len)
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
[[no_inline]] constexpr void BasicString<T, AllocT>::AssignWithAlloc(ConstPointer ptr, SizeT len)
{
    if (!LargeCapacity()) {
        LargeBuffer newBuffer(len + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), ptr, len);
        SwitchToLarge(MoveArg(newBuffer), len);
    } else {
        Pointer buffer;
        if (m_Storage.l.buffer.Capacity() < len) {
            LargeBuffer newBuffer = m_Storage.l.buffer.Resize(len + 1, m_Alloc);
            buffer = newBuffer.Data();
            memory::MemCopy(buffer, ptr, len);
            m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        } else {
            buffer = m_Storage.l.buffer.Data();
            memory::MemCopy(buffer, ptr, len);
        }

        buffer[len] = Value(0);
        m_Storage.l.size = len;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::AppendWithRange(Iter begin, Iter end, SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize <= Capacity()) {
        memory::CopyRange(Data() + oldSize, begin, end);
        SetSizeWithNullTerminator(newSize);
    } else {
        if (!LargeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_Storage.s.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_Storage.l.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AppendWithSource(ConstPointer ptr, SizeT len)
{
    if consteval {
        AppendWithAlloc(ptr, len, Size());
        return;
    }

    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;

    if (newSize > Capacity()) {
        AppendWithAlloc(ptr, len, oldSize);
        return;
    }

    memory::MemCopy(Data() + oldSize, ptr, len);
    SetSizeWithNullTerminator(newSize);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(ConstPointer ptr, SizeT len,
                                                                     SizeT oldSize)
{
    SizeT newSize = oldSize + len;
    Pointer oldBuffer = Data();

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(len, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.buffer.Data()[newSize] = Value(0);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::AppendWithSize(SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize <= Capacity()) {
        SetSizeWithNullTerminator(newSize);
        return Data() + oldSize;
    } else {
        AppendWithAlloc(size, oldSize);
        return m_Storage.l.buffer.Data() + oldSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(SizeT size, SizeT oldSize)
{
    SizeT newSize = oldSize + size;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), m_Storage.s.buffer.Data(), oldSize);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer buffer = m_Storage.l.buffer.Increment(size, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.l.buffer.Data(), oldSize);
        m_Storage.l.buffer.Assign(MoveArg(buffer), m_Alloc);
        m_Storage.l.buffer.Data()[newSize] = Value(0);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InsertWithSource(SizeT index, ConstPointer ptr,
                                                               SizeT len)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;

    if (newSize <= Capacity()) {
        Pointer oldBuffer = Data();
        Pointer loc = oldBuffer + index;

        if (memory::IsPtrOverlapping(ptr, oldBuffer, oldBuffer + oldSize)) {
            BasicString temp(ptr, len, m_Alloc);
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
[[no_inline]] constexpr void BasicString<T, AllocT>::InsertWithAlloc(SizeT index, ConstPointer ptr,
                                                                     SizeT len, SizeT oldSize)
{
    SizeT newSize = oldSize + len;
    Pointer oldBuffer = Data();
    Pointer loc = oldBuffer + index;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(len, m_Alloc);
        Pointer buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Pointer BasicString<T, AllocT>::InsertWithSize(SizeT index,
                                                                                        SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize <= Capacity()) {
        Pointer loc = Data() + index;
        memory::MemMove(loc + size, loc, oldSize - index);
        SetSizeWithNullTerminator(newSize);
        return loc;
    } else {
        InsertWithAlloc(index, size, oldSize);
        return m_Storage.l.buffer.Data() + index;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::InsertWithAlloc(SizeT index, SizeT size,
                                                                     SizeT oldSize)
{
    SizeT newSize = oldSize + size;

    if (!LargeCapacity()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Pointer buffer = newBuffer.Data();
        Pointer oldBuffer = m_Storage.s.buffer.Data();

        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + size, oldBuffer + index, oldSize - index);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    } else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
        Pointer buffer = newBuffer.Data();
        Pointer oldBuffer = m_Storage.l.buffer.Data();

        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + size, oldBuffer + index, oldSize - index);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.buffer.Data()[newSize] = Value(0);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertWithRange(SizeT index, Iter begin, Iter end,
                                                              SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;
    Pointer oldBuffer = Data();
    Pointer loc = oldBuffer + index;

    if (newSize <= Capacity()) {
        BasicString temp(size, m_Alloc);
        Pointer buffer = temp.Data();
        memory::CopyRange(buffer, begin, end);
        memory::MemMove(loc + size, loc, oldSize - index);
        memory::MemCopy(loc, temp.Data(), size);
        SetSizeWithNullTerminator(newSize);
    } else {
        if (!LargeCapacity()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, oldBuffer, index);
            memory::MemCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        } else {
            LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
            Pointer buffer = newBuffer.Data();
            memory::MemCopy(buffer, oldBuffer, index);
            memory::MemCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
            m_Storage.l.buffer.Data()[newSize] = Value(0);
            m_Storage.l.size = newSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <typename TraitsT, typename StdAllocT>
inline constexpr BasicString<T, AllocT>::
    BasicString(std::basic_string<T, TraitsT, StdAllocT> const& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    SizeT size = static_cast<SizeT>(other.size());
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

export template <CharT T, typename TraitsT, typename StdAllocT,
                 AllocatorT<T> AllocT = mini::Allocator<T>>
inline constexpr BasicString<T, AllocT>
ToString(std::basic_string<T, TraitsT, StdAllocT> const& other, AllocT const& alloc = AllocT())
{
    return BasicString<T, AllocT>(other.data(), other.size(), alloc);
}

export template <CharT T, AllocatorT<T> AllocT, typename TraitsT = std::char_traits<T>,
                 typename StdAllocT = std::allocator<T>>
inline constexpr std::basic_string<T, TraitsT, StdAllocT>
ToStdString(BasicString<T, AllocT> const& other, StdAllocT const& alloc = StdAllocT())
{
    return std::basic_string<T, TraitsT, StdAllocT>(other.Data(), other.Size(), alloc);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator BasicStringView<T>() const noexcept
{
    return BasicStringView(Data(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index");
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator");
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                         [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end), "invalid range");
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& l,
                                 BasicString<T, AllocT> const& r) noexcept
{
    SizeT size = l.Size();
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
inline constexpr bool operator==(BasicString<T, AllocT> const& l,
                                 BasicString<U, AllocU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    SizeT size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPointer lbuf = l.Data();
    typename BasicString<U, AllocU>::ConstPointer rbuf = r.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, AllocatorT<T> AllocT, typename StdAllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& l,
                                 std::basic_string<T, StdAllocT> const& r) noexcept
{
    SizeT size = l.Size();
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
inline constexpr bool operator==(BasicString<T, AllocT> const& l,
                                 std::basic_string<U, StdAllocU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    SizeT size = l.Size();
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

export template <CharT T, AllocatorT<T> AllocT, CharT U, AllocatorT<U> AllocU,
                 StringViewLikeT<U, AllocU> ViewU>
inline constexpr bool operator==(BasicString<T, AllocT> const& s, ViewU const& v) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<T> l = s;
    BasicStringView<U> r = v;
    return l == r;
}

export template <CharT T, AllocatorT<T> AllocT>
bool operator==(BasicString<T, AllocT> const&, NullptrT) = delete;

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT> const& l,
                                                  typename BasicString<T, AllocT>::Value r)
{
    BasicString<T, AllocT> s = l;
    s.Push(r);
    return s;
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> operator+(BasicString<T, AllocT>&& l,
                                                  typename BasicString<T, AllocT>::Value r)
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
inline constexpr void Swap(BasicString<T, AllocT> const& l,
                           BasicString<T, AllocT> const& r) noexcept
{
    l.Swap(r);
}

} // namespace mini