export module mini.core:array;

import :type;
import :initializer_list;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :allocator;
import :dynamic_buffer;
import :array_iterator;

namespace mini {

export template <MovableT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class Array {
private:
    typedef memory::DynamicBuffer<T, AllocT> Buffer;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;
    using Iterator = ArrayIterator<Value, Array>;
    using ConstIterator = ArrayIterator<ConstValue, Array const>;

private:
    SizeT m_Size;
    Buffer m_Buffer;

public:
    constexpr Array() noexcept;
    constexpr ~Array();
    constexpr Array(Array const&)
        requires CopyableT<T>;
    constexpr Array(Array const&, AllocT const&)
        requires CopyableT<T>;
    constexpr Array(Array&&) noexcept;
    constexpr Array(Array&&, AllocT const&) noexcept;
    constexpr Array(AllocT const&) noexcept;
    constexpr Array(AllocT&&) noexcept;
    constexpr Array(InitializerList<T>, AllocT const& = AllocT());
    explicit constexpr Array(SizeT, AllocT const& = AllocT());
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr Array(Iter, Iter, AllocT const& = AllocT())
        requires CopyableT<T>;

    template <typename... Args>
    constexpr void Push(Args&&...)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(SizeT, Args&&...)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(ConstIterator, Args&&...)
        requires ConstructibleFromT<T, Args...>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    constexpr void Assign(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter, Iter);
    constexpr void Append(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(SizeT, Iter, Iter);
    constexpr void InsertRange(SizeT, InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator, Iter, Iter);
    constexpr void InsertRange(ConstIterator, InitializerList<T>);

    constexpr void RemoveLast();
    constexpr void RemoveLast(SizeT);
    constexpr void RemoveAt(SizeT);
    constexpr void RemoveAt(ConstIterator);
    constexpr void RemoveRange(SizeT, SizeT);
    constexpr void RemoveRange(ConstIterator, ConstIterator);

    template <typename... Args>
    constexpr void Resize(SizeT, Args&&...)
        requires ConstructibleFromT<T, Args...>;
    constexpr void Reserve(SizeT);
    constexpr void Shrink();
    constexpr void Clear();
    constexpr void Swap(Array&) noexcept;

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

    constexpr Array& operator=(Array const&)
        requires CopyableT<T>;
    constexpr Array& operator=(Array&&) noexcept;
    constexpr Array& operator=(InitializerList<T>);

private:
    constexpr void SwapNewBuffer(Buffer&);
    template <typename U>
    constexpr void AssignRangeWithSize(U, U, SizeT);
    template <typename U>
    constexpr void AppendRangeWithSize(U, U, SizeT);
    template <typename U>
    constexpr void InsertRangeWithSize(SizeT, U, U, SizeT);

    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array() noexcept
    : m_Size(0)
    , m_Buffer()
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::~Array()
{
    Clear();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array const& other)
    requires CopyableT<T>
    : m_Size(0)
    , m_Buffer(other.m_Buffer.GetAllocator())
{
    m_Buffer.Allocate(other.Size());
    memory::ConstructRange(m_Buffer.Data(), other.Begin(), other.End());
    m_Size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array const& other, AllocT const& alloc)
    requires CopyableT<T>
    : m_Size(0)
    , m_Buffer(alloc)
{
    m_Buffer.Allocate(other.Size());
    memory::ConstructRange(m_Buffer.Data(), other.Begin(), other.End());
    m_Size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array&& other) noexcept
    : m_Size(Exchange(other.m_Size, SizeT(0)))
    , m_Buffer(Exchange(other.m_Buffer, {}))
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array&& other, AllocT const& alloc) noexcept
    : m_Size(Exchange(other.m_Size, SizeT(0)))
    , m_Buffer(MoveArg(other.m_Buffer), alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(AllocT const& alloc) noexcept
    : m_Size(0)
    , m_Buffer(alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(AllocT&& alloc) noexcept
    : m_Size(0)
    , m_Buffer(MoveArg(alloc))
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(InitializerList<T> init, AllocT const& alloc)
    : m_Size(0)
    , m_Buffer(alloc)
{
    m_Buffer.Allocate(init.size());
    memory::ConstructRange(m_Buffer.Data(), init.begin(), init.end());
    m_Size = init.size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(SizeT capacity, AllocT const& alloc)
    : m_Size(0)
    , m_Buffer(alloc)
{
    m_Buffer.Allocate(capacity);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr Array<T, AllocT>::Array(Iter first, Iter last, AllocT const& alloc)
    requires CopyableT<T>
    : m_Size(0)
    , m_Buffer(alloc)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        return;
    }

    m_Buffer.Allocate(distance);
    memory::ConstructRange(m_Buffer.Data(), first, last);
    m_Size = distance;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Push(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    SizeT capacity = m_Buffer.Capacity();

    if (m_Size < capacity) {
        memory::ConstructAt(m_Buffer.Data() + m_Size, ForwardArg<Args>(args)...);
    } else {
        Buffer newBuf = m_Buffer.Increment(1);
        Ptr newBegin = newBuf.Data();
        Ptr begin = m_Buffer.Data();

        memory::ConstructAt(newBegin + m_Size, ForwardArg<Args>(args)...);
        memory::MoveConstructBackward(newBegin + m_Size, begin, begin + m_Size);
        SwapNewBuffer(newBuf);
    }

    ++m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
inline constexpr void Array<T, AllocT>::Insert(SizeT index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + (OffsetT)index, ForwardArg<Args>(args)...);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    OffsetT locDiff = iter - Begin();
    if (locDiff == (OffsetT)m_Size) {
        Push(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidIterator(iter);
    SizeT capacity = m_Buffer.Capacity();

    if (m_Size < capacity) {
        // without the copy, invalid reference can get copied
        Value temp(ForwardArg<Args>(args)...);
        Ptr begin = m_Buffer.Data();
        Ptr loc = begin + locDiff;
        Ptr end = begin + m_Size;
        Ptr last = end - 1;

        memory::ConstructAt(end, MoveArg(*last));
        memory::MoveBackward(end, loc, last);
        memory::DestructAt(loc);
        memory::ConstructAt(loc, MoveArg(temp));
    } else {
        Buffer newBuf = m_Buffer.Increment(1);
        Ptr newBegin = newBuf.Data();
        Ptr newLoc = newBegin + locDiff;
        Ptr begin = m_Buffer.Data();
        Ptr loc = begin + locDiff;

        memory::ConstructAt(newLoc, ForwardArg<Args>(args)...);
        memory::MoveConstructRange(newBegin, begin, loc);
        memory::MoveConstructRange(newLoc + 1, loc, begin + m_Size);
        SwapNewBuffer(newBuf);
    }

    ++m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void Array<T, AllocT>::Assign(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(first, last, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Assign(InitializerList<T> init)
{
    SizeT len = init.size();
    if (len == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(init.begin(), init.end(), len);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void Array<T, AllocT>::Append(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Push(ForwardArg<typename Iter::Value>(*first)); return;
        default: break;
    }

    AppendRangeWithSize(first, last, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Append(InitializerList<T> init)
{
    AppendRangeWithSize(init.begin(), init.end(), init.size());
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void Array<T, AllocT>::InsertRange(SizeT index, Iter first, Iter last)
{
    if (index == m_Size) {
        Append(first, last);
        return;
    }

    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(index, ForwardArg<typename Iter::Value>(*first)); return;
        default: break;
    }

    AssertValidIndex(index);
    InsertRangeWithSize(index, first, last, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::InsertRange(SizeT index, InitializerList<T> init)
{
    if (index == m_Size) {
        Append(init);
        return;
    }

    InsertRangeWithSize(index, init.begin(), init.end(), init.size());
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr void Array<T, AllocT>::InsertRange(ConstIterator iter, Iter first, Iter last)
{
    SizeT locDiff = static_cast<SizeT>(iter - Begin());
    if (locDiff == m_Size) {
        Append(first, last);
        return;
    }

    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(iter, ForwardArg<typename Iter::Value>(*first)); return;
        default: break;
    }

    AssertValidIterator(iter);
    InsertRangeWithSize(locDiff, first, last, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::InsertRange(ConstIterator iter, InitializerList<T> init)
{
    SizeT locDiff = static_cast<SizeT>(iter - Begin());
    if (locDiff == m_Size) {
        Append(init);
        return;
    }

    AssertValidIterator(iter);
    InsertRangeWithSize(locDiff, init.begin(), init.end(), init.size());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveLast()
{
    if (IsEmpty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_Buffer.Data() + m_Size - 1);
    --m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveLast(SizeT count)
{
    if (IsEmpty() || count == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = m_Size < count ? m_Size : count;
    Ptr end = m_Buffer.Data() + m_Size;
    memory::DestructRange(end - removeCnt, end);
    m_Size -= removeCnt;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveAt(SizeT index)
{
    RemoveAt(Begin() + (OffsetT)index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::RemoveAt(ConstIterator iter)
{
    OffsetT locDiff = iter - Begin();
    if (locDiff == OffsetT(m_Size) - 1) [[unlikely]] {
        RemoveLast();
        return;
    }

    AssertValidIterator(iter);
    Ptr begin = m_Buffer.Data();
    Ptr loc = begin + locDiff;
    Ptr end = begin + m_Size;

    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveRange(SizeT first, SizeT last)
{
    Iterator begin = Begin();
    RemoveRange(begin + (OffsetT)first, begin + (OffsetT)last);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::RemoveRange(ConstIterator first, ConstIterator last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  RemoveAt(first); return;
        default: break;
    }

    AssertValidRange(first, last);
    Iterator iterBegin = Begin();
    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;
    Ptr loc = begin + (first - iterBegin);

    memory::MoveRange(loc, loc + distance, end);
    memory::DestructRange(end - distance, end);
    m_Size -= distance;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Resize(SizeT size, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_Size == size) [[unlikely]] {
        return;
    }

    Ptr begin(m_Buffer.Data());

    if (m_Size < size) {
        Value temp(ForwardArg<Args>(args)...);

        if (m_Buffer.Capacity() < size) {
            Buffer newBuf = m_Buffer.Resize(size);
            Ptr newBegin(newBuf.Data());

            memory::ConstructRangeArgs(newBegin + m_Size, newBegin + size, temp);
            memory::MoveConstructRange(newBegin, begin, begin + m_Size);
            SwapNewBuffer(newBuf);
        } else {
            memory::ConstructRangeArgs(begin + m_Size, begin + size, temp);
        }
    } else {
        memory::DestructRange(begin + size, begin + m_Size);
    }

    m_Size = size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Reserve(SizeT size)
{
    if (m_Buffer.Capacity() > size) [[unlikely]] {
        return;
    }

    Buffer newBuf = m_Buffer.Resize(size);
    Ptr newBegin(newBuf.Data());
    Ptr oldBegin(m_Buffer.Data());

    memory::MoveConstructRange(newBegin, oldBegin, oldBegin + m_Size);
    SwapNewBuffer(newBuf);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Shrink()
{
    if (m_Buffer.Capacity() == m_Size) [[unlikely]] {
        return;
    }

    Buffer newBuf = m_Buffer.Resize(m_Size);
    Ptr newBegin(newBuf.Data());
    Ptr oldBegin(m_Buffer.Data());

    memory::MoveConstructRange(newBegin, oldBegin, oldBegin + m_Size);
    SwapNewBuffer(newBuf);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Clear()
{
    if (m_Size == 0) [[unlikely]] {
        return;
    }

    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;

    memory::DestructRange(begin, end);
    m_Size = 0;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Swap(Array& other) noexcept
{
    m_Buffer.Swap(other.m_Buffer);
    mini::Swap(m_Size, other.m_Size);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Ptr Array<T, AllocT>::Data() noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstPtr Array<T, AllocT>::Data() const noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::Begin() noexcept
{
    return Iterator(m_Buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::Begin() const noexcept
{
    return ConstIterator(m_Buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::End() noexcept
{
    return Iterator(m_Buffer.Data() + m_Size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::End() const noexcept
{
    return ConstIterator(m_Buffer.Data() + m_Size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *(m_Buffer.Data());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *(m_Buffer.Data());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::Last()
{
    AssertValidIndex(m_Size - 1);
    return *(m_Buffer.Data() + m_Size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::Last() const
{
    AssertValidIndex(m_Size - 1);
    return *(m_Buffer.Data() + m_Size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr SizeT Array<T, AllocT>::Size() const noexcept
{
    return m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr SizeT Array<T, AllocT>::Capacity() const noexcept
{
    return m_Buffer.Capacity();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::IsEmpty() const noexcept
{
    return m_Size == 0;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::IsValidIndex(SizeT index) const noexcept
{
    return index < m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::IsValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.m_Ptr - m_Buffer.Data());
    return index < m_Size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::IsValidRange(ConstIterator begin,
                                                     ConstIterator end) const noexcept
{
    ConstPtr buffer = m_Buffer.Data();
    SizeT beginIdx = static_cast<SizeT>(begin.m_Ptr - buffer);
    SizeT endIdx = static_cast<SizeT>(end.m_Ptr - buffer);
    return (beginIdx < m_Size) && (endIdx < m_Size + 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array const& other)
    requires CopyableT<T>
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array&& other) noexcept
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    SwapNewBuffer(other.m_Buffer);
    m_Size = Exchange(other.m_Size, SizeT(0));
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(InitializerList<T> init)
{
    Assign(init);
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::SwapNewBuffer(Buffer& buf)
{
    Ptr begin(m_Buffer.Data());
    m_Buffer.Swap(buf);
    memory::DestructRange(begin, begin + m_Size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::AssignRangeWithSize(U first, U last, SizeT len)
{
    OffsetT size = (OffsetT)m_Size;
    SizeT capacity = m_Buffer.Capacity();
    Ptr begin = m_Buffer.Data();

    if (capacity > len) {
        if (m_Size < len) {
            memory::CopyRange(begin, first, first + size);
            memory::ConstructRange(begin + m_Size, first + size, last);
        } else {
            memory::CopyRange(begin, first, last);
            memory::DestructRange(begin + len, begin + size);
        }
    } else {
        Buffer newBuf = m_Buffer.Resize(len);
        memory::ConstructRange(newBuf.Data(), first, last);
        SwapNewBuffer(newBuf);
    }

    m_Size = len;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::AppendRangeWithSize(U first, U last, SizeT len)
{
    SizeT capacity = m_Buffer.Capacity();
    SizeT newSize = m_Size + len;
    if (newSize <= capacity) {
        memory::ConstructRange(m_Buffer.Data() + m_Size, first, last);
    } else {
        Buffer newBuf = m_Buffer.Increment(newSize - capacity);
        Ptr newBegin = newBuf.Data();
        Ptr begin = m_Buffer.Data();

        memory::ConstructBackward(newBegin + newSize, first, last);
        memory::MoveConstructBackward(newBegin + m_Size, begin, begin + m_Size);
        SwapNewBuffer(newBuf);
    }

    m_Size = newSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::InsertRangeWithSize(SizeT index, U first, U last, SizeT len)
{
    SizeT capacity = m_Buffer.Capacity();
    SizeT newSize = m_Size + len;

    if (newSize <= capacity) {
        Ptr begin = m_Buffer.Data();
        Ptr loc = begin + index;
        Ptr end = begin + m_Size;

        if (static_cast<SizeT>(end - loc) > len) {
            Ptr middle = begin + m_Size - len;
            memory::MoveConstructBackward(end + len, middle, end);
            memory::MoveBackward(end, loc, middle);
            memory::DestructRange(loc, loc + len);
        } else {
            memory::MoveConstructBackward(end + len, loc, end);
            memory::DestructRange(loc, end);
        }

        m_Size = newSize;
        memory::ConstructRange(loc, first, last);
    } else {
        Buffer newBuf = m_Buffer.Increment(len);
        Ptr newBegin = newBuf.Data();
        Ptr begin = m_Buffer.Data();

        memory::ConstructRange(newBegin + index, first, last);
        memory::MoveConstructRange(newBegin, begin, begin + index);
        memory::MoveConstructBackward(newBegin + newSize, begin + index, begin + m_Size);
        SwapNewBuffer(newBuf);
        m_Size = newSize;
    }
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(IsValidIndex(index), "invalid index");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(IsValidIterator(iter), "invalid iterator");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                   [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(IsValidRange(begin, end), "invalid range");
}

export template <MovableT T, AllocatorT<T> AllocT, MovableT U, AllocatorT<U> AllocU>
inline constexpr bool operator==(Array<T, AllocT> const& l, Array<U, AllocU> const& r)
    requires EqualityComparableWithT<T, U>
{
    if (l.Size() != r.Size()) {
        return false;
    } else if (l.Data() == r.Data()) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(l.Begin(), l.End(), r.Begin(), r.End());
}

export template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Swap(Array<T, AllocT>& l, Array<T, AllocT>& r) noexcept
{
    return l.Swap(r);
}

} // namespace mini