export module mini.core:array;

import :type;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :allocator;
import :dynamic_buffer;
import :array_view;
import :array_iterator;

namespace mini {

export template <MovableT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class Array;

template <typename T, typename ValueT, typename AllocT>
concept ArrayLikeExceptArrayT =
    ArrayLikeT<T, ValueT> && AllocatorT<AllocT, ValueT> && !SameAsT<RemoveConstVolatileRefT<T>, Array<ValueT, AllocT>>;

template <MovableT T, AllocatorT<T> AllocT>
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
    constexpr Array(Array const& other)
        requires CopyableT<T>;
    constexpr Array(Array const& other, AllocT const& alloc)
        requires CopyableT<T>;
    constexpr Array(Array&& other) noexcept;
    constexpr Array(Array&& other, AllocT const& alloc) noexcept;
    explicit constexpr Array(AllocT const& alloc) noexcept;
    explicit constexpr Array(AllocT&& alloc) noexcept;
    explicit constexpr Array(size_t capacity, AllocT const& alloc = AllocT());
    template <ArrayLikeExceptArrayT<T, AllocT> U>
    constexpr Array(U const& arr, AllocT const& alloc = AllocT())
        requires CopyableT<T>;
    template <ArrayLikeExceptArrayT<T, AllocT> U>
    constexpr Array(U const& arr, size_t size, AllocT const& alloc = AllocT())
        requires CopyableT<T>;
    template <ForwardIteratableByT<T> Iter>
    constexpr Array(Iter begin, Iter end, AllocT const& alloc = AllocT());

    template <typename... Args>
    constexpr void PushBack(Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(size_t index, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(ConstIterator iter, Args&&... args)
        requires ConstructibleFromT<T, Args...>;

    template <ArrayLikeT<T> U>
    constexpr void Assign(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void Append(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void InsertRange(size_t index, U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void InsertRange(ConstIterator iter, U const& arr)
        requires CopyableT<T>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter begin, Iter end);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter begin, Iter end);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(size_t index, Iter begin, Iter end);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator iter, Iter begin, Iter end);

    constexpr Value PopLast();
    constexpr void PopBack();
    constexpr void PopBack(size_t count);
    constexpr void Remove(size_t index);
    constexpr void Remove(ConstIterator iter);
    constexpr void RemoveRange(ConstIterator begin, ConstIterator end);

    template <typename... Args>
    constexpr void Resize(size_t size, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    constexpr void Reserve(size_t capacity);
    constexpr void Shrink();
    constexpr void Clear();
    constexpr void Swap(Array& other) noexcept;

    [[nodiscard]] constexpr Pointer Data() noexcept;
    [[nodiscard]] constexpr ConstPointer Data() const noexcept;
    [[nodiscard]] constexpr Iterator Begin() noexcept;
    [[nodiscard]] constexpr ConstIterator Begin() const noexcept;
    [[nodiscard]] constexpr Iterator End() noexcept;
    [[nodiscard]] constexpr ConstIterator End() const noexcept;
    [[nodiscard]] constexpr Reference First();
    [[nodiscard]] constexpr ConstReference First() const;
    [[nodiscard]] constexpr Reference Last();
    [[nodiscard]] constexpr ConstReference Last() const;
    [[nodiscard]] constexpr Reference At(size_t index);
    [[nodiscard]] constexpr ConstReference At(size_t index) const;

    [[nodiscard]] constexpr size_t Size() const noexcept;
    [[nodiscard]] constexpr size_t Capacity() const noexcept;
    [[nodiscard]] constexpr bool Empty() const noexcept;
    [[nodiscard]] constexpr bool ValidIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool ValidIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool ValidRange(ConstIterator begin, ConstIterator end) const noexcept;

    [[nodiscard]] constexpr Reference operator[](size_t index);
    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    constexpr Array& operator=(Array const& other)
        requires CopyableT<T>;
    constexpr Array& operator=(Array&& other) noexcept;
    template <ArrayLikeExceptArrayT<T, AllocT> U>
    constexpr Array& operator=(U const& arr)
        requires CopyableT<T>;

    constexpr operator ArrayView<T>() const noexcept;

private:
    constexpr void SwapNewBuffer(Buffer& buffer) noexcept;
    template <typename U>
    constexpr void AssignRangeWithSize(U begin, U end, size_t len);
    template <typename U>
    constexpr void AppendRangeWithSize(U begin, U end, size_t len);
    template <typename U>
    constexpr void InsertRangeWithSize(size_t index, U begin, U end, size_t len);

    constexpr void AssertValidIndex(size_t index) const noexcept;
    constexpr void AssertValidIterator(ConstIterator iter) const noexcept;
    constexpr void AssertValidRange(ConstIterator begin, ConstIterator end) const noexcept;
};

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array() noexcept
    : m_size(0)
    , m_buffer()
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::~Array()
{
    Clear();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array const& other)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(other.m_buffer.GetAllocator())
{
    m_buffer.Allocate(other.Size());
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array const& other, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.Allocate(other.Size());
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.Size();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array&& other) noexcept
    : m_size(Exchange(other.m_size, size_t{0}))
    , m_buffer(Exchange(other.m_buffer, {}))
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array&& other, AllocT const& alloc) noexcept
    : m_size(Exchange(other.m_size, size_t{0}))
    , m_buffer(MoveArg(other.m_buffer), alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(AllocT const& alloc) noexcept
    : m_size(0)
    , m_buffer(alloc)
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(AllocT&& alloc) noexcept
    : m_size(0)
    , m_buffer(MoveArg(alloc))
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(size_t capacity, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.Allocate(capacity);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeExceptArrayT<T, AllocT> U>
constexpr Array<T, AllocT>::Array(U const& arr, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    ArrayView<T> view = arr;
    ConstPointer data = view.Data();
    size_t size = view.Size();

    m_buffer.Allocate(view.Size());
    memory::ConstructRange(m_buffer.Data(), data, data + size);
    m_size = size;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeExceptArrayT<T, AllocT> U>
constexpr Array<T, AllocT>::Array(U const& arr, size_t size, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    ArrayView<T> view = arr;
    ArrayView<T> subView = arr.SubFront(size);
    ConstPointer data = subView.Data();
    size_t subSize = subView.Size();

    m_buffer.Allocate(size);
    memory::ConstructRange(m_buffer.Data(), data, data + subSize);
    m_size = subSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr Array<T, AllocT>::Array(Iter begin, Iter end, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    size_t distance = Distance(begin, end);
    if (distance == 0) [[unlikely]] {
        return;
    }

    m_buffer.Allocate(distance);
    memory::ConstructRange(m_buffer.Data(), begin, end);
    m_size = distance;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::PushBack(Args&&... args)
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
constexpr void Array<T, AllocT>::Insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + static_cast<offset_t>(index), ForwardArg<Args>(args)...);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - Begin();
    if (static_cast<offset_t>(m_size) == locDiff) {
        PushBack(ForwardArg<Args>(args)...);
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
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::Assign(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    size_t size = view.Size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(view.Begin(), view.End(), size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::Append(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    size_t size = view.Size();
    if (size == 0) [[unlikely]] {
        return;
    }

    AppendRangeWithSize(view.Begin(), view.End(), size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::InsertRange(size_t index, U const& arr)
    requires CopyableT<T>
{
    if (index == m_size) {
        Append(arr);
        return;
    }

    ArrayView view = arr;
    size_t size = view.Size();
    switch (size) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(index, *view.Data()); return;
        default: break;
    }

    AssertValidIndex(index);
    InsertRangeWithSize(index, view.Begin(), view.End(), size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::InsertRange(ConstIterator iter, U const& arr)
    requires CopyableT<T>
{
    size_t locDiff = static_cast<size_t>(iter - Begin());
    if (locDiff == m_size) {
        Append(arr);
        return;
    }

    ArrayView<T> view = arr;
    size_t size = view.Size();
    switch (size) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(iter, *view.Data()); return;
        default: break;
    }

    AssertValidIterator(iter);
    InsertRangeWithSize(locDiff, view.Begin(), view.End(), size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::Assign(Iter begin, Iter end)
{
    size_t distance = Distance(begin, end);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(begin, end, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::Append(Iter begin, Iter end)
{
    size_t distance = Distance(begin, end);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  PushBack(ForwardArg<typename Iter::Value>(*begin)); return;
        default: break;
    }

    AppendRangeWithSize(begin, end, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::InsertRange(size_t index, Iter begin, Iter end)
{
    if (index == m_size) {
        Append(begin, end);
        return;
    }

    size_t distance = Distance(begin, end);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(index, ForwardArg<typename Iter::Value>(*begin)); return;
        default: break;
    }

    AssertValidIndex(index);
    InsertRangeWithSize(index, begin, end, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::InsertRange(ConstIterator iter, Iter begin, Iter end)
{
    size_t locDiff = static_cast<size_t>(iter - Begin());
    if (locDiff == m_size) {
        Append(begin, end);
        return;
    }

    size_t distance = Distance(begin, end);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Insert(iter, ForwardArg<typename Iter::Value>(*begin)); return;
        default: break;
    }

    AssertValidIterator(iter);
    InsertRangeWithSize(locDiff, begin, end, distance);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Value Array<T, AllocT>::PopLast()
{
    size_t index = m_size - 1;
    AssertValidIndex(index);

    Pointer loc = m_buffer.Data() + index;
    Value value{MoveArg(*loc)};
    memory::DestructAt(loc);
    --m_size;
    return value;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::PopBack()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_size - 1);
    --m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::PopBack(size_t count)
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
constexpr void Array<T, AllocT>::Remove(size_t index)
{
    Remove(Begin() + static_cast<offset_t>(index));
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::Remove(ConstIterator iter)
{
    offset_t locDiff = iter - Begin();
    if (static_cast<offset_t>(m_size - 1) == locDiff) [[unlikely]] {
        PopBack();
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
constexpr void Array<T, AllocT>::RemoveRange(ConstIterator begin, ConstIterator end)
{
    size_t distance = Distance(begin, end);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Remove(begin); return;
        default: break;
    }

    AssertValidRange(begin, end);
    Iterator iterBegin = Begin();
    Pointer ptrBegin = m_buffer.Data();
    Pointer ptrEnd = ptrBegin + m_size;
    Pointer ptrLoc = ptrBegin + (begin - iterBegin);

    memory::MoveRange(ptrLoc, ptrLoc + distance, ptrEnd);
    memory::DestructRange(ptrEnd - distance, ptrEnd);
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
constexpr void Array<T, AllocT>::Reserve(size_t capacity)
{
    if (m_buffer.Capacity() > capacity) [[unlikely]] {
        return;
    }

    Buffer newBuf = m_buffer.Resize(capacity);
    Pointer newBegin(newBuf.Data());
    Pointer oldBegin(m_buffer.Data());

    memory::MoveConstructRange(newBegin, oldBegin, oldBegin + m_size);
    SwapNewBuffer(newBuf);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::Shrink()
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
constexpr void Array<T, AllocT>::Clear()
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
constexpr void Array<T, AllocT>::Swap(Array& other) noexcept
{
    m_buffer.Swap(other.m_buffer);
    mini::Swap(m_size, other.m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Pointer Array<T, AllocT>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstPointer Array<T, AllocT>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::Begin() noexcept
{
    return Iterator(m_buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::Begin() const noexcept
{
    return ConstIterator(m_buffer.Data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::End() noexcept
{
    return Iterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::End() const noexcept
{
    return ConstIterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::First()
{
    AssertValidIndex(0);
    return *m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::First() const
{
    AssertValidIndex(0);
    return *m_buffer.Data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::Last()
{
    AssertValidIndex(m_size - 1);
    return *(m_buffer.Data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::Last() const
{
    AssertValidIndex(m_size - 1);
    return *(m_buffer.Data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::At(size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr size_t Array<T, AllocT>::Size() const noexcept
{
    return m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr size_t Array<T, AllocT>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.Data());
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    ConstPointer buffer = m_buffer.Data();
    size_t beginIdx = static_cast<size_t>(begin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(end.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::operator[](size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    SwapNewBuffer(other.m_buffer);
    m_size = Exchange(other.m_size, size_t{0});
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeExceptArrayT<T, AllocT> U>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(U const& arr)
    requires CopyableT<T>
{
    Assign(arr);
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::operator ArrayView<T>() const noexcept
{
    return ArrayView<T>{m_buffer.Data(), m_size};
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::SwapNewBuffer(Buffer& buffer) noexcept
{
    Pointer begin(m_buffer.Data());
    m_buffer.Swap(buffer);
    memory::DestructRange(begin, begin + m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::AssignRangeWithSize(U begin, U end, size_t len)
{
    offset_t size = static_cast<offset_t>(m_size);
    size_t capacity = m_buffer.Capacity();
    Pointer ptrBegin = m_buffer.Data();

    if (capacity > len) {
        if (m_size < len) {
            memory::CopyRange(ptrBegin, begin, begin + size);
            memory::ConstructRange(ptrBegin + m_size, begin + size, end);
        } else {
            memory::CopyRange(ptrBegin, begin, end);
            memory::DestructRange(ptrBegin + len, ptrBegin + size);
        }
    } else {
        Buffer newBuf = m_buffer.Resize(len);
        memory::ConstructRange(newBuf.Data(), begin, end);
        SwapNewBuffer(newBuf);
    }

    m_size = len;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::AppendRangeWithSize(U begin, U end, size_t len)
{
    size_t capacity = m_buffer.Capacity();
    size_t newSize = m_size + len;
    if (newSize <= capacity) {
        memory::ConstructRange(m_buffer.Data() + m_size, begin, end);
    } else {
        Buffer newBuf = m_buffer.Increment(newSize - capacity);
        Pointer newBegin = newBuf.Data();
        Pointer ptrBegin = m_buffer.Data();

        memory::ConstructBackward(newBegin + newSize, begin, end);
        memory::MoveConstructBackward(newBegin + m_size, ptrBegin, ptrBegin + m_size);
        SwapNewBuffer(newBuf);
    }

    m_size = newSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::InsertRangeWithSize(size_t index, U begin, U end, size_t len)
{
    size_t capacity = m_buffer.Capacity();
    size_t newSize = m_size + len;

    if (newSize <= capacity) {
        Pointer ptrBegin = m_buffer.Data();
        Pointer ptrEnd = ptrBegin + m_size;
        Pointer ptrLoc = ptrBegin + index;

        if (static_cast<size_t>(ptrEnd - ptrLoc) > len) {
            Pointer middle = ptrBegin + m_size - len;
            memory::MoveConstructBackward(ptrEnd + len, middle, ptrEnd);
            memory::MoveBackward(ptrEnd, ptrLoc, middle);
            memory::DestructRange(ptrLoc, ptrLoc + len);
        } else {
            memory::MoveConstructBackward(ptrEnd + len, ptrLoc, ptrEnd);
            memory::DestructRange(ptrLoc, ptrEnd);
        }

        m_size = newSize;
        memory::ConstructRange(ptrLoc, begin, end);
    } else {
        Buffer newBuf = m_buffer.Increment(len);
        Pointer newBegin = newBuf.Data();
        Pointer ptrBegin = m_buffer.Data();

        memory::ConstructRange(newBegin + index, begin, end);
        memory::MoveConstructRange(newBegin, ptrBegin, ptrBegin + index);
        memory::MoveConstructBackward(newBegin + newSize, ptrBegin + index, ptrBegin + m_size);
        SwapNewBuffer(newBuf);
        m_size = newSize;
    }
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. current size is {}", index, m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter),
           "invalid iterator at index {}. current size is {}",
           iter.Address() - m_buffer.Data(),
           m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                  [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}. current size is {}",
           begin.Address() - m_buffer.Data(),
           end.Address() - m_buffer.Data(),
           m_size);
}

export template <MovableT T, AllocatorT<T> AllocT, MovableT U, AllocatorT<U> AllocU>
constexpr bool operator==(Array<T, AllocT> const& lhs, Array<U, AllocU> const& rhs)
    requires EqualityComparableWithT<T, U>
{
    if (lhs.Size() != rhs.Size()) {
        return false;
    }

    if (lhs.Data() == rhs.Data()) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End());
}

export template <MovableT T, AllocatorT<T> AllocT>
constexpr void Swap(Array<T, AllocT>& lhs, Array<T, AllocT>& rhs) noexcept
{
    return lhs.Swap(rhs);
}

} // namespace mini