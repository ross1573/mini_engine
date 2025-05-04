module;

#include <string>

#include "assertion.h"

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

export using String = std::basic_string<char>;

export template <CharT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class BasicString {
private:
    static consteval SizeT SmallBufferSize() noexcept;

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
    typedef StaticBuffer<T, SmallBufferSize()> SmallBuffer;

private:
    [[no_unique_address]] AllocT m_Alloc;
    byte m_Layout;
    SizeT m_Size;

    union {
        SmallBuffer m_Small;
        LargeBuffer m_Large;
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
    constexpr void DestroyBuffer();
    constexpr void SwitchToLarge(LargeBuffer&&);
    constexpr LargeBuffer SwitchToSmall(Ptr, SizeT);
    constexpr bool IsOverlapping(ConstPtr) const noexcept;

    constexpr void InitWithCopy(BasicString const&);
    constexpr void InitWithMove(BasicString&&) noexcept;
    constexpr void AssignWithMove(BasicString&&) noexcept;
    constexpr void AssignWithSource(ConstPtr, SizeT);
    constexpr void AppendWithSource(ConstPtr, SizeT);
    constexpr void InsertWithSource(SizeT, ConstPtr, SizeT);
    constexpr Ptr InitWithSize(SizeT);
    constexpr Ptr AppendWithSize(SizeT);
    constexpr Ptr InsertWithSize(SizeT, SizeT);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertWithRange(SizeT, Iter, Iter);

    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
};

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString() noexcept
    : m_Alloc{}
    , m_Layout(0)
    , m_Size(0)
    , m_Small{}
{
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::~BasicString()
{
    DestroyBuffer();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other)
    : m_Alloc{}
    , m_Layout(0)
    , m_Size(0)
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString const& other, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    InitWithCopy(other);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other) noexcept
    : m_Alloc{}
    , m_Layout(0)
    , m_Size(0)
{
    InitWithMove(MoveArg(other));
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(BasicString&& other,
                                                     AllocT const& alloc) noexcept
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    InitWithMove(MoveArg(other));
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT const& alloc) noexcept
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
    , m_Small{}
{
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(AllocT&& alloc) noexcept
    : m_Alloc(MoveArg(alloc))
    , m_Layout(0)
    , m_Size(0)
    , m_Small{}
{
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(Value ch, SizeT size, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    Ptr buffer = InitWithSize(size);
    memory::StringFill(buffer, ch, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(ConstPtr ptr, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    if (ptr == nullptr) [[unlikely]] {
        memory::ConstructAt(&m_Small);
        return;
    }

    SizeT size = memory::StringLength(ptr);
    if (size == 0) [[unlikely]] {
        memory::ConstructAt(&m_Small);
        return;
    }

    Ptr buffer = InitWithSize(size);
    memory::StringCopy(buffer, ptr, size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::BasicString(ConstPtr ptr, SizeT size, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    if (ptr == nullptr || size == 0) [[unlikely]] {
        memory::ConstructAt(&m_Small);
        return;
    }

    Ptr buffer = InitWithSize(size);
    memory::StringCopy(buffer, ptr, size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr BasicString<T, AllocT>::BasicString(Iter begin, Iter end, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
{
    SizeT size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        memory::ConstructAt(&m_Small);
        return;
    }

    Ptr buffer = InitWithSize(size);
    memory::CopyRange(buffer, begin, end);
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
    if (other.m_Size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignWithSource(other.GetBuffer(), other.m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Assign(BasicString&& other)
{
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

    if (size <= SmallBufferSize()) {
        Ptr buffer = GetBuffer();
        memory::CopyRange(buffer, begin, end);
    }
    else {
        if (m_Layout == 0) {
            LargeBuffer newBuffer(size + 1, m_Alloc);
            memory::CopyRange(newBuffer.Data(), begin, end);
            SwitchToLarge(MoveArg(newBuffer));
        }
        else if (m_Large.Capacity() < size) {
            LargeBuffer newBuffer = m_Large.Resize(size + 1, m_Alloc);
            memory::CopyRange(newBuffer.Data(), begin, end);
            m_Large.Assign(MoveArg(newBuffer), m_Alloc);
        }
    }

    m_Size = size;
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
    if (other.m_Size == 0) [[unlikely]] {
        return;
    }

    AppendWithSource(other.GetBuffer(), other.m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::Append(Iter begin, Iter end)
{
    SizeT size = Distance(begin, end);
    if (size == 0) [[unlikely]] {
        return;
    }

    SizeT newSize = m_Size + size;
    if (m_Layout == 0) {
        if (newSize > SmallBufferSize()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, m_Small.Data(), m_Size);
            memory::CopyRange(buffer + m_Size, begin, end);
            SwitchToLarge(MoveArg(newBuffer));
        }
        else {
            memory::CopyRange(m_Small.Data() + m_Size, begin, end);
        }
    }
    else {
        if (newSize > m_Large.Capacity()) {
            LargeBuffer newBuffer = m_Large.Increment(size, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, m_Large.Data(), m_Size);
            memory::CopyRange(buffer + m_Size, begin, end);
            m_Large.Assign(MoveArg(newBuffer), m_Alloc);
        }
        else {
            memory::CopyRange(m_Large.Data() + m_Size, begin, end);
        }
    }

    m_Size = newSize;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, Value ch)
{
    if (index == m_Size) {
        Push(ch);
        return;
    }

    AssertValidIndex(index);
    Ptr buffer = InsertWithSize(index, 1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, Value ch, SizeT count)
{
    if (index == m_Size) {
        Push(ch, count);
        return;
    }

    if (count == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    Ptr buffer = InsertWithSize(index, count);
    memory::StringFill(buffer, ch, count);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(SizeT index, ConstPtr ptr)
{
    if (index == m_Size) {
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
    if (index == m_Size) {
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
    if (index == m_Size) {
        Append(other);
        return;
    }

    if (other.m_Size == 0) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    InsertWithSource(index, other.GetBuffer(), other.m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == m_Size) {
        Push(ch);
        return;
    }

    AssertValidIterator(iter);
    Ptr buffer = InsertWithSize(index, 1);
    *buffer = ch;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Insert(ConstIterator iter, Value ch, SizeT count)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == m_Size) {
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
    if (index == m_Size) {
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
    if (index == m_Size) {
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
    if (index == m_Size) {
        Append(other);
        return;
    }

    if (other.m_Size == 0) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    InsertWithSource(index, other.GetBuffer(), other.m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertRange(SizeT index, Iter begin, Iter end)
{
    if (index == m_Size) {
        Append(begin, end);
        return;
    }

    AssertValidIndex(index);
    InsertWithRange(index, begin, end);
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertRange(ConstIterator iter, Iter begin, Iter end)
{
    SizeT index = static_cast<SizeT>(iter.Address() - GetBuffer());
    if (index == m_Size) {
        Append(begin, end);
        return;
    }

    AssertValidIterator(iter);
    InsertWithRange(index, begin, end);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast()
{
    if (m_Size == 0) [[unlikely]] {
        return;
    }

    --m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveLast(SizeT count)
{
    if (m_Size == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = m_Size < count ? m_Size : count;
    m_Size -= removeCnt;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(SizeT index)
{
    if (index == m_Size) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    Ptr loc = GetBuffer() + index;
    memory::StringMove(loc, loc + 1, m_Size - index - 1);
    --m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveAt(ConstIterator iter)
{
    Ptr buffer = GetBuffer();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == m_Size) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    Ptr loc = buffer + static_cast<OffsetT>(index);
    memory::StringMove(loc, loc + 1, m_Size - index - 1);
    --m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(SizeT index, SizeT count)
{
    if (index == m_Size) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    if (index + count >= m_Size) [[unlikely]] {
        m_Size = index;
        return;
    }

    Ptr buffer = GetBuffer();
    memory::StringMove(buffer + static_cast<OffsetT>(index),
                       buffer + static_cast<OffsetT>(index + count), m_Size - index - count);
    m_Size -= count;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator iter, SizeT count)
{
    Ptr buffer = GetBuffer();
    SizeT index = static_cast<SizeT>(iter.Address() - buffer);
    if (index == m_Size) [[unlikely]] {
        return;
    }

    AssertValidIterator(iter);
    if (index + count >= m_Size) [[unlikely]] {
        m_Size = index;
        return;
    }

    memory::StringMove(buffer + static_cast<OffsetT>(index),
                       buffer + static_cast<OffsetT>(index + count), m_Size - index - count);
    m_Size -= count;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::RemoveRange(ConstIterator begin, ConstIterator end)
{
    AssertValidIterator(begin);
    AssertValidIterator(end - 1);

    Ptr buffer = GetBuffer();
    SizeT count = static_cast<SizeT>(end - begin);
    SizeT index = static_cast<SizeT>(begin.Address() - buffer);
    if (index + count >= m_Size) [[unlikely]] {
        m_Size = index;
        return;
    }

    Ptr loc = buffer + static_cast<OffsetT>(index);
    memory::StringMove(loc, end.Address(), m_Size - index - count);
    m_Size -= count;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Resize(SizeT size, Value ch)
{
    if (m_Size == size) [[unlikely]] {
        return;
    }

    if (m_Size < size) {
        if (Capacity() > size) {
            memory::StringFill(GetBuffer() + m_Size, ch, size - m_Size);
        }
        else if (m_Layout == 0) {
            LargeBuffer buffer(size + 1, m_Alloc);
            Ptr newBegin = buffer.Data();
            memory::StringCopy(newBegin, m_Small.Data(), m_Size);
            memory::StringFill(newBegin + m_Size, ch, size - m_Size);
            SwitchToLarge(MoveArg(buffer));
        }
        else {
            LargeBuffer buffer = m_Large.Resize(size + 1, m_Alloc);
            Ptr newBegin = buffer.Data();
            memory::StringCopy(newBegin, m_Large.Data(), m_Size);
            memory::StringFill(newBegin + m_Size, ch, size - m_Size);
            m_Large.Assign(MoveArg(buffer), m_Alloc);
        }
    }

    m_Size = size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Reserve(SizeT size)
{
    if (Capacity() > size) [[unlikely]] {
        return;
    }

    if (m_Layout == 0) {
        LargeBuffer buffer(size + 1, m_Alloc);
        memory::StringCopy(buffer.Data(), m_Small.Data(), m_Size);
        SwitchToLarge(MoveArg(buffer));
    }
    else {
        LargeBuffer buffer = m_Large.Resize(size + 1, m_Alloc);
        memory::StringCopy(buffer.Data(), m_Large.Data(), m_Size);
        m_Large.Assign(MoveArg(buffer), m_Alloc);
    }
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Shrink()
{
    if (m_Layout == 0) [[unlikely]] {
        return;
    }

    LargeBuffer newBuffer = m_Large.Resize(m_Size + 1, m_Alloc);
    memory::StringCopy(newBuffer.Data(), m_Large.Data(), m_Size);
    m_Large.Assign(MoveArg(newBuffer), m_Alloc);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Clear()
{
    m_Size = 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::Swap(BasicString& other) noexcept
{
    if (m_Layout == other.m_Layout) {
        if (m_Layout == 0) {
            constexpr SizeT bufferCap = SmallBufferSize();
            T tmpBuffer[bufferCap];
            Ptr buffer = m_Small.Data();
            Ptr otherBuffer = other.m_Small.Data();
            memory::StringCopy(tmpBuffer, buffer, m_Size);
            memory::StringCopy(buffer, otherBuffer, other.m_Size);
            memory::StringCopy(otherBuffer, tmpBuffer, m_Size);
        }
        else {
            m_Large.Swap(other.m_Large);
        }
    }
    else {
        if (m_Layout == 0) {
            LargeBuffer buffer = other.SwitchToSmall(m_Small.Data(), m_Size);
            SwitchToLarge(MoveArg(buffer));
        }
        else {
            LargeBuffer buffer = SwitchToSmall(other.m_Small.Data(), other.m_Size);
            other.SwitchToLarge(MoveArg(buffer));
        }
    }

    mini::Swap(m_Size, other.m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::Data() noexcept
{
    Ptr buffer = GetBuffer();
    if (buffer[m_Size] != T(0)) {
        buffer[m_Size] = T(0);
    }

    return buffer;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstPtr BasicString<T, AllocT>::Data() const noexcept
{
    Ptr buffer = GetBuffer();
    if (buffer[m_Size] != T(0)) {
        buffer[m_Size] = T(0);
    }

    return static_cast<ConstPtr>(buffer);
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
    return Iterator(GetBuffer() + m_Size, GetVersion(), this);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstIterator BasicString<T, AllocT>::End() const noexcept
{
    return ConstIterator(GetBuffer() + m_Size, GetVersion(), this);
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
    AssertValidIndex(m_Size - 1);
    return *(GetBuffer() + m_Size - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::ConstRef BasicString<T, AllocT>::Last() const
{
    AssertValidIndex(m_Size - 1);
    return *(GetBuffer() + m_Size - 1);
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
inline constexpr SizeT BasicString<T, AllocT>::Size() const noexcept
{
    return m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::Capacity() const noexcept
{
    return m_Layout == 0 ? SmallBufferSize() : (m_Large.Capacity() - 1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsEmpty() const noexcept
{
    return m_Size == 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIndex(SizeT index) const noexcept
{
    return index < m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsValidIterator(ConstIterator iter) const noexcept
{
    if (iter.m_Version != GetVersion()) {
        return false;
    }

    OffsetT dist = iter.Address() - GetBuffer();
    return dist >= 0 && dist < static_cast<OffsetT>(m_Size);
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
    AssignWithSource(other.GetBuffer(), other.m_Size);
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT>& BasicString<T, AllocT>::operator=(BasicString&& other) noexcept
{
    AssignWithMove(MoveArg(other));
    return *this;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicString<T, AllocT>::GetVersion() const noexcept
{
    switch (m_Layout) {
        case 0:  return 0;
        case 1:  return m_Large.Version();
        default: break;
    }

    return static_cast<SizeT>(-1);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::GetBuffer() const noexcept
{
    // TODO: to get rid of const_cast, we should first modify array inside Data() const
    return const_cast<Ptr>(m_Layout == 0 ? m_Small.Data() : m_Large.Data());
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::DestroyBuffer()
{
    // inplace buffer does not need to be destroyed
    if (m_Layout == 0) {
        return;
    }

    m_Large.Deallocate(m_Alloc);
    memory::DestructAt(&m_Large);
    m_Layout = 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::SwitchToLarge(LargeBuffer&& buffer)
{
    memory::DestructAt(&m_Small);
    memory::ConstructAt(&m_Large, MoveArg(buffer));
    m_Layout = 1;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::LargeBuffer
BasicString<T, AllocT>::SwitchToSmall(Ptr ptr, SizeT size)
{
    LargeBuffer tmpBuffer(MoveArg(m_Large));
    memory::DestructAt(&m_Large);
    memory::ConstructAt(&m_Small);
    memory::StringCopy(m_Small.Data(), ptr, size);
    m_Layout = 0;
    return tmpBuffer;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr bool BasicString<T, AllocT>::IsOverlapping(ConstPtr ptr) const noexcept
{
    // TODO: this is how clang manages comparison between pointers to unrelated objects
    if (IsConstantEvaluated()) {
        return false;
    }

    ConstPtr buffer = GetBuffer();
    return (buffer <= ptr) && (buffer + m_Size > ptr);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithCopy(BasicString const& other)
{
    ConstPtr from = nullptr;
    Ptr to = nullptr;

    if (other.m_Layout == 0) {
        memory::ConstructAt(&m_Small);
        to = m_Small.Data();
        from = other.m_Small.Data();
    }
    else {
        memory::ConstructAt(&m_Large);
        m_Large.Allocate(other.m_Size + 1, m_Alloc);
        to = m_Large.Data();
        from = other.m_Large.Data();
        m_Layout = 1;
    }

    memory::StringCopy(to, from, other.m_Size);
    m_Size = other.m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InitWithMove(BasicString&& other) noexcept
{
    if (other.m_Layout == 0) {
        memory::ConstructAt(&m_Small);
        memory::StringCopy(m_Small.Data(), other.m_Small.Data(), other.m_Size);
    }
    else {
        memory::ConstructAt(&m_Large, MoveArg(other.m_Large));
        memory::DestructAt(&other.m_Large);
        memory::ConstructAt(&other.m_Small);
        m_Layout = 1;
        other.m_Layout = 0;
    }

    m_Size = Exchange(other.m_Size, SizeT(0));
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithMove(BasicString&& other) noexcept
{
    Ptr buffer = GetBuffer();
    Ptr otherBuffer = other.GetBuffer();

    if (other.m_Layout == 0) {
        memory::StringCopy(buffer, otherBuffer, other.m_Size);
    }
    else {
        if (m_Layout == 0) {
            SwitchToLarge(MoveArg(other.m_Large));
        }
        else {
            m_Large.Assign(MoveArg(other.m_Large), m_Alloc);
        }

        memory::DestructAt(&other.m_Large);
        memory::ConstructAt(&other.m_Small);
        other.m_Layout = 0;
    }

    m_Size = other.m_Size;
    other.m_Size = 0;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AssignWithSource(ConstPtr ptr, SizeT len)
{
    if (len <= SmallBufferSize()) {
        memory::StringCopy(GetBuffer(), ptr, len);
    }
    else {
        if (m_Layout == 0) {
            LargeBuffer buffer(len + 1, m_Alloc);
            memory::StringCopy(buffer.Data(), ptr, len);
            SwitchToLarge(MoveArg(buffer));
        }
        else {
            LargeBuffer buffer = m_Large.Resize(len + 1, m_Alloc);
            memory::StringCopy(buffer.Data(), ptr, len);
            m_Large.Assign(MoveArg(buffer), m_Alloc);
        }
    }

    m_Size = len;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::AppendWithSource(ConstPtr ptr, SizeT len)
{
    SizeT newSize = m_Size + len;

    if (m_Layout == 0) {
        if (newSize > SmallBufferSize()) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, m_Small.Data(), m_Size);
            memory::StringCopy(buffer + m_Size, ptr, len);
            SwitchToLarge(MoveArg(newBuffer));
        }
        else {
            memory::StringCopy(m_Small.Data() + m_Size, ptr, len);
        }
    }
    else {
        if (newSize > m_Large.Capacity()) {
            LargeBuffer newBuffer = m_Large.Increment(len, m_Alloc);
            Ptr buffer = newBuffer.Data();
            memory::StringCopy(buffer, m_Large.Data(), m_Size);
            memory::StringCopy(buffer + m_Size, ptr, len);
            m_Large.Assign(MoveArg(newBuffer), m_Alloc);
        }
        else {
            memory::StringCopy(m_Large.Data() + m_Size, ptr, len);
        }
    }

    m_Size = newSize;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr void BasicString<T, AllocT>::InsertWithSource(SizeT index, ConstPtr ptr, SizeT len)
{
    SizeT newSize = m_Size + len;

    if (m_Layout == 0) {
        if (newSize > SmallBufferSize()) {
            LargeBuffer buffer(newSize + 1, m_Alloc);
            Ptr newBuffer = buffer.Data();
            Ptr oldBuffer = m_Small.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + len),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index), ptr, len);
            SwitchToLarge(MoveArg(buffer));
        }
        else if (IsOverlapping(ptr)) {
            Ptr loc = m_Small.Data() + static_cast<OffsetT>(index);
            SmallBuffer temp;

            memory::StringCopy(temp.Data(), ptr, len);
            memory::StringMove(loc + static_cast<OffsetT>(len), loc, m_Size - index);
            memory::StringCopy(loc, temp.Data(), len);
        }
        else {
            Ptr loc = m_Small.Data() + static_cast<OffsetT>(index);
            memory::StringMove(loc + static_cast<OffsetT>(len), loc, m_Size - index);
            memory::StringCopy(loc, ptr, len);
        }
    }
    else {
        if (newSize > m_Large.Capacity()) {
            LargeBuffer buffer = m_Large.Increment(len, m_Alloc);
            Ptr newBuffer = buffer.Data();
            Ptr oldBuffer = m_Large.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + len),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index), ptr, len);
            m_Large.Assign(MoveArg(buffer), m_Alloc);
        }
        else if (IsOverlapping(ptr)) {
            Ptr loc = m_Large.Data() + static_cast<OffsetT>(index);
            BasicString temp(ptr, len, m_Alloc);

            memory::StringCopy(temp.GetBuffer(), ptr, len);
            memory::StringMove(loc + static_cast<OffsetT>(len), loc, m_Size - index);
            memory::StringCopy(loc, temp.GetBuffer(), len);
        }
        else {
            Ptr loc = m_Large.Data() + static_cast<OffsetT>(index);
            memory::StringMove(loc + static_cast<OffsetT>(len), loc, m_Size - index);
            memory::StringCopy(loc, ptr, len);
        }
    }

    m_Size = newSize;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InitWithSize(SizeT size)
{
    Ptr buffer = nullptr;

    if (size <= SmallBufferSize()) {
        memory::ConstructAt(&m_Small);
        buffer = m_Small.Data();
    }
    else {
        memory::ConstructAt(&m_Large);
        m_Large.Allocate(size + 1, m_Alloc);
        buffer = m_Large.Data();
        m_Layout = 1;
    }

    m_Size = size;
    return buffer;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::AppendWithSize(SizeT size)
{
    SizeT oldSize = m_Size;
    SizeT newSize = oldSize + size;
    SizeT capacity = Capacity();

    if (capacity < newSize) {
        if (m_Layout == 0) {
            LargeBuffer newBuffer(newSize + 1, m_Alloc);
            memory::StringCopy(newBuffer.Data(), m_Small.Data(), m_Size);
            SwitchToLarge(MoveArg(newBuffer));
        }
        else {
            LargeBuffer newBuffer = m_Large.Increment(size, m_Alloc);
            memory::StringCopy(newBuffer.Data(), m_Large.Data(), m_Size);
            m_Large.Assign(MoveArg(newBuffer), m_Alloc);
        }

        m_Size += size;
        return m_Large.Data() + oldSize;
    }

    m_Size += size;
    return GetBuffer() + oldSize;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::Ptr BasicString<T, AllocT>::InsertWithSize(SizeT index,
                                                                                    SizeT size)
{
    SizeT newSize = m_Size + size;
    SizeT capacity = Capacity();

    if (capacity < newSize) {
        if (m_Layout == 0) {
            LargeBuffer buffer(newSize + 1, m_Alloc);
            Ptr oldBuffer = m_Small.Data();
            Ptr newBuffer = buffer.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + size),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            SwitchToLarge(MoveArg(buffer));
        }
        else {
            LargeBuffer buffer = m_Large.Increment(size, m_Alloc);
            Ptr oldBuffer = m_Large.Data();
            Ptr newBuffer = buffer.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + size),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            m_Large.Assign(MoveArg(buffer), m_Alloc);
        }

        m_Size += size;
        return m_Large.Data() + static_cast<OffsetT>(index);
    }
    else {
        Ptr loc = GetBuffer() + static_cast<OffsetT>(index);
        memory::StringMove(loc + size, loc, m_Size - index);
        m_Size += size;
        return loc;
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void BasicString<T, AllocT>::InsertWithRange(SizeT index, Iter begin, Iter end)
{
    SizeT size = Distance(begin, end);
    SizeT newSize = m_Size + size;
    if (size == 0) [[unlikely]] {
        return;
    }

    if (m_Layout == 0) {
        if (newSize > SmallBufferSize()) {
            LargeBuffer buffer(newSize + 1, m_Alloc);
            Ptr newBuffer = buffer.Data();
            Ptr oldBuffer = m_Small.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + size),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            memory::CopyRange(newBuffer + static_cast<OffsetT>(index), begin, end);
            SwitchToLarge(MoveArg(buffer));
        }
        else {
            Ptr loc = m_Small.Data() + static_cast<OffsetT>(index);
            SmallBuffer temp;

            memory::CopyRange(temp.Data(), begin, end);
            memory::StringMove(loc + static_cast<OffsetT>(size), loc, m_Size - index);
            memory::StringCopy(loc, temp.Data(), size);
        }
    }
    else {
        if (newSize > m_Large.Capacity()) {
            LargeBuffer buffer = m_Large.Increment(size, m_Alloc);
            Ptr newBuffer = buffer.Data();
            Ptr oldBuffer = m_Large.Data();

            memory::StringCopy(newBuffer, oldBuffer, index);
            memory::StringCopy(newBuffer + static_cast<OffsetT>(index + size),
                               oldBuffer + static_cast<OffsetT>(index), m_Size - index);
            memory::CopyRange(newBuffer + static_cast<OffsetT>(index), begin, end);
            m_Large.Assign(MoveArg(buffer), m_Alloc);
        }
        else {
            BasicString temp(begin, end, m_Alloc);
            Ptr buffer = m_Large.Data();
            Ptr loc = buffer + static_cast<OffsetT>(index);

            memory::CopyRange(temp.GetBuffer(), begin, end);
            memory::StringMove(loc + static_cast<OffsetT>(size), loc, m_Size - index);
            memory::StringCopy(loc, temp.GetBuffer(), temp.m_Size);
        }
    }

    m_Size = newSize;
}

template <CharT T, AllocatorT<T> AllocT>
template <ConvertibleToT<T> U, typename TraitsU, typename AllocU>
inline constexpr BasicString<T, AllocT>::
    BasicString(std::basic_string<U, TraitsU, AllocU> const& other, AllocT const& alloc)
    : m_Alloc(alloc)
    , m_Layout(0)
    , m_Size(0)
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
    return std::basic_string<T, TraitsT, StdAllocT>(GetBuffer(), m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT>::operator std::basic_string<T>() const
{
    return std::basic_string<T>(GetBuffer(), m_Size);
}

template <CharT T, AllocatorT<T> AllocT>
consteval SizeT BasicString<T, AllocT>::SmallBufferSize() noexcept
{
    return (sizeof(LargeBuffer) / sizeof(Value)) - 1;
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
    ASSERT(dist >= 0 && dist < static_cast<OffsetT>(m_Size), "invalid range");
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