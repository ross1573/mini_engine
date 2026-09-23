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
    template <ArrayLikeT<T> U>
    constexpr FixedQueue(U const& arr)
        requires CopyableT<T>;
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr FixedQueue(Iter begin, Iter end);

    template <typename... Args>
    constexpr void PushBack(Args&&... args)
        requires ConstructibleFromT<T, Args...>;

    template <ArrayLikeT<T> U>
    constexpr void Assign(U const& arr)
        requires CopyableT<T>;
    template <ArrayLikeT<T> U>
    constexpr void Append(U const& arr)
        requires CopyableT<T>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter begin, Iter end);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter begin, Iter end);

    constexpr Value PopFirst();
    constexpr void PopFront();
    constexpr void PopFront(size_t count);
    constexpr void Clear();

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

    [[nodiscard]] constexpr size_t Capacity() const noexcept;
    [[nodiscard]] constexpr size_t Size() const noexcept;
    [[nodiscard]] constexpr bool Empty() const noexcept;
    [[nodiscard]] constexpr bool Full() const noexcept;
    [[nodiscard]] constexpr bool ValidIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool ValidIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool ValidRange(ConstIterator begin, ConstIterator end) const noexcept;

    [[nodiscard]] constexpr Reference operator[](size_t index);
    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    constexpr FixedQueue& operator=(FixedQueue const& other)
        requires CopyableT<T>;
    constexpr FixedQueue& operator=(FixedQueue&& other) noexcept;
    template <ArrayLikeT<T> U>
    constexpr FixedQueue& operator=(U const& arr)
        requires CopyableT<T>;

private:
    template <typename U>
    constexpr void AppendRangeWithSize(U begin, U end, size_t len);
    template <typename U>
    constexpr void AssignRangeWithSize(U begin, U end, size_t len);

    constexpr void AssertValidCapacity(size_t capacity) const noexcept;
    constexpr void AssertValidOffset(size_t offset) const noexcept;
    constexpr void AssertValidIterator(ConstIterator iter) const noexcept;
    constexpr void AssertValidRange(ConstIterator begin, ConstIterator end) const noexcept;
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
    Clear();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue(FixedQueue const& other)
    requires CopyableT<T>
    : m_buffer()
{
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_begin = 0;
    m_end = other.m_size;
    m_size = other.m_size;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::FixedQueue(FixedQueue&& other) noexcept
    : m_buffer()
{
    Iterator otherBegin = other.Begin();
    Iterator otherEnd = other.End();

    memory::MoveConstructRange(m_buffer.Data(), otherBegin, otherEnd);
    memory::DestructRange(otherBegin, otherEnd);
    m_begin = 0;
    m_end = other.m_size;
    m_size = other.m_size;
    other.m_begin = 0;
    other.m_end = 0;
    other.m_size = 0;
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr FixedQueue<T, N>::FixedQueue(U const& arr)
    requires CopyableT<T>
    : m_buffer()
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.Data();
    size_t size = view.Size();

    AssertValidCapacity(size);
    memory::ConstructRange(m_buffer.Data(), ptr, ptr + size);
    m_begin = 0;
    m_end = size;
    m_size = size;
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr FixedQueue<T, N>::FixedQueue(Iter begin, Iter end)
    : m_begin(0)
    , m_end(0)
    , m_size(0)
    , m_buffer()
{
    Assign(begin, end);
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedQueue<T, N>::PushBack(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_size + 1);
    memory::ConstructAt(m_buffer.Data() + m_end, ForwardArg<Args>(args)...);
    m_end = (m_end + 1) % m_buffer.Capacity();
    ++m_size;
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr void FixedQueue<T, N>::Assign(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.Data();
    size_t size = view.Size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(ptr, ptr + size, size);
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr void FixedQueue<T, N>::Append(U const& arr)
    requires CopyableT<T>
{
    ArrayView<T> view = arr;
    ConstPointer ptr = view.Data();
    size_t size = view.Size();
    if (size == 0) [[unlikely]] {
        return;
    }

    AppendRangeWithSize(ptr, ptr + size, size);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedQueue<T, N>::Assign(Iter begin, Iter end)
{
    size_t distance = Distance(begin, end);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(begin, end, distance);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedQueue<T, N>::Append(Iter begin, Iter end)
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

template <MovableT T, size_t N>
constexpr T FixedQueue<T, N>::PopFirst()
{
    Pointer begin = m_buffer.Data() + m_begin;
    T ele = MoveArg(*begin);
    memory::DestructAt(begin);
    m_begin = (m_begin + 1) % m_buffer.Capacity();
    --m_size;
    return ele;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::PopFront()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_begin);
    m_begin = (m_begin + 1) % m_buffer.Capacity();
    --m_size;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::PopFront(size_t count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t oldSize = Size();
    count = count < oldSize ? count : oldSize;

    Pointer ptrBegin = m_buffer.Data() + m_begin;
    size_t frontCap = m_buffer.Capacity() - m_begin;
    if (frontCap >= count) {
        memory::DestructRange(ptrBegin, ptrBegin + count);
        m_begin += count;
    } else {
        Pointer bufBegin = m_buffer.Data();
        memory::DestructRange(ptrBegin, ptrBegin + frontCap);
        memory::DestructRange(bufBegin, bufBegin + count - frontCap);
        m_begin = count - frontCap;
    }

    m_size -= count;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::Clear()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    Pointer buf = m_buffer.Data();
    Pointer ptrBegin = buf + m_begin;
    Pointer ptrEnd = buf + m_end;

    if (m_begin < m_end) {
        memory::DestructRange(ptrBegin, ptrEnd);
    } else {
        memory::DestructRange(ptrBegin, buf + static_cast<offset_t>(m_buffer.Capacity()));
        memory::DestructRange(buf, ptrEnd);
    }

    m_begin = 0;
    m_end = 0;
    m_size = 0;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Pointer FixedQueue<T, N>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstPointer FixedQueue<T, N>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Iterator FixedQueue<T, N>::Begin() noexcept
{
    return Iterator(static_cast<size_t>(m_begin), m_buffer.Capacity(), m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstIterator FixedQueue<T, N>::Begin() const noexcept
{
    return ConstIterator(static_cast<size_t>(m_begin), m_buffer.Capacity(), m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::Iterator FixedQueue<T, N>::End() noexcept
{
    size_t cap = m_buffer.Capacity();
    size_t endIdx = m_begin == m_end
                        ? m_begin + m_size
                        : (m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + cap);

    return Iterator(endIdx, cap, m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>::ConstIterator FixedQueue<T, N>::End() const noexcept
{
    size_t cap = m_buffer.Capacity();
    size_t endIdx = m_begin == m_end
                        ? m_begin + m_size
                        : (m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + cap);

    return ConstIterator(endIdx, cap, m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::First()
{
    AssertValidOffset(m_begin);
    return *(m_buffer.Data() + m_begin);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::First() const
{
    AssertValidOffset(m_begin);
    return *(m_buffer.Data() + m_begin);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::Last()
{
    size_t cap = m_buffer.Capacity();
    size_t offset = (m_end - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::Last() const
{
    size_t cap = m_buffer.Capacity();
    size_t offset = (m_end - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::At(size_t index)
{
    size_t offset = (m_begin + index) % m_buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::At(size_t index) const
{
    size_t offset = (m_begin + index) % m_buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, size_t N>
constexpr size_t FixedQueue<T, N>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, size_t N>
constexpr size_t FixedQueue<T, N>::Size() const noexcept
{
    return m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::Full() const noexcept
{
    return m_size == m_buffer.Capacity();
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t begin = static_cast<size_t>(m_begin);
    size_t end = m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + m_buffer.Capacity();
    return iter.m_offset >= begin && iter.m_offset < end;
}

template <MovableT T, size_t N>
constexpr bool FixedQueue<T, N>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    size_t bufferBegin = static_cast<size_t>(m_begin);
    size_t bufferEnd = m_begin < m_end ? static_cast<size_t>(m_end) : static_cast<size_t>(m_end) + m_buffer.Capacity();
    return (begin.m_offset >= bufferBegin && begin.m_offset < bufferEnd) &&
           (end.m_offset > bufferBegin && end.m_offset < bufferEnd + 1);
}

template <MovableT T, size_t N>
constexpr T& FixedQueue<T, N>::operator[](size_t index)
{
    return *(m_buffer.Data() + ((m_begin + index) % m_buffer.Capacity()));
}

template <MovableT T, size_t N>
constexpr T const& FixedQueue<T, N>::operator[](size_t index) const
{
    return *(m_buffer.Data() + ((m_begin + index) % m_buffer.Capacity()));
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(FixedQueue const& other)
    requires CopyableT<T>
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, size_t N>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(FixedQueue&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, size_t N>
template <ArrayLikeT<T> U>
constexpr FixedQueue<T, N>& FixedQueue<T, N>::operator=(U const& arr)
    requires CopyableT<T>
{
    Assign(arr);
    return *this;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedQueue<T, N>::AppendRangeWithSize(U begin, U end, size_t len)
{
    AssertValidCapacity(m_size + len);
    Pointer ptrEnd = m_buffer.Data() + m_end;
    size_t backCap = m_buffer.Capacity() - m_end;

    if (backCap >= len) {
        memory::ConstructRange(ptrEnd, begin, end);
        m_end += len;
    } else {
        size_t frontInsertCnt = len - backCap;
        Pointer ptrBegin = m_buffer.Data();

        memory::ConstructRange(ptrEnd, begin, begin + static_cast<offset_t>(backCap));
        memory::ConstructRange(ptrBegin, begin + static_cast<offset_t>(backCap), end);
        m_end = frontInsertCnt;
    }

    m_size += len;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedQueue<T, N>::AssignRangeWithSize(U begin, U end, size_t len)
{
    AssertValidCapacity(len);
    offset_t size = static_cast<offset_t>(m_size);
    Iterator iterBegin = Begin();

    if (len > m_size) {
        memory::CopyRange(iterBegin, begin, begin + size);
        memory::ConstructRange(iterBegin + size, begin + size, end);
    } else {
        memory::CopyRange(iterBegin, begin, end);
        memory::DestructRange(iterBegin + static_cast<offset_t>(len), End());
    }

    m_end = (m_begin + len) % m_buffer.Capacity();
    m_size = len;
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::AssertValidCapacity([[maybe_unused]] size_t capacity) const noexcept
{
    ASSERT(capacity <= m_buffer.Capacity(), "invalid capacity {}. max capacity is {}", capacity, N);
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::AssertValidOffset([[maybe_unused]] size_t offset) const noexcept
{
    ASSERT(m_size != 0, "invalid access on empty queue");

    if (m_begin < m_end) {
        ASSERT(offset >= m_begin && offset < m_end,
               "invalid offset {}. current: {}/{}",
               offset,
               m_begin.Get(),
               m_end.Get());
    } else {
        ASSERT(offset < m_end || offset >= m_begin && offset < m_buffer.Capacity(),
               "invalid offset {}. current: {}/{}",
               offset,
               m_begin.Get(),
               m_end.Get());
    }
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at offset {}", iter.m_offset);
}

template <MovableT T, size_t N>
constexpr void FixedQueue<T, N>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                  [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end), "invalid range from {} to {}", begin.m_offset, end.m_offset);
}

export template <MovableT T, size_t CapT, MovableT U, size_t CapU>
constexpr bool operator==(FixedQueue<T, CapT> const& lhs, FixedQueue<U, CapU> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.Size() != rhs.Size()) {
        return false;
    }

    if (lhs.Size() == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End());
}

} // namespace mini