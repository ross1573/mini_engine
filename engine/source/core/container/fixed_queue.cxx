export module mini.core:fixed_queue;

import :type;
import :initializer_list;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :fixed_buffer;
import :array_view;
import :circular_iterator;

namespace mini {

export template <MovableT T, size_t CapacityN>
class FixedQueue {
private:
    typedef memory::FixedBuffer<T, CapacityN> Buffer;
    typedef memory::FixedSize<CapacityN> BufferSize;

    template <MovableT U, size_t CapU>
    friend class FixedQueue;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    typedef CircularIterator<T, FixedQueue> Iterator;
    typedef CircularIterator<T const, FixedQueue const> ConstIterator;

private:
    BufferSize m_begin;
    BufferSize m_end;
    BufferSize m_size;
    Buffer m_buffer;

public:
    constexpr FixedQueue() noexcept;
    constexpr ~FixedQueue();
    constexpr FixedQueue(FixedQueue const& other)
        requires CopyableT<T>;
    constexpr FixedQueue(FixedQueue&& other) noexcept;
    constexpr FixedQueue(ArrayView<T> arr)
        requires CopyableT<T>;
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr FixedQueue(Iter rangeBegin, Iter rangeEnd);

    template <typename... Args>
    constexpr void pushBack(Args&&... args)
        requires ConstructibleFromT<T, Args...>;

    template <ArrayLikeT<T> U>
    constexpr void assign(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void append(U const& arr)
        requires CopyableT<T>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void assign(Iter rangeBegin, Iter rangeEnd);
    template <ForwardIteratableByT<T> Iter>
    constexpr void append(Iter rangeBegin, Iter rangeEnd);

    constexpr Value popFirst();
    constexpr void popFront();
    constexpr void popFront(size_t count);
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

    constexpr FixedQueue& operator=(ArrayView<T> arr)
        requires CopyableT<T>;
    constexpr FixedQueue& operator=(FixedQueue const& other)
        requires CopyableT<T>;
    constexpr FixedQueue& operator=(FixedQueue&& other) noexcept;

private:
    template <typename U>
    constexpr void appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len);
    template <typename U>
    constexpr void assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len);

    constexpr void assertValidCapacity(size_t count) const noexcept;
    constexpr void assertValidOffset(size_t offset) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
};

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue() noexcept
    : m_begin(0)
    , m_end(0)
    , m_size(0)
    , m_buffer()
{
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::~FixedQueue()
{
    clear();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue(FixedQueue const& other)
    requires CopyableT<T>
    : m_buffer()
{
    memory::constructRange(m_buffer.data(), other.begin(), other.end());
    m_begin = 0;
    m_end = other.m_size;
    m_size = other.m_size;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue(FixedQueue&& other) noexcept
    : m_buffer()
{
    Iterator otherBegin = other.begin();
    Iterator otherEnd = other.end();

    memory::moveconstructRange(m_buffer.data(), otherBegin, otherEnd);
    memory::destructRange(otherBegin, otherEnd);
    m_begin = 0;
    m_end = other.m_size;
    m_size = other.m_size;
    other.m_begin = 0;
    other.m_end = 0;
    other.m_size = 0;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue(ArrayView<T> arr)
    requires CopyableT<T>
    : m_buffer()
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t viewSize = view.size();

    assertValidCapacity(viewSize);
    memory::constructRange(m_buffer.data(), ptr, ptr + viewSize);
    m_begin = 0;
    m_end = viewSize;
    m_size = viewSize;
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr FixedQueue<T, N>::FixedQueue(Iter rangeBegin, Iter rangeEnd)
    : m_begin(0)
    , m_end(0)
    , m_size(0)
    , m_buffer()
{
    assign(rangeBegin, rangeEnd);
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedQueue<T, N>::pushBack(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    assertValidCapacity(m_size + 1);
    memory::constructAt(m_buffer.data() + m_end, forwardCast<Args>(args)...);
    m_end = (m_end + 1) % m_buffer.capacity();
    ++m_size;
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr void FixedQueue<T, N>::assign(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t viewSize = view.size();
    if (viewSize == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(ptr, ptr + viewSize, viewSize);
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr void FixedQueue<T, N>::append(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.data();
    size_t viewSize = view.size();
    if (viewSize == 0) [[unlikely]] {
        return;
    }

    appendRangeWithSize(ptr, ptr + viewSize, viewSize);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedQueue<T, N>::assign(Iter rangeBegin, Iter rangeEnd)
{
    size_t count = distance(rangeBegin, rangeEnd);
    if (count == 0) [[unlikely]] {
        clear();
        return;
    }

    assignRangeWithSize(rangeBegin, rangeEnd, count);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedQueue<T, N>::append(Iter rangeBegin, Iter rangeEnd)
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

template <MovableT T, size_t N>
constexpr T FixedQueue<T, N>::popFirst()
{
    Pointer curbegin = m_buffer.data() + m_begin;
    T ele = moveCast(*curbegin);
    memory::destructAt(curbegin);
    m_begin = (m_begin + 1) % m_buffer.capacity();
    --m_size;
    return ele;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::popFront()
{
    if (empty()) [[unlikely]] {
        return;
    }

    memory::destructAt(m_buffer.data() + m_begin);
    m_begin = (m_begin + 1) % m_buffer.capacity();
    --m_size;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::popFront(size_t count)
{
    if (empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t oldSize = size();
    count = count < oldSize ? count : oldSize;

    Pointer ptrBegin = m_buffer.data() + m_begin;
    size_t frontCap = m_buffer.capacity() - m_begin;
    if (frontCap >= count) {
        memory::destructRange(ptrBegin, ptrBegin + count);
        m_begin += count;
    } else {
        Pointer bufBegin = m_buffer.data();
        memory::destructRange(ptrBegin, ptrBegin + frontCap);
        memory::destructRange(bufBegin, bufBegin + count - frontCap);
        m_begin = count - frontCap;
    }

    m_size -= count;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::clear()
{
    if (empty()) [[unlikely]] {
        return;
    }

    Pointer buf = m_buffer.data();
    Pointer ptrBegin = buf + m_begin;
    Pointer ptrEnd = buf + m_end;

    if (m_begin < m_end) {
        memory::destructRange(ptrBegin, ptrEnd);
    } else {
        memory::destructRange(ptrBegin, buf + static_cast<offset_t>(m_buffer.capacity()));
        memory::destructRange(buf, ptrEnd);
    }

    m_begin = 0;
    m_end = 0;
    m_size = 0;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Pointer FixedQueue<T, N>::data() noexcept
{
    return m_buffer.data();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstPointer FixedQueue<T, N>::data() const noexcept
{
    return m_buffer.data();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Iterator FixedQueue<T, N>::begin() noexcept
{
    return Iterator(static_cast<size_t>(m_begin), m_buffer.capacity(), m_buffer.data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstIterator FixedQueue<T, N>::begin() const noexcept
{
    return ConstIterator(static_cast<size_t>(m_begin), m_buffer.capacity(), m_buffer.data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Iterator FixedQueue<T, N>::end() noexcept
{
    size_t cap = m_buffer.capacity();
    size_t endIdx = m_begin == m_end
                        ? m_begin + m_size
                        : (m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + cap);

    return Iterator(endIdx, cap, m_buffer.data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstIterator FixedQueue<T, N>::end() const noexcept
{
    size_t cap = m_buffer.capacity();
    size_t endIdx = m_begin == m_end
                        ? m_begin + m_size
                        : (m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + cap);

    return ConstIterator(endIdx, cap, m_buffer.data(), this);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::first()
{
    assertValidOffset(m_begin);
    return *(m_buffer.data() + m_begin);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::first() const
{
    assertValidOffset(m_begin);
    return *(m_buffer.data() + m_begin);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::last()
{
    size_t cap = m_buffer.capacity();
    size_t offset = (m_end - 1 + cap) % cap;
    assertValidOffset(offset);
    return *(m_buffer.data() + offset);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::last() const
{
    size_t cap = m_buffer.capacity();
    size_t offset = (m_end - 1 + cap) % cap;
    assertValidOffset(offset);
    return *(m_buffer.data() + offset);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::at(size_t index)
{
    size_t offset = (m_begin + index) % m_buffer.capacity();
    assertValidOffset(offset);
    return *(m_buffer.data() + offset);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::at(size_t index) const
{
    size_t offset = (m_begin + index) % m_buffer.capacity();
    assertValidOffset(offset);
    return *(m_buffer.data() + offset);
}

template <MovableT T, size_t N>
constexpr size_t FixedQueue<T, N>::capacity() const noexcept
{
    return m_buffer.capacity();
}

template <MovableT T, size_t N>
constexpr size_t FixedQueue<T, N>::size() const noexcept
{
    return m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::full() const noexcept
{
    return m_size == m_buffer.capacity();
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::validIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::validIterator(ConstIterator iter) const noexcept
{
    size_t beginidx = static_cast<size_t>(m_begin);
    size_t endidx = m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + m_buffer.capacity();
    return iter.m_offset >= beginidx && iter.m_offset < endidx;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    size_t bufferBegin = static_cast<size_t>(m_begin);
    size_t bufferEnd = m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + m_buffer.capacity();
    return (rangeBegin.m_offset >= bufferBegin && rangeBegin.m_offset < bufferEnd) &&
           (rangeEnd.m_offset > bufferBegin && rangeEnd.m_offset < bufferEnd + 1);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::operator[](size_t index)
{
    return *(m_buffer.data() + ((m_begin + index) % m_buffer.capacity()));
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::operator[](size_t index) const
{
    return *(m_buffer.data() + ((m_begin + index) % m_buffer.capacity()));
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(FixedQueue const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    assign(other.begin(), other.end());
    return *this;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(FixedQueue&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    assign(MoveIterator(other.begin()), MoveIterator(other.end()));
    other.clear();
    return *this;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(ArrayView<T> arr)
    requires CopyableT<T>
{
    assign(arr);
    return *this;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedQueue<T, N>::appendRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    assertValidCapacity(m_size + len);
    Pointer ptrEnd = m_buffer.data() + m_end;
    size_t backCap = m_buffer.capacity() - m_end;

    if (backCap >= len) {
        memory::constructRange(ptrEnd, rangeBegin, rangeEnd);
        m_end += len;
    } else {
        size_t frontInsertCnt = len - backCap;
        Pointer ptrBegin = m_buffer.data();

        memory::constructRange(ptrEnd, rangeBegin, rangeBegin + static_cast<offset_t>(backCap));
        memory::constructRange(ptrBegin, rangeBegin + static_cast<offset_t>(backCap), rangeEnd);
        m_end = frontInsertCnt;
    }

    m_size += len;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedQueue<T, N>::assignRangeWithSize(U rangeBegin, U rangeEnd, size_t len)
{
    assertValidCapacity(len);
    offset_t offset = static_cast<offset_t>(m_size);
    Iterator iterBegin = begin();

    if (len > m_size) {
        memory::copyRange(iterBegin, rangeBegin, rangeBegin + offset);
        memory::constructRange(iterBegin + offset, rangeBegin + offset, rangeEnd);
    } else {
        memory::copyRange(iterBegin, rangeBegin, rangeEnd);
        memory::destructRange(iterBegin + static_cast<offset_t>(len), end());
    }

    m_end = (m_begin + len) % m_buffer.capacity();
    m_size = len;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::assertValidCapacity([[maybe_unused]] size_t count) const noexcept
{
    ASSERT(count <= m_buffer.capacity(), "invalid capacity {}. max capacity is {}", count, N);
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::assertValidOffset([[maybe_unused]] size_t offset) const noexcept
{
    ASSERT(m_size != 0, "invalid access on empty queue");

    if (m_begin < m_end) {
        ASSERT(offset >= m_begin && offset < m_end,
               "invalid offset {}. current: {}/{}",
               offset,
               m_begin.get(),
               m_end.get());
    } else {
        ASSERT(offset < m_end || offset >= m_begin && offset < m_buffer.capacity(),
               "invalid offset {}. current: {}/{}",
               offset,
               m_begin.get(),
               m_end.get());
    }
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter), "invalid iterator at offset {}", iter.m_offset);
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                                  [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd), "invalid range from {} to {}", rangeBegin.m_offset, rangeEnd.m_offset);
}

export template <MovableT T, size_t CapT, MovableT U, size_t CapU>
constexpr bool operator==(FixedQueue<T, CapT> const& lhs, FixedQueue<U, CapU> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    if (lhs.size() == 0) [[unlikely]] {
        return true;
    }

    return memory::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace mini