module;

#include <string>

#include "assertion.h"

#if CLANG || GCC
#  define PACKED_STRUCT_BEGIN __attribute__((__packed__))
#  define PACKED_STRUCT_END
#else
#  define PACKED_STRUCT_BEGIN
#  define PACKED_STRUCT_END
#endif

export module mini.core:string;

import :type;
import :utility;
import :memory;
import :algorithm;
import :allocator;
import :buffer;
import :static_buffer;
import :string_utility;
import :array_iterator;

namespace mini {

export template <CharT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class BasicString;

export using String = BasicString<char>;
export using WString = BasicString<wchar>;
export using U8String = BasicString<char8>;
export using U16String = BasicString<char16>;
export using U32String = BasicString<char32>;

export template <CharT T, AllocatorT<T> AllocT>
class BasicString {
private:
    template <CharT U, AllocatorT<U> AllocU>
    friend class BasicString;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;
    using Iterator = ArrayIterator<Value, BasicString>;
    using ConstIterator = ArrayIterator<ConstValue, BasicString const>;

private:
    typedef TrivialBuffer<T> LargeBuffer;

    struct LargeStorage {
        PACKED_STRUCT_BEGIN
        struct {
            byte padding : 7;
            byte layout  : 1;
        };
        PACKED_STRUCT_END
        byte _[3];
        SizeT size;
        LargeBuffer buffer;

        // TODO: when will msvc resolve this shitty bug..?
        constexpr LargeStorage() = default;
        constexpr LargeStorage(LargeStorage&&) = default;
        constexpr ~LargeStorage() = default;
    };

    static constexpr SizeT AllocatedSize = (sizeof(LargeStorage) / sizeof(T)) - 2;
    static constexpr SizeT SmallCapacity = AllocatedSize > 2 ? AllocatedSize : 2;
    static_assert(SmallCapacity < 128, "small capacity should not excced 127");

    typedef StaticBuffer<T, SmallCapacity> SmallBuffer;

    struct SmallStorage {
        PACKED_STRUCT_BEGIN
        struct {
            byte size   : 7;
            byte layout : 1;
        };
        PACKED_STRUCT_END
        SmallBuffer buffer;

        // TODO: when will msvc resolve this shitty bug..?
        constexpr SmallStorage() = default;
        constexpr ~SmallStorage() = default;
    };

    [[no_unique_address]] AllocT m_Alloc;
    union {
        SmallStorage m_Small;
        LargeStorage m_Large;
    };

public:
    constexpr BasicString() noexcept;
    constexpr ~BasicString();
    constexpr BasicString(BasicString const&);
    constexpr BasicString(BasicString const&, AllocT const&);
    constexpr BasicString(BasicString&&) noexcept;
    constexpr BasicString(BasicString&&, AllocT const&) noexcept;
    constexpr BasicString(AllocT const&) noexcept;
    constexpr BasicString(AllocT&&) noexcept;
    constexpr BasicString(SizeT, AllocT const& = AllocT());
    constexpr BasicString(Value, SizeT, AllocT const& = AllocT());
    constexpr BasicString(ConstPtr, AllocT const& = AllocT());
    constexpr BasicString(ConstPtr, SizeT, AllocT const& = AllocT());
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr BasicString(Iter, Iter, AllocT const& = AllocT());

    constexpr void Assign(ConstPtr);
    constexpr void Assign(ConstPtr, SizeT);
    constexpr void Assign(BasicString const&);
    constexpr void Assign(BasicString&&);
    constexpr void Push(Value);
    constexpr void Push(Value, SizeT);
    constexpr void Append(ConstPtr);
    constexpr void Append(ConstPtr, SizeT);
    constexpr void Append(BasicString const&);
    constexpr void Insert(SizeT, Value);
    constexpr void Insert(SizeT, Value, SizeT);
    constexpr void Insert(SizeT, ConstPtr);
    constexpr void Insert(SizeT, ConstPtr, SizeT);
    constexpr void Insert(SizeT, BasicString const&);
    constexpr void Insert(ConstIterator, Value);
    constexpr void Insert(ConstIterator, Value, SizeT);
    constexpr void Insert(ConstIterator, ConstPtr);
    constexpr void Insert(ConstIterator, ConstPtr, SizeT);
    constexpr void Insert(ConstIterator, BasicString const&);

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
    constexpr void Swap(BasicString&) noexcept;

    constexpr Ptr Data() noexcept;
    constexpr ConstPtr Data() const noexcept;
    constexpr Iterator Begin() noexcept;
    constexpr ConstIterator Begin() const noexcept;
    constexpr Iterator End() noexcept;
    constexpr ConstIterator End() const noexcept;
    constexpr Ref First();
    constexpr ConstRef First() const;
    constexpr Ref Last();
    constexpr ConstRef Last() const;
    constexpr Ref At(SizeT);
    constexpr ConstRef At(SizeT) const;

    constexpr SizeT Size() const noexcept;
    constexpr SizeT Capacity() const noexcept;
    constexpr bool IsEmpty() const noexcept;
    constexpr bool IsValidIndex(SizeT) const noexcept;
    constexpr bool IsValidIterator(ConstIterator) const noexcept;

    constexpr Ref operator[](SizeT);
    constexpr ConstRef operator[](SizeT) const;

    constexpr BasicString& operator=(BasicString const&);
    constexpr BasicString& operator=(BasicString&&) noexcept;

    template <ConvertibleToT<T> U, typename TraitsU, typename AllocU>
    constexpr BasicString(std::basic_string<U, TraitsU, AllocU> const&, AllocT const& = AllocT());

    template <typename TraitsT = std::char_traits<T>, typename StdAllocT = std::allocator<T>>
    constexpr std::basic_string<T, TraitsT, StdAllocT> ToStdString() const;

    explicit constexpr operator std::basic_string<T>() const;

private:
    BasicString(NullptrT) = delete;

    constexpr SizeT GetVersion() const noexcept;
    constexpr Ptr GetBuffer() const noexcept;
    constexpr void SetSize(SizeT) noexcept;
    constexpr void DestroyStorage();
    constexpr void SwitchToLarge(LargeBuffer&&, SizeT);
    constexpr LargeBuffer SwitchToSmall(Ptr, SizeT);

    constexpr Ptr InitWithSize(SizeT);
    constexpr void InitEmpty();
    constexpr void InitWithCopy(BasicString const&);
    constexpr void InitWithMove(BasicString&&) noexcept;

    template <ForwardIteratableByT<T> Iter>
    constexpr void AssignWithRange(Iter, Iter, SizeT);
    constexpr void AssignWithMove(BasicString&&) noexcept;
    constexpr void AssignWithSource(ConstPtr, SizeT);

    template <ForwardIteratableByT<T> Iter>
    constexpr void AppendWithRange(Iter, Iter, SizeT);
    constexpr void AppendWithSource(ConstPtr, SizeT);
    constexpr Ptr AppendWithSize(SizeT);

    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertWithRange(SizeT, Iter, Iter, SizeT);
    constexpr void InsertWithSource(SizeT, ConstPtr, SizeT);
    constexpr Ptr InsertWithSize(SizeT, SizeT);

    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
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
    if (m_Large.layout == 0) {
        return;
    }

    m_Large.buffer.Deallocate(m_Alloc);
    m_Large.size = 0;
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
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other) noexcept
    : m_Alloc{}
{
    InitWithMove(MoveArg(other));
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other,
                                                     AllocT const& alloc) noexcept
    : m_Alloc(alloc)
{
    InitWithMove(MoveArg(other));
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
    if (IsConstantEvaluated() || capacity > SmallCapacity) {
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Allocate(capacity + 1, m_Alloc);
        m_Large.layout = 1;
        m_Large.size = 0;
    }
    else {
        memory::ConstructAt(&m_Small);
        m_Small.layout = 0;
        m_Small.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(Value ch, SizeT size, AllocT const& alloc)
    : m_Alloc(alloc)
{
    Ptr loc = InitWithSize(size);
    memory::StringFill(loc, ch, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(ConstPtr ptr, AllocT const& alloc)
    : m_Alloc(alloc)
{
    if (ptr == nullptr) [[unlikely]] {
        InitEmpty();
        return;
    }

    SizeT size = memory::StringLength(ptr);
    Ptr loc = InitWithSize(size);

    if (size != 0) {
        memory::StringCopy(loc, ptr, size);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(ConstPtr ptr, SizeT size, AllocT const& alloc)
    : m_Alloc(alloc)
{
    if (ptr == nullptr || size == 0) [[unlikely]] {
        InitEmpty();
        return;
    }

    Ptr loc = InitWithSize(size);
    memory::StringCopy(loc, ptr, size);
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

    Ptr loc = InitWithSize(size);
    memory::CopyRange(loc, begin, end);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(ConstPtr ptr)
{
    if (ptr == nullptr) [[unlikely]] {
        Clear();
        return;
    }

    SizeT len = memory::StringLength(ptr);
    if (len == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithSource(ptr, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(ConstPtr ptr, SizeT size)
{
    if (ptr == nullptr || size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithSource(ptr, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString const& other)
{
    SizeT otherSize = other.Size();
    if (otherSize == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithSource(other.GetBuffer(), otherSize);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString&& other)
{
    // prevent self destruction
    if (GetBuffer() == other.GetBuffer()) [[unlikely]] {
        return;
    }

    AssignWithMove(MoveArg(other));
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Assign(Iter begin, Iter end)
{
    SizeT size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithRange(begin, end, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Push(Value ch)
{
    Ptr buffer = AppendWithSize(1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Push(Value ch, SizeT count)
{
    if (count == 0) [[unlikely]] {
        return;
    }

    Ptr buffer = AppendWithSize(count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Append(ConstPtr ptr)
{
    if (ptr == nullptr) [[unlikely]] {
        return;
    }

    SizeT size = memory::StringLength(ptr);
    if (size == 0) [[unlikely]] {
        return;
    }

    AppendWithSource(ptr, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Append(ConstPtr ptr, SizeT size)
{
    if (ptr == nullptr || size == 0) [[unlikely]] {
        return;
    }

    AppendWithSource(ptr, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Append(BasicString const& other)
{
    SizeT otherSize = other.Size();
    if (otherSize == 0) [[unlikely]] {
        return;
    }

    AppendWithSource(other.GetBuffer(), otherSize);
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
    Ptr loc = InsertWithSize(index, 1);
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
    Ptr loc = InsertWithSize(index, count);
    memory::StringFill(loc, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, ConstPtr ptr)
{
    if (index == Size()) {
        Append(ptr);
        return;
    }

    if (ptr == nullptr) [[unlikely]] {
        return;
    }

    SizeT len = memory::StringLength(ptr);
    if (len == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    InsertWithSource(index, ptr, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, ConstPtr ptr, SizeT len)
{
    if (index == Size()) {
        Append(ptr, len);
        return;
    }

    if (ptr == nullptr || len == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    InsertWithSource(index, ptr, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, BasicString const& other)
{
    if (index == Size()) {
        Append(other);
        return;
    }

    SizeT otherSize = other.Size();
    if (otherSize == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    InsertWithSource(index, other.GetBuffer(), otherSize);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == Size()) {
        Push(ch);
        return;
    }

    AssertValidIterator(iter);
    Ptr loc = InsertWithSize(index, 1);
    *loc = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch, SizeT count)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == Size()) {
        Push(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Ptr buffer = InsertWithSize(index, count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, ConstPtr ptr)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == Size()) {
        Append(ptr);
        return;
    }

    if (ptr == nullptr) [[unlikely]] {
        return;
    }

    SizeT len = memory::StringLength(ptr);
    if (len == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithSource(index, ptr, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, ConstPtr ptr, SizeT len)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == Size()) {
        Append(ptr, len);
        return;
    }

    if (ptr == nullptr || len == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithSource(index, ptr, len);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, BasicString const& other)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == Size()) {
        Append(other);
        return;
    }

    SizeT otherSize = other.Size();
    if (otherSize == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithSource(index, other.GetBuffer(), otherSize);
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
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
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

    SetSize(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast(SizeT count)
{
    SizeT oldSize = Size();
    if (oldSize == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = oldSize < count ? oldSize : count;
    SetSize(oldSize - removeCnt);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(SizeT index)
{
    SizeT oldSize = Size();
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    Ptr loc = GetBuffer() + index;
    memory::StringMove(loc, loc + 1, oldSize - index - 1);
    SetSize(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(ConstIterator iter)
{
    Ptr buffer = GetBuffer();
    SizeT oldSize = Size();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Ptr loc = buffer + index;
    memory::StringMove(loc, loc + 1, oldSize - index - 1);
    SetSize(oldSize - 1);
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
        SetSize(index);
        return;
    }

    Ptr loc = GetBuffer() + index;
    memory::StringMove(loc, loc + count, oldSize - index - count);
    SetSize(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator iter, SizeT count)
{
    Ptr buffer = GetBuffer();
    SizeT oldSize = Size();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    if (index + count >= oldSize) [[unlikely]] {
        SetSize(index);
        return;
    }

    Ptr loc = buffer + index;
    memory::StringMove(loc, loc + count, oldSize - index - count);
    SetSize(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator begin, ConstIterator end)
{
    AssertValidIterator(begin);
    AssertValidIterator(end - 1);

    Ptr buffer = GetBuffer();
    SizeT oldSize = Size();
    SizeT count = static_cast<SizeT>(end - begin);
    if (count == 0) [[unlikely]] {
        return;
    }

    SizeT index = static_cast<SizeT>(begin.Address() - buffer);
    SizeT endIndex = static_cast<SizeT>(end.Address() - buffer);
    if (endIndex == oldSize) [[unlikely]] {
        SetSize(index);
        return;
    }

    Ptr loc = buffer + index;
    memory::StringMove(loc, end.Address(), oldSize - index - count);
    SetSize(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Resize(SizeT size, Value ch)
{
    SizeT oldSize = Size();
    if (oldSize == size) [[unlikely]] {
        return;
    }
    else if (size < oldSize) {
        SetSize(size);
        return;
    }

    if (size < Capacity()) {
        memory::StringFill(GetBuffer() + oldSize, ch, size - oldSize);
        SetSize(size);
    }
    else if (m_Large.layout == 0) {
        LargeBuffer newBuffer(size + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::StringCopy(buffer, m_Small.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        SwitchToLarge(MoveArg(newBuffer), size);
    }
    else {
        LargeBuffer newBuffer = m_Large.buffer.Resize(size + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::StringCopy(buffer, m_Large.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Large.size = size;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Reserve(SizeT size)
{
    if (size < Capacity()) [[unlikely]] {
        return;
    }

    if (m_Large.layout == 0) {
        LargeBuffer buffer(size + 1, m_Alloc);
        memory::StringCopy(buffer.Data(), m_Small.buffer.Data(), m_Small.size);
        SwitchToLarge(MoveArg(buffer), m_Small.size);
    }
    else {
        LargeBuffer buffer = m_Large.buffer.Resize(size + 1, m_Alloc);
        memory::StringCopy(buffer.Data(), m_Large.buffer.Data(), m_Large.size);
        m_Large.buffer.Assign(MoveArg(buffer), m_Alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Shrink()
{
    if (m_Large.layout == 0) [[unlikely]] {
        return;
    }

    SizeT oldSize = m_Large.size;
    LargeBuffer newBuffer = m_Large.buffer.Resize(oldSize + 1, m_Alloc);
    memory::StringCopy(newBuffer.Data(), m_Large.buffer.Data(), oldSize);
    m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Clear()
{
    SetSize(0);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Swap(BasicString& other) noexcept
{
    if (m_Large.layout == other.m_Large.layout) {
        if (m_Large.layout == 0) {
            T tmpBuffer[SmallCapacity];
            Ptr buffer = m_Small.buffer.Data();
            Ptr otherBuffer = other.m_Small.buffer.Data();
            byte otherSize = other.m_Small.size;

            memory::StringCopy(tmpBuffer, buffer, m_Small.size);
            memory::StringCopy(buffer, otherBuffer, otherSize);
            memory::StringCopy(otherBuffer, tmpBuffer, m_Small.size);
            other.m_Small.size = m_Small.size;
            m_Small.size = otherSize;
        }
        else {
            m_Large.buffer.Swap(other.m_Large.buffer);
            mini::Swap(m_Large.size, other.m_Large.size);
        }
    }
    else {
        if (m_Large.layout == 0) {
            SizeT size = other.m_Large.size;
            LargeBuffer buffer = other.SwitchToSmall(m_Small.buffer.Data(), m_Small.size);
            SwitchToLarge(MoveArg(buffer), size);
        }
        else {
            SizeT size = m_Large.size;
            LargeBuffer buffer = SwitchToSmall(other.m_Small.buffer.Data(), other.m_Small.size);
            other.SwitchToLarge(MoveArg(buffer), size);
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::Data() noexcept
{
    if (m_Large.layout == 0) {
        Ptr buffer = m_Small.buffer.Data();
        Ptr loc = buffer + m_Small.size;
        if (*loc != T(0)) {
            *loc = T(0);
        }

        return buffer;
    }
    else {
        Ptr buffer = m_Large.buffer.Data();
        Ptr loc = buffer + m_Large.size;
        if (*loc != T(0)) {
            *loc = T(0);
        }

        return buffer;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstPtr BasicString<T, AllocT>::Data() const noexcept
{
    if (m_Large.layout == 0) {
        ConstPtr buffer = m_Small.buffer.Data();
        ConstPtr loc = buffer + m_Small.size;
        if (*loc != T(0)) {
            *const_cast<Ptr>(loc) = T(0);
        }

        return buffer;
    }
    else {
        ConstPtr buffer = m_Large.buffer.Data();
        ConstPtr loc = buffer + m_Large.size;
        if (*loc != T(0)) {
            *const_cast<Ptr>(loc) = T(0);
        }

        return buffer;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::Begin() noexcept
{
    return Iterator(GetBuffer(), GetVersion(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator
BasicString<T, AllocT>::Begin() const noexcept
{
    return ConstIterator(GetBuffer(), GetVersion(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Iterator BasicString<T, AllocT>::End() noexcept
{
    return Iterator(GetBuffer() + Size(), GetVersion(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::End() const noexcept
{
    return ConstIterator(GetBuffer() + Size(), GetVersion(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *GetBuffer();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *GetBuffer();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::Last()
{
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(GetBuffer() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::Last() const
{
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(GetBuffer() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(GetBuffer() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(GetBuffer() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::GetVersion() const noexcept
{
    return m_Large.layout == 0 ? 0 : m_Large.buffer.Version();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::GetBuffer() const noexcept
{
    return const_cast<Ptr>(m_Large.layout == 0 ? m_Small.buffer.Data() : m_Large.buffer.Data());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Size() const noexcept
{
    return m_Large.layout == 0 ? m_Small.size : m_Large.size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Capacity() const noexcept
{
    return m_Large.layout == 0 ? SmallCapacity : (m_Large.buffer.Capacity() - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsEmpty() const noexcept
{
    return m_Large.layout == 0 ? m_Small.size == 0 : m_Large.size == 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIndex(SizeT index) const noexcept
{
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIterator(ConstIterator iter) const noexcept
{
    if (iter.m_Version != GetVersion()) {
        return false;
    }

    OffsetT dist = iter.Address() - GetBuffer();
    return dist >= 0 && dist < static_cast<OffsetT>(Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(GetBuffer() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef
BasicString<T, AllocT>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(GetBuffer() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString const& other)
{
    AssignWithSource(other.GetBuffer(), other.Size());
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString&& other) noexcept
{
    AssignWithMove(MoveArg(other));
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SetSize(SizeT size) noexcept
{
    if (m_Large.layout == 0) {
        m_Small.size = static_cast<byte>(size);
    }
    else {
        m_Large.size = size;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::DestroyStorage()
{
    if (m_Large.layout == 0) {
        return;
    }

    if (IsConstantEvaluated()) {
        m_Large.buffer.Deallocate(m_Alloc);
        m_Large.buffer.Allocate(1, m_Alloc);
        m_Large.size = 0;
    }
    else {
        m_Large.buffer.Deallocate(m_Alloc);
        memory::DestructAt(&m_Large);
        memory::ConstructAt(&m_Small);
        m_Small.layout = 0;
        m_Small.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SwitchToLarge(LargeBuffer&& buffer, SizeT size)
{
    memory::DestructAt(&m_Small);
    memory::ConstructAt(&m_Large);
    m_Large.buffer.Assign(MoveArg(buffer), m_Alloc);
    m_Large.layout = 1;
    m_Large.size = size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::LargeBuffer
BasicString<T, AllocT>::SwitchToSmall(Ptr ptr, SizeT size)
{
    LargeBuffer tmpBuffer(MoveArg(m_Large.buffer));
    memory::DestructAt(&m_Large);
    memory::ConstructAt(&m_Small);
    memory::StringCopy(m_Small.buffer.Data(), ptr, size);
    m_Small.layout = 0;
    m_Small.size = static_cast<byte>(size);
    return tmpBuffer;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InitWithSize(SizeT size)
{
    if (IsConstantEvaluated() || size > SmallCapacity) {
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Allocate(size + 1, m_Alloc);
        m_Large.layout = 1;
        m_Large.size = size;
        return m_Large.buffer.Data();
    }
    else {
        memory::ConstructAt(&m_Small);
        m_Small.layout = 0;
        m_Small.size = static_cast<byte>(size);
        return m_Small.buffer.Data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitEmpty()
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Allocate(1, m_Alloc);
        m_Large.layout = 1;
        m_Large.size = 0;
    }
    else {
        memory::ConstructAt(&m_Small);
        m_Small.layout = 0;
        m_Small.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithCopy(BasicString const& other)
{
    if (IsConstantEvaluated() || other.m_Large.layout == 1) {
        SizeT size = other.m_Large.size;
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Allocate(size + 1, m_Alloc);
        m_Large.layout = 1;
        memory::StringCopy(m_Large.buffer.Data(), other.m_Large.buffer.Data(), size);
        m_Large.size = size;
    }
    else {
        byte size = other.m_Small.size;
        memory::ConstructAt(&m_Small);
        memory::StringCopy(m_Small.buffer.Data(), other.m_Small.buffer.Data(), size);
        m_Small.size = size;
        m_Small.layout = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithMove(BasicString&& other) noexcept
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Assign(MoveArg(other.m_Large.buffer), m_Alloc);
        m_Large.layout = 1;
        m_Large.size = other.m_Large.size;
        other.DestroyStorage();
        return;
    }

    if (other.m_Large.layout == 1) {
        memory::ConstructAt(&m_Large);
        m_Large.buffer.Assign(MoveArg(other.m_Large.buffer), m_Alloc);
        m_Large.layout = 1;
        m_Large.size = other.m_Large.size;
        memory::DestructAt(&other.m_Large);
        memory::ConstructAt(&other.m_Small);
        other.m_Small.layout = 0;
        other.m_Small.size = 0;
    }
    else {
        byte size = other.m_Small.size;
        memory::ConstructAt(&m_Small);
        memory::StringCopy(m_Small.buffer.Data(), other.m_Small.buffer.Data(), size);
        m_Small.size = size;
        m_Small.layout = 0;
        other.m_Small.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithMove(BasicString&& other) noexcept
{
    if (other.m_Large.layout == 0) {
        SizeT size = other.m_Small.size;
        Ptr otherBuffer = other.m_Small.buffer.Data();

        if (m_Large.layout == 0) {
            memory::StringCopy(m_Small.buffer.Data(), otherBuffer, size);
            m_Small.size = static_cast<byte>(size);
        }
        else {
            memory::StringCopy(m_Large.buffer.Data(), otherBuffer, size);
            m_Large.size = size;
        }
    }
    else {
        if (m_Large.layout == 0) {
            memory::DestructAt(&m_Small);
            memory::ConstructAt(&m_Large);
            m_Large.layout = 1;
        }

        m_Large.buffer.Assign(MoveArg(other.m_Large.buffer), m_Alloc);
        m_Large.size = other.m_Large.size;

        other.DestroyStorage();
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::AssignWithRange(Iter begin, Iter end, SizeT size)
{
    if (size <= Capacity()) {
        // force copy to prevent data loss
        BasicString temp(size, m_Alloc);
        Ptr buffer = temp.GetBuffer();
        memory::CopyRange(buffer, begin, end);

        if (m_Large.layout == 0) {
            memory::StringCopy(m_Small.buffer.Data(), buffer, size);
            m_Small.size = static_cast<byte>(size);
        }
        else {
            memory::StringCopy(m_Large.buffer.Data(), buffer, size);
            m_Large.size = size;
        }
    }
    else {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(size + 1, m_Alloc);
            memory::CopyRange(newBuffer.Data(), begin, end);
            SwitchToLarge(MoveArg(newBuffer), size);
        }
        else {
            LargeBuffer buffer = m_Large.buffer.Resize(size + 1, m_Alloc);
            memory::CopyRange(buffer.Data(), begin, end);
            m_Large.buffer.Assign(MoveArg(buffer), m_Alloc);
            m_Large.size = size;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithSource(ConstPtr ptr, SizeT len)
{
    if (len <= Capacity()) {
        if (m_Large.layout == 0) {
            memory::StringMove(m_Small.buffer.Data(), ptr, len);
            m_Small.size = static_cast<byte>(len);
        }
        else {
            memory::StringMove(m_Large.buffer.Data(), ptr, len);
            m_Large.size = len;
        }
    }
    else {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(len + 1, m_Alloc);
            memory::StringCopy(newBuffer.Data(), ptr, len);
            SwitchToLarge(MoveArg(newBuffer), len);
        }
        else {
            LargeBuffer buffer = m_Large.buffer.Resize(len + 1, m_Alloc);
            memory::StringCopy(buffer.Data(), ptr, len);
            m_Large.buffer.Assign(MoveArg(buffer), m_Alloc);
            m_Large.size = len;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::AppendWithRange(Iter begin, Iter end, SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;
    Ptr oldBuffer = GetBuffer();

    if (m_Large.layout == 0) {
        if (newSize > SmallCapacity) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            memory::CopyRange(oldBuffer + oldSize, begin, end);
            m_Small.size = static_cast<byte>(newSize);
        }
    }
    else {
        if (newSize > m_Large.buffer.Capacity()) {
            LargeBuffer newBuffer = m_Large.buffer.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        }
        else {
            memory::CopyRange(oldBuffer + oldSize, begin, end);
        }

        m_Large.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AppendWithSource(ConstPtr ptr, SizeT len)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;
    Ptr oldBuffer = GetBuffer();

    if (m_Large.layout == 0) {
        if (newSize > SmallCapacity) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, oldSize);
            memory::StringCopy(buffer + oldSize, ptr, len);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            memory::StringCopy(oldBuffer + oldSize, ptr, len);
            m_Small.size = static_cast<byte>(newSize);
        }
    }
    else {
        if (newSize > m_Large.buffer.Capacity()) {
            LargeBuffer newBuffer = m_Large.buffer.Increment(len, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, oldSize);
            memory::StringCopy(buffer + oldSize, ptr, len);
            m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        }
        else {
            memory::StringCopy(oldBuffer + oldSize, ptr, len);
        }

        m_Large.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::AppendWithSize(SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize > Capacity()) {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            memory::StringCopy(newBuffer.Data(), m_Small.buffer.Data(), oldSize);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer buffer = m_Large.buffer.Increment(size, m_Alloc);
            memory::StringCopy(buffer.Data(), m_Large.buffer.Data(), oldSize);
            m_Large.buffer.Assign(MoveArg(buffer), m_Alloc);
        }

        m_Large.size = newSize;
        return m_Large.buffer.Data() + oldSize;
    }
    else {
        if (m_Large.layout == 0) {
            m_Small.size = static_cast<byte>(newSize);
            return m_Small.buffer.Data() + oldSize;
        }
        else {
            m_Large.size = newSize;
            return m_Large.buffer.Data() + oldSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InsertWithSource(SizeT index, ConstPtr ptr, SizeT len)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;
    Ptr oldBuffer = GetBuffer();
    Ptr loc = oldBuffer + index;

    if (newSize <= Capacity()) {
        Ptr oldEnd = oldBuffer + oldSize;
        if (memory::IsPtrOverlapping(ptr, oldBuffer, oldEnd)) {
            BasicString temp(ptr, len, m_Alloc);
            memory::StringMove(loc + len, loc, oldSize - index);
            memory::StringCopy(loc, temp.GetBuffer(), len);
        }
        else {
            memory::StringMove(loc + len, loc, oldSize - index);
            memory::StringCopy(loc, ptr, len);
        }

        SetSize(newSize);
    }
    else {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + len, loc, oldSize - index);
            memory::StringCopy(buffer + index, ptr, len);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer newBuffer = m_Large.buffer.Increment(len, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + len, loc, oldSize - index);
            memory::StringCopy(buffer + index, ptr, len);
            m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
            m_Large.size = newSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InsertWithSize(SizeT index,
                                                                                    SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize > Capacity()) {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            Ptr oldBuffer = m_Small.buffer.Data();

            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + size, oldBuffer + index, oldSize - index);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer newBuffer = m_Large.buffer.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
            Ptr oldBuffer = m_Large.buffer.Data();

            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + size, oldBuffer + index, oldSize - index);
            m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
            m_Large.size = newSize;
        }

        return m_Large.buffer.Data() + index;
    }
    else {
        if (m_Large.layout == 0) {
            Ptr loc = m_Small.buffer.Data() + index;
            memory::StringMove(loc + size, loc, oldSize - index);
            m_Small.size = static_cast<byte>(newSize);
            return loc;
        }
        else {
            Ptr loc = m_Large.buffer.Data() + index;
            memory::StringMove(loc + size, loc, oldSize - index);
            m_Large.size = newSize;
            return loc;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertWithRange(SizeT index, Iter begin, Iter end,
                                                              SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;
    Ptr oldBuffer = GetBuffer();
    Ptr loc = oldBuffer + index;

    if (newSize <= Capacity()) {
        BasicString temp(size, m_Alloc);
        Ptr buffer = temp.GetBuffer();
        memory::CopyRange(buffer, begin, end);
        memory::StringMove(loc + size, loc, oldSize - index);
        memory::StringCopy(loc, temp.GetBuffer(), size);
        SetSize(newSize);
    }
    else {
        if (m_Large.layout == 0) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer newBuffer = m_Large.buffer.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, oldBuffer, index);
            memory::StringCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            m_Large.buffer.Assign(MoveArg(newBuffer), m_Alloc);
            m_Large.size = newSize;
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ConvertibleToT<T> U, typename TraitsU, typename AllocU>
inline constexpr BasicString<T, AllocT>::
    BasicString(std::basic_string<U, TraitsU, AllocU> const& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    SizeT size = static_cast<SizeT>(other.size());
    Ptr buffer = InitWithSize(size);
    memory::StringCopy(buffer, other.data(), size);
}

template <CharT T, AllocatorT<T> AllocT>
template <typename TraitsT, typename StdAllocT>
inline constexpr std::basic_string<T, TraitsT, StdAllocT>
BasicString<T, AllocT>::ToStdString() const
{
    return std::basic_string<T, TraitsT, StdAllocT>(GetBuffer(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator std::basic_string<T>() const
{
    return std::basic_string<T>(GetBuffer(), Size());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(IsValidIndex(index), "invalid index");
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    [[maybe_unused]] OffsetT dist = iter.m_Ptr - GetBuffer();
    ASSERT(iter.m_Version == GetVersion(), "invalid version");
    ASSERT(dist >= 0 && dist < static_cast<OffsetT>(Size()), "invalid range");
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& l,
                                 BasicString<T, AllocT> const& r) noexcept
{
    SizeT size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    using Ptr = BasicString<T, AllocT>::ConstPtr;
    Ptr lbuf = l.Begin().Address();
    Ptr rbuf = r.Begin().Address();

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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Begin().Address();
    typename BasicString<U, AllocU>::ConstPtr rbuf = r.Begin().Address();
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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Begin().Address();
    typename std::basic_string<T, StdAllocT>::const_pointer rbuf = r.begin().operator->();

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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Begin().Address();
    typename std::basic_string<T, StdAllocU>::const_pointer rbuf = r.begin().operator->();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool operator==(BasicString<T, AllocT> const& s, T const* p) noexcept
{
    if (p == nullptr) [[unlikely]] {
        return false;
    }

    SizeT len = memory::StringLength(p);
    SizeT size = s.Size();
    if (size != len) {
        return false;
    }

    typename BasicString<T, AllocT>::ConstPtr buffer = s.Begin().Address();
    if (buffer == p) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(buffer, p, size) == 0;
}

export template <CharT T, AllocatorT<T> AllocT>
bool operator==(BasicString<T, AllocT> const&, NullptrT) = delete;

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr void Swap(BasicString<T, AllocT> const& l,
                           BasicString<T, AllocT> const& r) noexcept
{
    l.Swap(r);
}

} // namespace mini