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
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    using Iterator = ArrayIterator<Value, Array>;
    using ConstIterator = ArrayIterator<ConstValue, Array const>;

private:
    SizeT m_size;
    Buffer m_buffer;

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
    : m_size(0)
    , m_buffer()
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
    : m_size(0)
    , m_buffer(other.m_buffer.GetAllocator())
{
    m_buffer.Allocate(other.Size());
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array const& other, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.Allocate(other.Size());
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array&& other) noexcept
    : m_size(Exchange(other.m_size, SizeT(0)))
    , m_buffer(Exchange(other.m_buffer, {}))
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array&& other, AllocT const& alloc) noexcept
    : m_size(Exchange(other.m_size, SizeT(0)))
    , m_buffer(MoveArg(other.m_buffer), alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(AllocT const& alloc) noexcept
    : m_size(0)
    , m_buffer(alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(AllocT&& alloc) noexcept
    : m_size(0)
    , m_buffer(MoveArg(alloc))
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(InitializerList<T> init, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.Allocate(init.size());
    memory::ConstructRange(m_buffer.Data(), init.begin(), init.end());
    m_size = init.size();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(SizeT capacity, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.Allocate(capacity);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
inline constexpr Array<T, AllocT>::Array(Iter first, Iter last, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        return;
    }

    m_buffer.Allocate(distance);
    memory::ConstructRange(m_buffer.Data(), first, last);
    m_size = distance;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Push(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    SizeT capacity = m_buffer.Capacity();

    if (m_size < capacity) {
        memory::ConstructAt(m_buffer.Data() + m_size, ForwardArg<Args>(args)...);
    } else {
        Buffer newBuf = m_buffer.Increment(1);
        Pointer newBegin = newBuf.Data();
        Pointer begin = m_buffer.Data();

        memory::ConstructAt(newBegin + m_size, ForwardArg<Args>(args)...);
        memory::MoveConstructBackward(newBegin + m_size, begin, begin + m_size);
        SwapNewBuffer(newBuf);
    }

    ++m_size;
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
    if (locDiff == (OffsetT)m_size) {
        Push(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidIterator(iter);
    SizeT capacity = m_buffer.Capacity();

    if (m_size < capacity) {
        // without the copy, invalid reference can get copied
        Value temp(ForwardArg<Args>(args)...);
        Pointer begin = m_buffer.Data();
        Pointer loc = begin + locDiff;
        Pointer end = begin + m_size;
        Pointer last = end - 1;

        memory::ConstructAt(end, MoveArg(*last));
        memory::MoveBackward(end, loc, last);
        memory::DestructAt(loc);
        memory::ConstructAt(loc, MoveArg(temp));
    } else {
        Buffer newBuf = m_buffer.Increment(1);
        Pointer newBegin = newBuf.Data();
        Pointer newLoc = newBegin + locDiff;
        Pointer begin = m_buffer.Data();
        Pointer loc = begin + locDiff;

        memory::ConstructAt(newLoc, ForwardArg<Args>(args)...);
        memory::MoveConstructRange(newBegin, begin, loc);
        memory::MoveConstructRange(newLoc + 1, loc, begin + m_size);
        SwapNewBuffer(newBuf);
    }

    ++m_size;
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
    if (index == m_size) {
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
    if (index == m_size) {
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
    if (locDiff == m_size) {
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
    if (locDiff == m_size) {
        Append(init);
        return;
    }

    AssertValidIterator(iter);
    InsertRangeWithSize(locDiff, init.begin(), init.end(), init.size());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveLast()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_size - 1);
    --m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveLast(SizeT count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = m_size < count ? m_size : count;
    Pointer end = m_buffer.Data() + m_size;
    memory::DestructRange(end - removeCnt, end);
    m_size -= removeCnt;
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
    if (locDiff == OffsetT(m_size) - 1) [[unlikely]] {
        RemoveLast();
        return;
    }

    AssertValidIterator(iter);
    Pointer begin = m_buffer.Data();
    Pointer loc = begin + locDiff;
    Pointer end = begin + m_size;

    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_size;
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
    Pointer begin = m_buffer.Data();
    Pointer end = begin + m_size;
    Pointer loc = begin + (first - iterBegin);

    memory::MoveRange(loc, loc + distance, end);
    memory::DestructRange(end - distance, end);
    m_size -= distance;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Resize(SizeT size, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_size == size) [[unlikely]] {
        return;
    }

    Pointer begin(m_buffer.Data());

    if (m_size < size) {
        Value temp(ForwardArg<Args>(args)...);

        if (m_buffer.Capacity() < size) {
            Buffer newBuf = m_buffer.Resize(size);
            Pointer newBegin(newBuf.Data());

            memory::ConstructRangeArgs(newBegin + m_size, newBegin + size, temp);
            memory::MoveConstructRange(newBegin, begin, begin + m_size);
            SwapNewBuffer(newBuf);
        } else {
            memory::ConstructRangeArgs(begin + m_size, begin + size, temp);
        }
    } else {
        memory::DestructRange(begin + size, begin + m_size);
    }

    m_size = size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Reserve(SizeT size)
{
    if (m_buffer.Capacity() > size) [[unlikely]] {
        return;
    }

    Buffer newBuf = m_buffer.Resize(size);
    Pointer newBegin(newBuf.Data());
    Pointer oldBegin(m_buffer.Data());

    memory::MoveConstructRange(newBegin, oldBegin, oldBegin + m_size);
    SwapNewBuffer(newBuf);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Shrink()
{
    if (m_buffer.Capacity() == m_size) [[unlikely]] {
        return;
    }

    Buffer newBuf = m_buffer.Resize(m_size);
    Pointer newBegin(newBuf.Data());
    Pointer oldBegin(m_buffer.Data());

    memory::MoveConstructRange(newBegin, oldBegin, oldBegin + m_size);
    SwapNewBuffer(newBuf);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Clear()
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    Pointer begin = m_buffer.Data();
    Pointer end = begin + m_size;

    memory::DestructRange(begin, end);
    m_size = 0;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Swap(Array& other) noexcept
{
    m_buffer.Swap(other.m_buffer);
    mini::Swap(m_size, other.m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Pointer Array<T, AllocT>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstPointer Array<T, AllocT>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::Begin() noexcept
{
    return Iterator(m_buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::Begin() const noexcept
{
    return ConstIterator(m_buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::End() noexcept
{
    return Iterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::End() const noexcept
{
    return ConstIterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *(m_buffer.Data());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *(m_buffer.Data());
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::Last()
{
    AssertValidIndex(m_size - 1);
    return *(m_buffer.Data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::Last() const
{
    AssertValidIndex(m_size - 1);
    return *(m_buffer.Data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr SizeT Array<T, AllocT>::Size() const noexcept
{
    return m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr SizeT Array<T, AllocT>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidIndex(SizeT index) const noexcept
{
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.m_ptr - m_buffer.Data());
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidRange(ConstIterator begin,
                                                   ConstIterator end) const noexcept
{
    ConstPointer buffer = m_buffer.Data();
    SizeT beginIdx = static_cast<SizeT>(begin.m_ptr - buffer);
    SizeT endIdx = static_cast<SizeT>(end.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    SwapNewBuffer(other.m_buffer);
    m_size = Exchange(other.m_size, SizeT(0));
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
    Pointer begin(m_buffer.Data());
    m_buffer.Swap(buf);
    memory::DestructRange(begin, begin + m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::AssignRangeWithSize(U first, U last, SizeT len)
{
    OffsetT size = (OffsetT)m_size;
    SizeT capacity = m_buffer.Capacity();
    Pointer begin = m_buffer.Data();

    if (capacity > len) {
        if (m_size < len) {
            memory::CopyRange(begin, first, first + size);
            memory::ConstructRange(begin + m_size, first + size, last);
        } else {
            memory::CopyRange(begin, first, last);
            memory::DestructRange(begin + len, begin + size);
        }
    } else {
        Buffer newBuf = m_buffer.Resize(len);
        memory::ConstructRange(newBuf.Data(), first, last);
        SwapNewBuffer(newBuf);
    }

    m_size = len;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::AppendRangeWithSize(U first, U last, SizeT len)
{
    SizeT capacity = m_buffer.Capacity();
    SizeT newSize = m_size + len;
    if (newSize <= capacity) {
        memory::ConstructRange(m_buffer.Data() + m_size, first, last);
    } else {
        Buffer newBuf = m_buffer.Increment(newSize - capacity);
        Pointer newBegin = newBuf.Data();
        Pointer begin = m_buffer.Data();

        memory::ConstructBackward(newBegin + newSize, first, last);
        memory::MoveConstructBackward(newBegin + m_size, begin, begin + m_size);
        SwapNewBuffer(newBuf);
    }

    m_size = newSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
inline constexpr void Array<T, AllocT>::InsertRangeWithSize(SizeT index, U first, U last, SizeT len)
{
    SizeT capacity = m_buffer.Capacity();
    SizeT newSize = m_size + len;

    if (newSize <= capacity) {
        Pointer begin = m_buffer.Data();
        Pointer loc = begin + index;
        Pointer end = begin + m_size;

        if (static_cast<SizeT>(end - loc) > len) {
            Pointer middle = begin + m_size - len;
            memory::MoveConstructBackward(end + len, middle, end);
            memory::MoveBackward(end, loc, middle);
            memory::DestructRange(loc, loc + len);
        } else {
            memory::MoveConstructBackward(end + len, loc, end);
            memory::DestructRange(loc, end);
        }

        m_size = newSize;
        memory::ConstructRange(loc, first, last);
    } else {
        Buffer newBuf = m_buffer.Increment(len);
        Pointer newBegin = newBuf.Data();
        Pointer begin = m_buffer.Data();

        memory::ConstructRange(newBegin + index, first, last);
        memory::MoveConstructRange(newBegin, begin, begin + index);
        memory::MoveConstructBackward(newBegin + newSize, begin + index, begin + m_size);
        SwapNewBuffer(newBuf);
        m_size = newSize;
    }
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void
Array<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                   [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end), "invalid range");
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