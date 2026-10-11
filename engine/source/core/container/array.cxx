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
    explicit constexpr Array(size_t initCapacity, AllocT const& alloc = AllocT());
    constexpr Array(ArrayView<T> view, AllocT const& alloc = AllocT())
        requires CopyableT<T>;
    constexpr Array(ArrayView<T> view, size_t count, AllocT const& alloc = AllocT())
        requires CopyableT<T>;
    template <ForwardIteratableByT<T> Iter>
    constexpr Array(Iter rangeBegin, Iter rangeEnd, AllocT const& alloc = AllocT());

    template <typename... Args>
    constexpr void pushBack(Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void insert(size_t index, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void insert(ConstIterator iter, Args&&... args)
        requires ConstructibleFromT<T, Args...>;

    template <ArrayLikeT<T> U>
    constexpr void assign(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void append(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void insertRange(size_t index, U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void insertRange(ConstIterator iter, U const& arr)
        requires CopyableT<T>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void assign(Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void append(Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void insertRange(size_t index, Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void insertRange(ConstIterator iter, Iter rangeBegin, Iter rangeEnd);

    constexpr Value popLast();
    constexpr void popBack();
    constexpr void popBack(size_t count);
    constexpr void remove(size_t index);
    constexpr void remove(ConstIterator iter);
    constexpr void removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd);

    template <typename... Args>
    constexpr void resize(size_t count, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    constexpr void reserve(size_t newCapacity);
    constexpr void shrink();
    constexpr void clear();
    constexpr void swap(Array& other) noexcept;

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

    constexpr Array& operator=(ArrayView<T> view)
        requires CopyableT<T>;
    constexpr Array& operator=(Array const& other)
        requires CopyableT<T>;
    constexpr Array& operator=(Array&& other) noexcept;

    template <MovableT U>
    constexpr operator ArrayView<U>() const noexcept
        requires ConvertibleToT<T const*, U const*>;

private:
    constexpr void swapNewBuffer(Buffer& buffer) noexcept;
    template <typename U>
    constexpr void assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len);
    template <typename U>
    constexpr void appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len);
    template <typename U>
    constexpr void insertRangeWithSize(size_t index, U rangeBegin, U rangeEnd, size_t len);

    constexpr void assertValidIndex(size_t index) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
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
    clear();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array const& other)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(other.m_buffer.allocator())
{
    m_buffer.allocate(other.size());
    memory::constructRange(m_buffer.data(), other.begin(), other.end());
    m_size = other.size();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array const& other, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.allocate(other.size());
    memory::constructRange(m_buffer.data(), other.begin(), other.end());
    m_size = other.size();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array&& other) noexcept
    : m_size(exchange(other.m_size, size_t{0}))
    , m_buffer(exchange(other.m_buffer, {}))
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(Array&& other, AllocT const& alloc) noexcept
    : m_size(exchange(other.m_size, size_t{0}))
    , m_buffer(moveCast(other.m_buffer), alloc)
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
    , m_buffer(moveCast(alloc))
{
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(size_t initCapacity, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    m_buffer.allocate(initCapacity);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(ArrayView<T> view, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    ConstPointer viewData = view.data();
    size_t count = view.size();

    m_buffer.allocate(view.size());
    memory::constructRange(m_buffer.data(), viewData, viewData + count);
    m_size = count;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Array(ArrayView<T> view, size_t count, AllocT const& alloc)
    requires CopyableT<T>
    : m_size(0)
    , m_buffer(alloc)
{
    ArrayView<T> subview = view.subFront(count);
    ConstPointer viewData = subview.data();
    size_t subSize = subview.size();

    m_buffer.allocate(count);
    memory::constructRange(m_buffer.data(), viewData, viewData + subSize);
    m_size = subSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr Array<T, AllocT>::Array(Iter rangeBegin, Iter rangeEnd, AllocT const& alloc)
    : m_size(0)
    , m_buffer(alloc)
{
    size_t count = distance(rangeBegin, rangeEnd);
    if (count == 0) [[unlikely]] {
        return;
    }

    m_buffer.allocate(count);
    memory::constructRange(m_buffer.data(), rangeBegin, rangeEnd);
    m_size = count;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::pushBack(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_size < m_buffer.capacity()) {
        memory::constructAt(m_buffer.data() + m_size, forwardCast<Args>(args)...);
    } else {
        Buffer newbuf = m_buffer.increment(1);
        Pointer newbegin = newbuf.data();
        Pointer curbegin = m_buffer.data();

        memory::constructAt(newbegin + m_size, forwardCast<Args>(args)...);
        memory::moveconstructBackward(newbegin + m_size, curbegin, curbegin + m_size);
        swapNewBuffer(newbuf);
    }

    ++m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    insert(begin() + static_cast<offset_t>(index), forwardCast<Args>(args)...);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - begin();
    if (static_cast<offset_t>(m_size) == locDiff) {
        pushBack(forwardCast<Args>(args)...);
        return;
    }

    assertValidIterator(iter);
    if (m_size < m_buffer.capacity()) {
        // without the copy, invalid reference can get copied
        Value temp(forwardCast<Args>(args)...);
        Pointer curbegin = m_buffer.data();
        Pointer curend = curbegin + m_size;
        Pointer loc = curbegin + locDiff;
        Pointer last = curend - 1;

        memory::constructAt(curend, moveCast(*last));
        memory::moveBackward(curend, loc, last);
        memory::destructAt(loc);
        memory::constructAt(loc, moveCast(temp));
    } else {
        Buffer newbuf = m_buffer.increment(1);
        Pointer newbegin = newbuf.data();
        Pointer newloc = newbegin + locDiff;
        Pointer curbegin = m_buffer.data();
        Pointer loc = curbegin + locDiff;

        memory::constructAt(newloc, forwardCast<Args>(args)...);
        memory::moveconstructRange(newbegin, curbegin, loc);
        memory::moveconstructRange(newloc + 1, loc, curbegin + m_size);
        swapNewBuffer(newbuf);
    }

    ++m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::assign(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    size_t count = view.size();
    if (count == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(view.begin(), view.end(), count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::append(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    size_t count = view.size();
    if (count == 0) [[unlikely]] {
        return;
    }

    appendRangeWithSize(view.begin(), view.end(), count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::insertRange(size_t index, U const& arr)
    requires CopyableT<T>
{
    if (index == m_size) {
        append(arr);
        return;
    }

    ArrayView view = arr;
    size_t count = view.size();
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  insert(index, *view.data()); return;
        default: break;
    }

    assertValidIndex(index);
    insertRangeWithSize(index, view.begin(), view.end(), count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ArrayLikeT<T> U>
constexpr void Array<T, AllocT>::insertRange(ConstIterator iter, U const& arr)
    requires CopyableT<T>
{
    size_t locDiff = static_cast<size_t>(iter - begin());
    if (locDiff == m_size) {
        append(arr);
        return;
    }

    ArrayView<T> view = arr;
    size_t count = view.size();
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  insert(iter, *view.data()); return;
        default: break;
    }

    assertValidIterator(iter);
    insertRangeWithSize(locDiff, view.begin(), view.end(), count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::assign(Iter rangeBegin, Iter rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    if (count == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(rangeBegin, rangeEnd, count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::append(Iter rangeBegin, Iter rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  pushBack(forwardCast<typename Iter::Value>(*rangeBegin)); return;
        default: break;
    }

    appendRangeWithSize(rangeBegin, rangeEnd, count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::insertRange(size_t index, Iter rangeBegin, Iter rangeEnd)
{
    if (index == m_size) {
        append(rangeBegin, rangeEnd);
        return;
    }

    size_t count = distance(rangeBegin, rangeEnd);
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  insert(index, forwardCast<typename Iter::Value>(*rangeBegin)); return;
        default: break;
    }

    assertValidIndex(index);
    insertRangeWithSize(index, rangeBegin, rangeEnd, count);
}

template <MovableT T, AllocatorT<T> AllocT>
template <ForwardIteratableByT<T> Iter>
constexpr void Array<T, AllocT>::insertRange(ConstIterator iter, Iter rangeBegin, Iter rangeEnd)
{
    size_t locDiff = static_cast<size_t>(iter - begin());
    if (locDiff == m_size) {
        append(rangeBegin, rangeEnd);
        return;
    }

    size_t count = distance(rangeBegin, rangeEnd);
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  insert(iter, forwardCast<typename Iter::Value>(*rangeBegin)); return;
        default: break;
    }

    assertValidIterator(iter);
    insertRangeWithSize(locDiff, rangeBegin, rangeEnd, count);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Value Array<T, AllocT>::popLast()
{
    size_t index = m_size - 1;
    assertValidIndex(index);

    Pointer loc = m_buffer.data() + index;
    Value value{moveCast(*loc)};
    memory::destructAt(loc);
    --m_size;
    return value;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::popBack()
{
    if (empty()) [[unlikely]] {
        return;
    }

    memory::destructAt(m_buffer.data() + m_size - 1);
    --m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::popBack(size_t count)
{
    if (empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = m_size < count ? m_size : count;
    Pointer end = m_buffer.data() + m_size;
    memory::destructRange(end - removeCnt, end);
    m_size -= removeCnt;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::remove(size_t index)
{
    remove(begin() + static_cast<offset_t>(index));
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::remove(ConstIterator iter)
{
    offset_t locDiff = iter - begin();
    if (static_cast<offset_t>(m_size - 1) == locDiff) [[unlikely]] {
        popBack();
        return;
    }

    assertValidIterator(iter);
    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + m_size;
    Pointer loc = curbegin + locDiff;

    memory::moveRange(loc, loc + 1, curend);
    memory::destructAt(curend - 1);
    --m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  remove(rangeBegin); return;
        default: break;
    }

    assertValidRange(rangeBegin, rangeEnd);
    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + m_size;
    Pointer loc = curbegin + (rangeBegin - begin());

    memory::moveRange(loc, loc + count, curend);
    memory::destructRange(curend - count, curend);
    m_size -= count;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename... Args>
constexpr void Array<T, AllocT>::resize(size_t count, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_size == count) [[unlikely]] {
        return;
    }

    Pointer curbegin = m_buffer.data();

    if (m_size < count) {
        Value temp(forwardCast<Args>(args)...);

        if (m_buffer.capacity() < count) {
            Buffer newbuf = m_buffer.resize(count);
            Pointer newbegin(newbuf.data());

            memory::constructRangeArgs(newbegin + m_size, newbegin + count, temp);
            memory::moveconstructRange(newbegin, curbegin, curbegin + m_size);
            swapNewBuffer(newbuf);
        } else {
            memory::constructRangeArgs(curbegin + m_size, curbegin + count, temp);
        }
    } else {
        memory::destructRange(curbegin + count, curbegin + m_size);
    }

    m_size = count;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::reserve(size_t newCapacity)
{
    if (m_buffer.capacity() > newCapacity) [[unlikely]] {
        return;
    }

    Buffer newbuf = m_buffer.resize(newCapacity);
    Pointer newbegin(newbuf.data());
    Pointer oldBegin(m_buffer.data());

    memory::moveconstructRange(newbegin, oldBegin, oldBegin + m_size);
    swapNewBuffer(newbuf);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::shrink()
{
    if (m_buffer.capacity() == m_size) [[unlikely]] {
        return;
    }

    Buffer newbuf = m_buffer.resize(m_size);
    Pointer newbegin(newbuf.data());
    Pointer oldBegin(m_buffer.data());

    memory::moveconstructRange(newbegin, oldBegin, oldBegin + m_size);
    swapNewBuffer(newbuf);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::clear()
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + m_size;

    memory::destructRange(curbegin, curend);
    m_size = 0;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::swap(Array& other) noexcept
{
    m_buffer.swap(other.m_buffer);
    mini::swap(m_size, other.m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Pointer Array<T, AllocT>::data() noexcept
{
    return m_buffer.data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstPointer Array<T, AllocT>::data() const noexcept
{
    return m_buffer.data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::begin() noexcept
{
    return Iterator(m_buffer.data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::begin() const noexcept
{
    return ConstIterator(m_buffer.data(), this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::Iterator Array<T, AllocT>::end() noexcept
{
    return Iterator(m_buffer.data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>::ConstIterator Array<T, AllocT>::end() const noexcept
{
    return ConstIterator(m_buffer.data() + m_size, this);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::first()
{
    assertValidIndex(0);
    return *m_buffer.data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::first() const
{
    assertValidIndex(0);
    return *m_buffer.data();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::last()
{
    assertValidIndex(m_size - 1);
    return *(m_buffer.data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::last() const
{
    assertValidIndex(m_size - 1);
    return *(m_buffer.data() + m_size - 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::at(size_t index)
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::at(size_t index) const
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr size_t Array<T, AllocT>::size() const noexcept
{
    return m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr size_t Array<T, AllocT>::capacity() const noexcept
{
    return m_buffer.capacity();
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::validIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::validIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.data());
    return index < m_size;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr bool Array<T, AllocT>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    ConstPointer buffer = m_buffer.data();
    size_t beginIdx = static_cast<size_t>(rangeBegin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(rangeEnd.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T& Array<T, AllocT>::operator[](size_t index)
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr T const& Array<T, AllocT>::operator[](size_t index) const
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    assign(other.begin(), other.end());
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(Array&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    swapNewBuffer(other.m_buffer);
    m_size = exchange(other.m_size, size_t{0});
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr Array<T, AllocT>& Array<T, AllocT>::operator=(ArrayView<T> view)
    requires CopyableT<T>
{
    assign(view);
    return *this;
}

template <MovableT T, AllocatorT<T> AllocT>
template <MovableT U>
constexpr Array<T, AllocT>::operator ArrayView<U>() const noexcept
    requires ConvertibleToT<T const*, U const*>
{
    return ArrayView<U>{m_buffer.data(), m_size};
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::swapNewBuffer(Buffer& buffer) noexcept
{
    Pointer curbegin = m_buffer.data();
    m_buffer.swap(buffer);
    memory::destructRange(curbegin, curbegin + m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    offset_t count = static_cast<offset_t>(m_size);
    Pointer curbegin = m_buffer.data();

    if (m_buffer.capacity() > len) {
        if (m_size < len) {
            memory::copyRange(curbegin, rangeBegin, rangeBegin + count);
            memory::constructRange(curbegin + m_size, rangeBegin + count, rangeEnd);
        } else {
            memory::copyRange(curbegin, rangeBegin, rangeEnd);
            memory::destructRange(curbegin + len, curbegin + count);
        }
    } else {
        Buffer newbuf = m_buffer.resize(len);
        memory::constructRange(newbuf.data(), rangeBegin, rangeEnd);
        swapNewBuffer(newbuf);
    }

    m_size = len;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    size_t bufferCapacity = m_buffer.capacity();
    size_t newSize = m_size + len;
    if (newSize <= bufferCapacity) {
        memory::constructRange(m_buffer.data() + m_size, rangeBegin, rangeEnd);
    } else {
        Buffer newbuf = m_buffer.increment(newSize - bufferCapacity);
        Pointer newbegin = newbuf.data();
        Pointer curbegin = m_buffer.data();

        memory::constructBackward(newbegin + newSize, rangeBegin, rangeEnd);
        memory::moveconstructBackward(newbegin + m_size, curbegin, curbegin + m_size);
        swapNewBuffer(newbuf);
    }

    m_size = newSize;
}

template <MovableT T, AllocatorT<T> AllocT>
template <typename U>
constexpr void Array<T, AllocT>::insertRangeWithSize(size_t index, U rangeBegin, U rangeEnd, size_t len)
{
    size_t newSize = m_size + len;

    if (newSize <= m_buffer.capacity()) {
        Pointer curbegin = m_buffer.data();
        Pointer curend = curbegin + m_size;
        Pointer ptrLoc = curbegin + index;

        if (static_cast<size_t>(curend - ptrLoc) > len) {
            Pointer middle = curbegin + m_size - len;
            memory::moveconstructBackward(curend + len, middle, curend);
            memory::moveBackward(curend, ptrLoc, middle);
            memory::destructRange(ptrLoc, ptrLoc + len);
        } else {
            memory::moveconstructBackward(curend + len, ptrLoc, curend);
            memory::destructRange(ptrLoc, curend);
        }

        m_size = newSize;
        memory::constructRange(ptrLoc, rangeBegin, rangeEnd);
    } else {
        Buffer newbuf = m_buffer.increment(len);
        Pointer newbegin = newbuf.data();
        Pointer curbegin = m_buffer.data();

        memory::constructRange(newbegin + index, rangeBegin, rangeEnd);
        memory::moveconstructRange(newbegin, curbegin, curbegin + index);
        memory::moveconstructBackward(newbegin + newSize, curbegin + index, curbegin + m_size);
        swapNewBuffer(newbuf);
        m_size = newSize;
    }
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::assertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(validIndex(index), "invalid index {}. current count is {}", index, m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter),
           "invalid iterator at index {}. current count is {}",
           iter.address() - m_buffer.data(),
           m_size);
}

template <MovableT T, AllocatorT<T> AllocT>
constexpr void Array<T, AllocT>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                                  [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd),
           "invalid range from {} to {}. current count is {}",
           rangeBegin.address() - m_buffer.data(),
           rangeBegin.address() - m_buffer.data(),
           m_size);
}

export template <MovableT T, AllocatorT<T> AllocT, MovableT U, AllocatorT<U> AllocU>
constexpr bool operator==(Array<T, AllocT> const& lhs, Array<U, AllocU> const& rhs)
    requires EqualityComparableWithT<T, U>
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    if (lhs.data() == rhs.data()) [[unlikely]] {
        return true;
    }

    return memory::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

export template <MovableT T, AllocatorT<T> AllocT>
constexpr void swap(Array<T, AllocT>& lhs, Array<T, AllocT>& rhs) noexcept
{
    return lhs.swap(rhs);
}

} // namespace mini