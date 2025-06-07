module;

#include <string>

#include "assertion.h"

#if CLANG || GNUC
#  define PACKED_STRUCT_BEGIN(x) __attribute__((packed))
#  define PACKED_STRUCT_END

// TODO: might be a bug when using with c++20 modules
//  windows standalone will mostly pack bit-fields, so it may not be necessary..?
// #elif MSVC
// #  define PACKED_STRUCT_BEGIN(x) __pragma(pack(push, x))
// #  define PACKED_STRUCT_END      __pragma(pack(pop))

#else
#  define PACKED_STRUCT_BEGIN(x)
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

template <CharT T, AllocatorT<T> AllocT>
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
        PACKED_STRUCT_BEGIN(1)
        struct {
            SizeT layout : 1;
            SizeT size : (sizeof(SizeT) * 8 - 1);
        };
        PACKED_STRUCT_END
        LargeBuffer buffer;

        constexpr LargeStorage() = default;
        constexpr LargeStorage(LargeStorage&&) = default;
        constexpr ~LargeStorage() = default;
    };

    static constexpr SizeT AllocatedSize = (sizeof(LargeStorage) / sizeof(T)) - 2;
    static constexpr SizeT SmallCapacity = AllocatedSize > 2 ? AllocatedSize : 2;

    typedef StaticBuffer<T, SmallCapacity + 1> SmallBuffer;
    typedef StaticBuffer<byte, sizeof(LargeStorage)> RawBuffer;

    struct SmallStorage {
        PACKED_STRUCT_BEGIN(1)
        struct {
            byte layout : 1;
            byte size   : 7;
        };
        PACKED_STRUCT_END
        SmallBuffer buffer;

        constexpr SmallStorage() = default;
        constexpr ~SmallStorage() = default;
    };

    union Storage {
        SmallStorage s;
        LargeStorage l;
        RawBuffer raw;
    };

    [[no_unique_address]] AllocT m_Alloc;
    Storage m_Storage;

    static_assert(SmallCapacity < 128, "small capacity should not excced 127");
    static_assert(sizeof(LargeStorage) == sizeof(SmallStorage));
    static_assert(sizeof(LargeStorage) == sizeof(RawBuffer));

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
    constexpr BasicString(ConstPtr, AllocT const& = AllocT());
    constexpr BasicString(ConstPtr, SizeT, AllocT const& = AllocT());
    template <typename U>
    constexpr BasicString(U, SizeT, AllocT const& = AllocT())
        requires AnyOfT<U, Value, ConstValue>;
    template <ForwardIteratableByT<T> Iter>
    constexpr BasicString(Iter, Iter, AllocT const& = AllocT());

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
    constexpr void Swap(BasicString&);

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
    constexpr bool IsValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Ref operator[](SizeT);
    constexpr ConstRef operator[](SizeT) const;

    constexpr BasicString& operator=(BasicString const&);
    constexpr BasicString& operator=(BasicString&&);
    constexpr BasicString& operator=(ConstPtr);

    constexpr BasicString& operator+=(BasicString const&);
    constexpr BasicString& operator+=(ConstPtr);
    constexpr BasicString& operator+=(Value);

    template <ConvertibleToT<T> U, typename TraitsU, typename AllocU>
    constexpr BasicString(std::basic_string<U, TraitsU, AllocU> const&, AllocT const& = AllocT());

    explicit constexpr operator std::basic_string<T>() const;

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

    constexpr bool IsLarge() const noexcept;
    constexpr void SetSizeWithNullTerminator(SizeT) noexcept;
    constexpr void DestroyBuffer();
    constexpr void ResizeWithAlloc(SizeT, Value, SizeT);
    constexpr void SwitchToLarge(LargeBuffer&&, SizeT);
    constexpr LargeBuffer SwitchToSmall(Ptr, SizeT);

    constexpr void InitEmpty();
    constexpr void InitWithCopy(BasicString const&);
    constexpr void InitWithMove(BasicString&&) noexcept;
    constexpr Ptr InitWithSize(SizeT);

    constexpr void AssignWithAlloc(ConstPtr, SizeT);
    constexpr void AssignWithSource(ConstPtr, SizeT);
    constexpr void AssignWithMove(BasicString&&) noexcept;

    template <ForwardIteratableByT<T> Iter>
    constexpr void AppendWithRange(Iter, Iter, SizeT);
    constexpr void AppendWithSource(ConstPtr, SizeT);
    constexpr void AppendWithAlloc(ConstPtr, SizeT, SizeT);
    constexpr void AppendWithAlloc(SizeT, SizeT);
    constexpr Ptr AppendWithSize(SizeT);

    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertWithRange(SizeT, Iter, Iter, SizeT);
    constexpr void InsertWithSource(SizeT, ConstPtr, SizeT);
    constexpr void InsertWithAlloc(SizeT, ConstPtr, SizeT, SizeT);
    constexpr void InsertWithAlloc(SizeT, SizeT, SizeT);
    constexpr Ptr InsertWithSize(SizeT, SizeT);

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
    if (IsLarge()) {
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
    if (other.IsLarge() && m_Alloc != other.m_Alloc) {
        InitWithCopy(other);
    }
    else {
        InitWithMove(MoveArg(other));
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    if (other.IsLarge() && m_Alloc != other.m_Alloc) {
        InitWithCopy(other);
    }
    else {
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
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Storage.l);
    }

    if (IsConstantEvaluated() || capacity > SmallCapacity) {
        memory::ConstructAt(&m_Storage.l.buffer, capacity + 1, m_Alloc);
        m_Storage.l.buffer.Data()[0] = Value(0);
        m_Storage.l.layout = 1;
        m_Storage.l.size = 0;
    }
    else {
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
    if (size == 0) [[unlikely]] {
        InitEmpty();
        return;
    }

    Ptr loc = InitWithSize(size);
    memory::MemCopy(loc, ptr, size);
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
    memory::MemCopy(loc, ptr, size);
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
    if (!IsLarge() && !other.IsLarge()) {
        m_Storage.raw = other.m_Storage.raw;
        return;
    }

    SizeT otherSize = other.m_Storage.l.size;
    if (otherSize == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithSource(other.m_Storage.l.buffer.Data(), otherSize);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString&& other)
{
    if (m_Alloc == other.m_Alloc) [[likely]] {
        AssignWithMove(MoveArg(other));
    }
    else {
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

    Ptr buffer = AppendWithSize(1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Push(Value ch, SizeT count)
{
    if (ch == Value(0) || count == 0) [[unlikely]] {
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

    AppendWithSource(other.Data(), otherSize);
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
    InsertWithSource(index, other.Data(), otherSize);
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
    Ptr loc = InsertWithSize(index, 1);
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
    Ptr buffer = InsertWithSize(index, count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, ConstPtr ptr)
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
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
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
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
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    if (index == Size()) {
        Append(other);
        return;
    }

    SizeT otherSize = other.Size();
    if (otherSize == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithSource(index, other.Data(), otherSize);
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
    Ptr loc = Data() + index;
    memory::MemMove(loc, loc + 1, oldSize - index - 1);
    SetSizeWithNullTerminator(oldSize - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(ConstIterator iter)
{
    Ptr buffer = Data();
    SizeT oldSize = Size();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == oldSize) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Ptr loc = buffer + index;
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

    Ptr loc = Data() + index;
    memory::MemMove(loc, loc + count, oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator iter, SizeT count)
{
    Ptr buffer = Data();
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

    Ptr loc = buffer + index;
    memory::MemMove(loc, loc + count, oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator begin, ConstIterator end)
{
    AssertValidRange(begin, end);
    Ptr buffer = Data();
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

    Ptr loc = buffer + index;
    memory::MemMove(loc, end.Address(), oldSize - index - count);
    SetSizeWithNullTerminator(oldSize - count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Resize(SizeT size, Value ch)
{
    SizeT oldSize = Size();
    if (oldSize == size) [[unlikely]] {
        return;
    }
    else if (size < oldSize) {
        SetSizeWithNullTerminator(size);
        return;
    }

    if (size < Capacity()) {
        memory::StringFill(Data() + oldSize, ch, size - oldSize);
        SetSizeWithNullTerminator(size);
    }
    else {
        ResizeWithAlloc(size, ch, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::ResizeWithAlloc(SizeT size, Value ch,
                                                                     SizeT oldSize)
{
    if (!IsLarge()) {
        LargeBuffer newBuffer(size + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::MemCopy(buffer, m_Storage.s.buffer.Data(), oldSize);
        memory::StringFill(buffer + oldSize, ch, size - oldSize);
        SwitchToLarge(MoveArg(newBuffer), size);
    }
    else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Resize(size + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
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

    if (!IsLarge()) {
        LargeBuffer buffer(size + 1, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.s.buffer.Data(), m_Storage.s.size);
        SwitchToLarge(MoveArg(buffer), m_Storage.s.size);
    }
    else {
        LargeBuffer buffer = m_Storage.l.buffer.Resize(size + 1, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.l.buffer.Data(), m_Storage.l.size + 1);
        m_Storage.l.buffer.Assign(MoveArg(buffer), m_Alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Shrink()
{
    if (!IsLarge()) [[unlikely]] {
        return;
    }

    SizeT size = m_Storage.l.size;
    if (!IsConstantEvaluated() && size <= SmallCapacity) {
        Ptr buffer = m_Storage.l.buffer.Data();
        memory::MemCopy(m_Storage.s.buffer.Data(), buffer, size + 1);
        m_Storage.s.layout = 0;
        m_Storage.s.size = static_cast<byte>(size);
        m_Alloc.Deallocate(buffer, size);
    }
    else {
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
    if (IsConstantEvaluated()) {
        SizeT size = m_Storage.l.size;
        m_Storage.l.buffer.Swap(other.m_Storage.l.buffer);
        m_Storage.l.size = other.m_Storage.l.size;
        other.m_Storage.l.size = size;
        return;
    }

    mini::Swap(m_Storage.raw, other.m_Storage.raw);
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
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::Last()
{
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::Last() const
{
    SizeT size = Size();
    AssertValidIndex(size - 1);
    return *(Data() + size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::Data() noexcept
{
    return IsLarge() ? m_Storage.l.buffer.Data() : m_Storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstPtr BasicString<T, AllocT>::Data() const noexcept
{
    return IsLarge() ? m_Storage.l.buffer.Data() : m_Storage.s.buffer.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Size() const noexcept
{
    return IsLarge() ? m_Storage.l.size : m_Storage.s.size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Capacity() const noexcept
{
    return IsLarge() ? (m_Storage.l.buffer.Capacity() - 1) : SmallCapacity;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsEmpty() const noexcept
{
    return IsLarge() ? m_Storage.l.size == 0 : m_Storage.s.size == 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIndex(SizeT index) const noexcept
{
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.Address() - Data());
    return index < Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidRange(ConstIterator begin,
                                                           ConstIterator end) const noexcept
{
    ConstPtr buffer = Data();
    SizeT size = Size();
    SizeT beginIdx = static_cast<SizeT>(begin.Address() - buffer);
    SizeT endIdx = static_cast<SizeT>(end.Address() - buffer);
    return (beginIdx < size) && (endIdx <= size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ref BasicString<T, AllocT>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(Data() + index);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef
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
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString&& other)
{
    Assign(MoveArg(other));
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(ConstPtr ptr)
{
    Assign(ptr);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>&
BasicString<T, AllocT>::operator+=(BasicString const& other)
{
    Append(other);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(ConstPtr ptr)
{
    Append(ptr);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator+=(Value ch)
{
    Push(ch);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsLarge() const noexcept
{
    if (IsConstantEvaluated()) {
        return m_Storage.l.layout == 1;
    }

    return m_Storage.s.layout == 1;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SetSizeWithNullTerminator(SizeT size) noexcept
{
    Ptr buffer = nullptr;
    if (IsLarge()) {
        m_Storage.l.size = size;
        buffer = m_Storage.l.buffer.Data();
    }
    else {
        m_Storage.s.size = static_cast<byte>(size);
        buffer = m_Storage.s.buffer.Data();
    }

    buffer[size] = Value(0);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::DestroyBuffer()
{
    if (!IsLarge()) {
        return;
    }

    m_Storage.l.buffer.Deallocate(m_Alloc);

    if (IsConstantEvaluated()) {
        m_Storage.l.buffer.Allocate(1, m_Alloc);
        m_Storage.l.buffer.Data()[0] = Value(0);
        m_Storage.l.size = 0;
    }
    else {
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
BasicString<T, AllocT>::SwitchToSmall(Ptr ptr, SizeT size)
{
    LargeBuffer tmpBuffer(MoveArg(m_Storage.l.buffer));
    memory::MemCopy(m_Storage.s.buffer.Data(), ptr, size);
    m_Storage.s.buffer.Data()[size] = Value(0);
    m_Storage.s.layout = 0;
    m_Storage.s.size = static_cast<byte>(size);
    return tmpBuffer;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitEmpty()
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Allocate(1, m_Alloc);
        m_Storage.l.buffer.Data()[0] = Value(0);
        m_Storage.l.layout = 1;
        m_Storage.l.size = 0;
    }
    else {
        m_Storage.s.buffer.Data()[0] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = 0;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithCopy(BasicString const& other)
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Storage.l);
    }

    if (other.IsLarge()) {
        SizeT size = other.m_Storage.l.size;
        memory::ConstructAt(&m_Storage.l.buffer, size + 1, m_Alloc);
        memory::MemCopy(m_Storage.l.buffer.Data(), other.m_Storage.l.buffer.Data(), size + 1);
        m_Storage.l.layout = 1;
        m_Storage.l.size = size;
    }
    else {
        m_Storage.raw = other.m_Storage.raw;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithMove(BasicString&& other) noexcept
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Storage.l);
        m_Storage.l.buffer.Assign(MoveArg(other.m_Storage.l.buffer), m_Alloc);
        m_Storage.l.layout = 1;
        m_Storage.l.size = other.m_Storage.l.size;
        other.DestroyBuffer();
    }
    else {
        m_Storage.raw = other.m_Storage.raw;
        other.InitEmpty();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InitWithSize(SizeT size)
{
    if (IsConstantEvaluated()) {
        memory::ConstructAt(&m_Storage.l);
    }

    if (IsConstantEvaluated() || size > SmallCapacity) {
        memory::ConstructAt(&m_Storage.l.buffer, size + 1, m_Alloc);
        m_Storage.l.buffer.Data()[size] = Value(0);
        m_Storage.l.layout = 1;
        m_Storage.l.size = size;
        return m_Storage.l.buffer.Data();
    }
    else {
        m_Storage.s.buffer.Data()[size] = Value(0);
        m_Storage.s.layout = 0;
        m_Storage.s.size = static_cast<byte>(size);
        return m_Storage.s.buffer.Data();
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithMove(BasicString&& other) noexcept
{
    if (IsConstantEvaluated()) {
        m_Storage.l.buffer.Assign(MoveArg(other.m_Storage.l.buffer), m_Alloc);
        m_Storage.l.size = other.m_Storage.l.size;
        other.DestroyBuffer();
        return;
    }

    mini::Swap(m_Storage.raw, other.m_Storage.raw);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithSource(ConstPtr ptr, SizeT len)
{
    if (IsConstantEvaluated() || len > SmallCapacity) {
        AssignWithAlloc(ptr, len);
        return;
    }

    memory::MemMove(Data(), ptr, len);
    SetSizeWithNullTerminator(len);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AssignWithAlloc(ConstPtr ptr, SizeT len)
{
    if (!IsLarge()) {
        LargeBuffer newBuffer(len + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), ptr, len);
        SwitchToLarge(MoveArg(newBuffer), len);
    }
    else {
        Ptr buffer;
        if (m_Storage.l.buffer.Capacity() < len) {
            LargeBuffer newBuffer = m_Storage.l.buffer.Resize(len + 1, m_Alloc);
            buffer = newBuffer.Data();
            memory::MemCopy(buffer, ptr, len);
            m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        }
        else {
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
    }
    else {
        if (!IsLarge()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_Storage.s.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::MemCopy(buffer, m_Storage.l.buffer.Data(), oldSize);
            memory::CopyRange(buffer + oldSize, begin, end);
            m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        }
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AppendWithSource(ConstPtr ptr, SizeT len)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;

    if (IsConstantEvaluated() || newSize > Capacity()) {
        AppendWithAlloc(ptr, len, oldSize);
        return;
    }

    memory::MemCopy(Data() + oldSize, ptr, len);
    SetSizeWithNullTerminator(newSize);
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(ConstPtr ptr, SizeT len,
                                                                     SizeT oldSize)
{
    SizeT newSize = oldSize + len;
    Ptr oldBuffer = Data();

    if (!IsLarge()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    }
    else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(len, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, oldSize);
        memory::MemCopy(buffer + oldSize, ptr, len);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.buffer.Data()[newSize] = Value(0);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::AppendWithSize(SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize <= Capacity()) {
        SetSizeWithNullTerminator(newSize);
        return Data() + oldSize;
    }
    else {
        AppendWithAlloc(size, oldSize);
        return m_Storage.l.buffer.Data() + oldSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::AppendWithAlloc(SizeT size, SizeT oldSize)
{
    SizeT newSize = oldSize + size;

    if (!IsLarge()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        memory::MemCopy(newBuffer.Data(), m_Storage.s.buffer.Data(), oldSize);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    }
    else {
        LargeBuffer buffer = m_Storage.l.buffer.Increment(size, m_Alloc);
        memory::MemCopy(buffer.Data(), m_Storage.l.buffer.Data(), oldSize);
        m_Storage.l.buffer.Assign(MoveArg(buffer), m_Alloc);
        m_Storage.l.buffer.Data()[newSize] = Value(0);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InsertWithSource(SizeT index, ConstPtr ptr, SizeT len)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + len;

    if (newSize <= Capacity()) {
        Ptr oldBuffer = Data();
        Ptr loc = oldBuffer + index;

        if (memory::IsPtrOverlapping(ptr, oldBuffer, oldBuffer + oldSize)) {
            BasicString temp(ptr, len, m_Alloc);
            ptr = temp.Data();
        }

        memory::MemMove(loc + len, loc, oldSize - index);
        memory::MemCopy(loc, ptr, len);
        SetSizeWithNullTerminator(newSize);
    }
    else {
        InsertWithAlloc(index, ptr, len, oldSize);
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::InsertWithAlloc(SizeT index, ConstPtr ptr,
                                                                     SizeT len, SizeT oldSize)
{
    SizeT newSize = oldSize + len;
    Ptr oldBuffer = Data();
    Ptr loc = oldBuffer + index;

    if (!IsLarge()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    }
    else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(len, m_Alloc);
        Ptr buffer = newBuffer.Data();
        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + len, loc, oldSize - index + 1);
        memory::MemCopy(buffer + index, ptr, len);
        m_Storage.l.buffer.Assign(MoveArg(newBuffer), m_Alloc);
        m_Storage.l.size = newSize;
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InsertWithSize(SizeT index,
                                                                                    SizeT size)
{
    SizeT oldSize = Size();
    SizeT newSize = oldSize + size;

    if (newSize <= Capacity()) {
        Ptr loc = Data() + index;
        memory::MemMove(loc + size, loc, oldSize - index);
        SetSizeWithNullTerminator(newSize);
        return loc;
    }
    else {
        InsertWithAlloc(index, size, oldSize);
        return m_Storage.l.buffer.Data() + index;
    }
}

template <CharT T, AllocatorT<T> AllocT>
[[no_inline]] constexpr void BasicString<T, AllocT>::InsertWithAlloc(SizeT index, SizeT size,
                                                                     SizeT oldSize)
{
    SizeT newSize = oldSize + size;

    if (!IsLarge()) {
        LargeBuffer newBuffer(newSize + 1, m_Alloc);
        Ptr buffer = newBuffer.Data();
        Ptr oldBuffer = m_Storage.s.buffer.Data();

        memory::MemCopy(buffer, oldBuffer, index);
        memory::MemCopy(buffer + index + size, oldBuffer + index, oldSize - index);
        SwitchToLarge(MoveArg(newBuffer), newSize);
    }
    else {
        LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
        Ptr buffer = newBuffer.Data();
        Ptr oldBuffer = m_Storage.l.buffer.Data();

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
    Ptr oldBuffer = Data();
    Ptr loc = oldBuffer + index;

    if (newSize <= Capacity()) {
        BasicString temp(size, m_Alloc);
        Ptr buffer = temp.Data();
        memory::CopyRange(buffer, begin, end);
        memory::MemMove(loc + size, loc, oldSize - index);
        memory::MemCopy(loc, temp.Data(), size);
        SetSizeWithNullTerminator(newSize);
    }
    else {
        if (!IsLarge()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::MemCopy(buffer, oldBuffer, index);
            memory::MemCopy(buffer + index + size, loc, oldSize - index);
            memory::CopyRange(buffer + index, begin, end);
            SwitchToLarge(MoveArg(newBuffer), newSize);
        }
        else {
            LargeBuffer newBuffer = m_Storage.l.buffer.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
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
template <ConvertibleToT<T> U, typename TraitsU, typename AllocU>
inline constexpr BasicString<T, AllocT>::
    BasicString(std::basic_string<U, TraitsU, AllocU> const& other, AllocT const& alloc)
    : m_Alloc(alloc)
{
    SizeT size = static_cast<SizeT>(other.size());
    Ptr buffer = InitWithSize(size);
    memory::MemCopy(buffer, other.data(), size + 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator std::basic_string<T>() const
{
    return std::basic_string<T>(Data(), Size());
}

export template <CharT T, AllocatorT<T> AllocT, ConvertibleToT<T> U = T,
                 typename TraitsU = std::char_traits<U>, typename AllocU = std::allocator<U>>
inline constexpr BasicString<T, AllocT> ToString(std::basic_string<U, TraitsU, AllocU> const& other,
                                                 AllocT const& alloc = AllocT())
{
    return BasicString<T, AllocT>(other.data(), other.size(), alloc);
}

export template <CharT T, AllocatorT<T> AllocT, ConvertibleToT<T> U = T,
                 typename TraitsU = std::char_traits<U>, typename AllocU = std::allocator<U>>
inline constexpr std::basic_string<U, TraitsU, AllocU>
ToStdString(BasicString<T, AllocT> const& other, AllocU const& alloc = AllocU())
{
    return std::basic_string<U, TraitsU, AllocU>(other.Data(), other.Size(), alloc);
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
    ASSERT(IsValidIterator(iter), "invalid iterator");
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void
BasicString<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                         [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(IsValidRange(begin, end), "invalid range");
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
    Ptr lbuf = l.Data();
    Ptr rbuf = r.Data();

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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Data();
    typename BasicString<U, AllocU>::ConstPtr rbuf = r.Data();
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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Data();
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

    typename BasicString<T, AllocT>::ConstPtr lbuf = l.Data();
    typename std::basic_string<T, StdAllocU>::const_pointer rbuf = r.data();
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

    typename BasicString<T, AllocT>::ConstPtr buffer = s.Data();
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