export module mini.core:fixed_array;

import :type;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :fixed_buffer;
import :array_view;
import :array_iterator;

namespace mini {

export template <MovableT T, size_t CapacityN>
class FixedArray {
private:
    typedef memory::FixedBuffer<T, CapacityN> Buffer;
    typedef memory::FixedSize<CapacityN> BufferSize;

    template <MovableT U, size_t CapU>
    friend class FixedArray;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    typedef ArrayIterator<Value, FixedArray> Iterator;
    typedef ArrayIterator<ConstValue, FixedArray const> ConstIterator;

private:
    BufferSize m_size;
    Buffer m_buffer;

public:
    constexpr FixedArray() noexcept;
    constexpr ~FixedArray();
    constexpr FixedArray(FixedArray const& other)
        requires CopyableT<T>;
    constexpr FixedArray(FixedArray&& other) noexcept;
    constexpr FixedArray(ArrayView<T> view)
        requires CopyableT<T>;
    constexpr FixedArray(ArrayView<T> view, size_t count)
        requires CopyableT<T>;
    template <ForwardIteratableByT<T> Iter>
    constexpr FixedArray(Iter rangeBegin, Iter rangeEnd);

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

    constexpr void popBack();
    constexpr void popBack(size_t count);
    constexpr void remove(size_t index);
    constexpr void remove(ConstIterator iter);
    constexpr void removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd);

    template <typename... Args>
    constexpr void resize(size_t count, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    constexpr void clear();

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

    [[nodiscard]] constexpr size_t capacity() const noexcept;
    [[nodiscard]] constexpr size_t size() const noexcept;
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr bool full() const noexcept;
    [[nodiscard]] constexpr bool validIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool validIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;

    [[nodiscard]] constexpr Reference operator[](size_t index);
    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    constexpr FixedArray& operator=(ArrayView<T> view)
        requires CopyableT<T>;
    constexpr FixedArray& operator=(FixedArray const& other)
        requires CopyableT<T>;
    constexpr FixedArray& operator=(FixedArray&& other) noexcept;

    template <MovableT U>
    constexpr operator ArrayView<U>() const noexcept
        requires ConvertibleToT<T const*, U const*>;

private:
    template <typename U>
    constexpr void assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len);
    template <typename U>
    constexpr void appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len);
    template <typename U>
    constexpr void insertRangeWithSize(size_t index, U rangeBegin, U rangeEnd, size_t len);

    constexpr void assertValidCapacity(size_t count) const noexcept;
    constexpr void assertValidIndex(size_t index) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
};

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::FixedArray() noexcept
    : m_size(0)
    , m_buffer()
{
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::~FixedArray()
{
    clear();
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::FixedArray(FixedArray const& other)
    requires CopyableT<T>
{
    memory::constructRange(m_buffer.data(), other.begin(), other.end());
    m_size = other.m_size;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::FixedArray(FixedArray&& other) noexcept
{
    Pointer otherBegin = other.m_buffer.data();
    Pointer otherEnd = otherBegin + other.m_size;

    memory::moveconstructRange(m_buffer.data(), otherBegin, otherEnd);
    memory::destructRange(otherBegin, otherEnd);
    m_size = other.m_size;
    other.m_size = 0;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::FixedArray(ArrayView<T> view)
    requires CopyableT<T>
{
    ConstPointer ptr = view.data();
    size_t count = view.size();

    assertValidCapacity(count);
    memory::constructRange(m_buffer.data(), ptr, ptr + count);
    m_size = count;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::FixedArray(ArrayView<T> view, size_t count)
    requires CopyableT<T>
{
    ArrayView<T> subview = view.subFront(count);
    ConstPointer ptr = subview.data();
    size_t subSize = subview.size();

    assertValidCapacity(count);
    memory::constructRange(m_buffer.data(), ptr, ptr + subSize);
    m_size = count;
}

template <MovableT T, size_t CapacityN>
template <ForwardIteratableByT<T> Iter>
constexpr FixedArray<T, CapacityN>::FixedArray(Iter rangeBegin, Iter rangeEnd)
    : m_size(0)
{
    append(rangeBegin, rangeEnd);
}

template <MovableT T, size_t CapacityN>
template <typename... Args>
constexpr void FixedArray<T, CapacityN>::pushBack(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    assertValidCapacity(m_size + 1);
    memory::constructAt(m_buffer.data() + m_size, forwardCast<Args>(args)...);
    ++m_size;
}

template <MovableT T, size_t CapacityN>
template <typename... Args>
constexpr void FixedArray<T, CapacityN>::insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    insert(begin() + static_cast<offset_t>(index), forwardCast<Args>(args)...);
}

template <MovableT T, size_t CapacityN>
template <typename... Args>
constexpr void FixedArray<T, CapacityN>::insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - begin();
    if (locDiff == static_cast<offset_t>(m_size)) {
        pushBack(forwardCast<Args>(args)...);
        return;
    }

    assertValidCapacity(m_size + 1);
    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + static_cast<offset_t>(m_size);
    Pointer loc = curbegin + locDiff;
    Pointer last = curend - 1;

    // without the copy, invalid reference can get copied
    Value temp(forwardCast<Args>(args)...);

    memory::constructAt(curend, moveCast(*last));
    memory::moveBackward(curend, loc, last);
    memory::destructAt(loc);
    memory::constructAt(loc, moveCast(temp));
    ++m_size;
}

template <MovableT T, size_t CapacityN>
template <ArrayLikeT<T> U>
constexpr void FixedArray<T, CapacityN>::assign(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t count = view.size();
    if (count == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(ptr, ptr + count, count);
}

template <MovableT T, size_t CapacityN>
template <ArrayLikeT<T> U>
constexpr void FixedArray<T, CapacityN>::append(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t count = view.size();
    appendRangeWithSize(ptr, ptr + count, count);
}

template <MovableT T, size_t CapacityN>
template <ArrayLikeT<T> U>
constexpr void FixedArray<T, CapacityN>::insertRange(size_t index, U const& arr)
    requires CopyableT<T>
{
    if (index == m_size) {
        append(arr);
        return;
    }

    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t count = view.size();

    insertRangeWithSize(index, ptr, ptr + count, count);
}

template <MovableT T, size_t CapacityN>
template <ArrayLikeT<T> U>
constexpr void FixedArray<T, CapacityN>::insertRange(ConstIterator iter, U const& arr)
    requires CopyableT<T>
{
    size_t index = static_cast<size_t>(iter - begin());
    if (index == m_size) {
        append(index);
        return;
    }

    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t count = view.size();

    insertRangeWithSize(index, ptr, ptr + count, count);
}

template <MovableT T, size_t CapacityN>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, CapacityN>::assign(Iter rangeBegin, Iter rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    if (count == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(rangeBegin, rangeEnd, count);
}

template <MovableT T, size_t CapacityN>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, CapacityN>::append(Iter rangeBegin, Iter rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    appendRangeWithSize(rangeBegin, rangeEnd, count);
}

template <MovableT T, size_t CapacityN>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, CapacityN>::insertRange(size_t index, Iter rangeBegin, Iter rangeEnd)
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

    insertRangeWithSize(index, rangeBegin, rangeEnd, count);
}

template <MovableT T, size_t CapacityN>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, CapacityN>::insertRange(ConstIterator iter, Iter rangeBegin, Iter rangeEnd)
{
    size_t locDiff = static_cast<size_t>(iter - begin());
    if (locDiff == m_size) {
        append(rangeBegin, rangeEnd);
        return;
    }

    size_t distance = distance(rangeBegin, rangeEnd);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  insert(iter, forwardCast<typename Iter::Value>(*rangeBegin)); return;
        default: break;
    }

    insertRangeWithSize(locDiff, rangeBegin, rangeEnd, distance);
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::popBack()
{
    if (empty()) [[unlikely]] {
        return;
    }

    memory::destructAt(m_buffer.data() + m_size - 1);
    --m_size;
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::popBack(size_t count)
{
    if (empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = m_size < count ? static_cast<size_t>(m_size) : count;
    Pointer curend = m_buffer.data() + m_size;

    memory::destructRange(curend - removeCnt, curend);
    m_size -= count;
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::remove(size_t index)
{
    remove(begin() + static_cast<offset_t>(index));
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::remove(ConstIterator iter)
{
    offset_t locDiff = iter - begin();
    if (static_cast<offset_t>(m_size) == locDiff) {
        popBack();
        return;
    }

    assertValidIterator(iter);
    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + static_cast<offset_t>(m_size);
    Pointer loc = curbegin + locDiff;

    memory::moveRange(loc, loc + 1, curend);
    memory::destructAt(curend - 1);
    --m_size;
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::removeRange(ConstIterator rangeBegin, ConstIterator rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    switch (count) {
        [[unlikely]] case 0:
            return;
        case 1:  remove(rangeBegin); return;
        default: break;
    }

    assertValidRange(rangeBegin, rangeEnd);
    Iterator iterBegin = begin();
    Pointer ptrBegin = m_buffer.data();
    Pointer ptrEnd = ptrBegin + m_size;
    Pointer ptrLoc = ptrBegin + (rangeBegin - iterBegin);

    memory::moveRange(ptrLoc, ptrLoc + count, ptrEnd);
    memory::destructRange(ptrEnd - count, ptrEnd);
    m_size -= count;
}

template <MovableT T, size_t CapacityN>
template <typename... Args>
constexpr void FixedArray<T, CapacityN>::resize(size_t count, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_size == count) [[unlikely]] {
        return;
    }

    assertValidCapacity(count);
    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + m_size;

    if (m_size < count) {
        Value temp(forwardCast<Args>(args)...);
        memory::constructRangeArgs(curend, curbegin + count, temp);
    } else {
        memory::destructRange(curbegin + count, curend);
    }

    m_size = count;
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::clear()
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    Pointer curbegin = m_buffer.data();
    Pointer curend = curbegin + m_size;

    memory::destructRange(curbegin, curend);
    m_size = 0;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::Pointer FixedArray<T, CapacityN>::data() noexcept
{
    return m_buffer.data();
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::ConstPointer FixedArray<T, CapacityN>::data() const noexcept
{
    return m_buffer.data();
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::Iterator FixedArray<T, CapacityN>::begin() noexcept
{
    return Iterator(m_buffer.data(), this);
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::ConstIterator FixedArray<T, CapacityN>::begin() const noexcept
{
    return ConstIterator(m_buffer.data(), this);
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::Iterator FixedArray<T, CapacityN>::end() noexcept
{
    return Iterator(m_buffer.data() + m_size, this);
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>::ConstIterator FixedArray<T, CapacityN>::end() const noexcept
{
    return ConstIterator(m_buffer.data() + m_size, this);
}

template <MovableT T, size_t CapacityN>
constexpr T& FixedArray<T, CapacityN>::first()
{
    return at(0);
}

template <MovableT T, size_t CapacityN>
constexpr T const& FixedArray<T, CapacityN>::first() const
{
    return at(0);
}

template <MovableT T, size_t CapacityN>
constexpr T& FixedArray<T, CapacityN>::last()
{
    return at(m_size - 1);
}

template <MovableT T, size_t CapacityN>
constexpr T const& FixedArray<T, CapacityN>::last() const
{
    return at(m_size - 1);
}

template <MovableT T, size_t CapacityN>
constexpr T& FixedArray<T, CapacityN>::at(size_t index)
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, size_t CapacityN>
constexpr T const& FixedArray<T, CapacityN>::at(size_t index) const
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, size_t CapacityN>
constexpr size_t FixedArray<T, CapacityN>::capacity() const noexcept
{
    return m_buffer.capacity();
}

template <MovableT T, size_t CapacityN>
constexpr size_t FixedArray<T, CapacityN>::size() const noexcept
{
    return m_size.get();
}

template <MovableT T, size_t CapacityN>
constexpr bool FixedArray<T, CapacityN>::empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, size_t CapacityN>
constexpr bool FixedArray<T, CapacityN>::full() const noexcept
{
    return static_cast<size_t>(m_size) == m_buffer.capacity();
}

template <MovableT T, size_t CapacityN>
constexpr bool FixedArray<T, CapacityN>::validIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, size_t CapacityN>
constexpr bool FixedArray<T, CapacityN>::validIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.data());
    return index < m_size;
}

template <MovableT T, size_t CapacityN>
constexpr bool FixedArray<T, CapacityN>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    ConstPointer buffer = m_buffer.data();
    size_t beginIdx = static_cast<size_t>(rangeBegin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(rangeEnd.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, size_t CapacityN>
constexpr T& FixedArray<T, CapacityN>::operator[](size_t index)
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, size_t CapacityN>
constexpr T const& FixedArray<T, CapacityN>::operator[](size_t index) const
{
    assertValidIndex(index);
    return *(m_buffer.data() + index);
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>& FixedArray<T, CapacityN>::operator=(FixedArray const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    assign(other.begin(), other.end());
    return *this;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>& FixedArray<T, CapacityN>::operator=(FixedArray&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    assign(MoveIterator(other.begin()), MoveIterator(other.end()));
    other.clear();
    return *this;
}

template <MovableT T, size_t CapacityN>
constexpr FixedArray<T, CapacityN>& FixedArray<T, CapacityN>::operator=(ArrayView<T> view)
    requires CopyableT<T>
{
    assign(view);
    return *this;
}

template <MovableT T, size_t CapacityN>
template <MovableT U>
constexpr FixedArray<T, CapacityN>::operator ArrayView<U>() const noexcept
    requires ConvertibleToT<T const*, U const*>
{
    return ArrayView<U>(m_buffer, CapacityN);
}

template <MovableT T, size_t CapacityN>
template <typename U>
constexpr void FixedArray<T, CapacityN>::assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    assertValidCapacity(len);

    offset_t count = static_cast<offset_t>(m_size);
    Pointer ptrBegin = m_buffer.data();

    if (m_size < len) {
        memory::copyRange(ptrBegin, rangeBegin, rangeEnd + count);
        memory::constructRange(ptrBegin + count, rangeBegin + count, rangeEnd);
    } else {
        memory::copyRange(ptrBegin, rangeBegin, rangeEnd);
        memory::destructRange(ptrBegin + len, ptrBegin + count);
    }

    m_size = len;
}

template <MovableT T, size_t CapacityN>
template <typename U>
constexpr void FixedArray<T, CapacityN>::appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    size_t newSize = m_size + len;
    assertValidCapacity(newSize);
    memory::constructRange(m_buffer.data() + m_size, rangeBegin, rangeEnd);
    m_size = newSize;
}

template <MovableT T, size_t CapacityN>
template <typename U>
constexpr void FixedArray<T, CapacityN>::insertRangeWithSize(size_t index, U rangeBegin, U rangeEnd, size_t len)
{
    size_t newSize = m_size + len;
    assertValidCapacity(newSize);
    assertValidIndex(index);

    Pointer ptrBegin = m_buffer.data();
    Pointer ptrLoc = ptrBegin + index;
    Pointer ptrEnd = ptrBegin + static_cast<offset_t>(m_size);

    if (static_cast<size_t>(ptrEnd - ptrLoc) > len) {
        memory::moveconstructBackward(ptrEnd + len, ptrEnd - len, ptrEnd);
        memory::moveBackward(ptrEnd, ptrLoc, ptrEnd - len);
        memory::destructRange(ptrLoc, ptrLoc + len);
    } else {
        memory::moveconstructBackward(ptrEnd + len, ptrLoc, ptrEnd);
        memory::destructRange(ptrLoc, ptrEnd);
    }

    m_size = newSize;
    memory::constructRange(ptrLoc, rangeBegin, rangeEnd);
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::assertValidCapacity([[maybe_unused]] size_t count) const noexcept
{
    ASSERT(count <= m_buffer.capacity(), "invalid capacity {}. max capacity is {}", count, CapacityN);
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::assertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(validIndex(index), "invalid index {}. current count is {}", index, m_size.get());
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter), "invalid iterator at index {}", iter.address() - m_buffer.data());
}

template <MovableT T, size_t CapacityN>
constexpr void FixedArray<T, CapacityN>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                                          [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd),
           "invalid range from {} to {}",
           rangeBegin.address() - m_buffer.data(),
           rangeEnd.address() - m_buffer.data());
}

export template <MovableT T, size_t CapT, MovableT U, size_t CapU>
constexpr bool operator==(FixedArray<T, CapT> const& lhs, FixedArray<U, CapU> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    if (lhs.data() == rhs.data()) [[unlikely]] {
        return true;
    }

    if (lhs.size() == 0) [[unlikely]] {
        return true;
    }

    return memory::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace mini