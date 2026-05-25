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
    size_t m_size;
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
    explicit constexpr Array(size_t, AllocT const& = AllocT());
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr Array(Iter, Iter, AllocT const& = AllocT())
        requires CopyableT<T>;

    template <typename... Args>
    constexpr void Push(Args&&...)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(size_t, Args&&...)
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
    constexpr void InsertRange(size_t, Iter, Iter);
    constexpr void InsertRange(size_t, InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator, Iter, Iter);
    constexpr void InsertRange(ConstIterator, InitializerList<T>);

    constexpr void RemoveLast();
    constexpr void RemoveLast(size_t);
    constexpr void RemoveAt(size_t);
    constexpr void RemoveAt(ConstIterator);
    constexpr void RemoveRange(size_t, size_t);
    constexpr void RemoveRange(ConstIterator, ConstIterator);

    template <typename... Args>
    constexpr void Resize(size_t, Args&&...)
        requires ConstructibleFromT<T, Args...>;
    constexpr void Reserve(size_t);
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

    constexpr Array& operator=(Array const&)
        requires CopyableT<T>;
    constexpr Array& operator=(Array&&) noexcept;
    constexpr Array& operator=(InitializerList<T>);

private:
    constexpr void SwapNewBuffer(Buffer&);
    template <typename U>
    constexpr void AssignRangeWithSize(U, U, size_t);
    template <typename U>
    constexpr void AppendRangeWithSize(U, U, size_t);
    template <typename U>
    constexpr void InsertRangeWithSize(size_t, U, U, size_t);

    constexpr void AssertValidIndex(size_t) const noexcept;
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
    : m_size(Exchange(other.m_size, size_t(0)))
    , m_buffer(Exchange(other.m_buffer, { }))
{
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr Array<T, AllocT>::Array(Array&& other, AllocT const& alloc) noexcept
    : m_size(Exchange(other.m_size, size_t(0)))
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
inline constexpr Array<T, AllocT>::Array(size_t capacity, AllocT const& alloc)
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
    size_t distance = Distance(first, last);
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
    size_t capacity = m_buffer.Capacity();

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
inline constexpr void Array<T, AllocT>::Insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + (offset_t)index, ForwardArg<Args>(args)...);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - Begin();
    if (locDiff == (offset_t)m_size) {
        Push(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidIterator(iter);
    size_t capacity = m_buffer.Capacity();

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
    size_t distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(first, last, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::Assign(InitializerList<T> init)
{
    size_t len = init.size();
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
    size_t distance = Distance(first, last);
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
inline constexpr void Array<T, AllocT>::InsertRange(size_t index, Iter first, Iter last)
{
    if (index == m_size) {
        Append(first, last);
        return;
    }

    size_t distance = Distance(first, last);
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
inline constexpr void Array<T, AllocT>::InsertRange(size_t index, InitializerList<T> init)
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
    size_t locDiff = static_cast<size_t>(iter - Begin());
    if (locDiff == m_size) {
        Append(first, last);
        return;
    }

    size_t distance = Distance(first, last);
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
    size_t locDiff = static_cast<size_t>(iter - Begin());
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
inline constexpr void Array<T, AllocT>::RemoveLast(size_t count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = m_size < count ? m_size : count;
    Pointer end = m_buffer.Data() + m_size;
    memory::DestructRange(end - removeCnt, end);
    m_size -= removeCnt;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::RemoveAt(size_t index)
{
    RemoveAt(Begin() + (offset_t)index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::RemoveAt(ConstIterator iter)
{
    offset_t locDiff = iter - Begin();
    if (locDiff == offset_t(m_size) - 1) [[unlikely]] {
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
inline constexpr void Array<T, AllocT>::RemoveRange(size_t first, size_t last)
{
    Iterator begin = Begin();
    RemoveRange(begin + (offset_t)first, begin + (offset_t)last);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::RemoveRange(ConstIterator first, ConstIterator last)
{
    size_t distance = Distance(first, last);
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
constexpr void Array<T, AllocT>::Resize(size_t size, Args&&... args)
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
inline constexpr void Array<T, AllocT>::Reserve(size_t size)
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
inline constexpr T& Array<T, AllocT>::At(size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr size_t Array<T, AllocT>::Size() const noexcept
{
    return m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr size_t Array<T, AllocT>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.Data());
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr bool Array<T, AllocT>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    ConstPointer buffer = m_buffer.Data();
    size_t beginIdx = static_cast<size_t>(begin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(end.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T& Array<T, AllocT>::operator[](size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr T const& Array<T, AllocT>::operator[](size_t index) const
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
    m_size = Exchange(other.m_size, size_t(0));
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
inline constexpr void Array<T, AllocT>::AssignRangeWithSize(U first, U last, size_t len)
{
    offset_t size = (offset_t)m_size;
    size_t capacity = m_buffer.Capacity();
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
inline constexpr void Array<T, AllocT>::AppendRangeWithSize(U first, U last, size_t len)
{
    size_t capacity = m_buffer.Capacity();
    size_t newSize = m_size + len;
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
inline constexpr void Array<T, AllocT>::InsertRangeWithSize(size_t index, U first, U last, size_t len)
{
    size_t capacity = m_buffer.Capacity();
    size_t newSize = m_size + len;

    if (newSize <= capacity) {
        Pointer begin = m_buffer.Data();
        Pointer loc = begin + index;
        Pointer end = begin + m_size;

        if (static_cast<size_t>(end - loc) > len) {
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
inline constexpr void Array<T, AllocT>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator");
}

template <MovableT T, AllocatorT<T> AllocT>
inline constexpr void Array<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
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