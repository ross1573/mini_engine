export module mini.core:static_queue;

import :type;
import :initializer_list;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :static_buffer;
import :circular_iterator;

namespace mini {

export template <MovableT T, SizeT CapacityN>
class StaticQueue {
private:
    typedef memory::StaticBuffer<T, CapacityN> Buffer;
    typedef memory::StaticSize<CapacityN> BufferSize;

    template <MovableT U, SizeT CapU>
    friend class StaticQueue;

public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    typedef CircularIterator<T, StaticQueue> Iterator;
    typedef CircularIterator<T const, StaticQueue const> ConstIterator;

private:
    BufferSize m_begin;
    BufferSize m_end;
    BufferSize m_size;
    Buffer m_buffer;

public:
    constexpr StaticQueue() noexcept;
    constexpr ~StaticQueue();
    constexpr StaticQueue(StaticQueue const&);
    constexpr StaticQueue(StaticQueue&&) noexcept;
    constexpr StaticQueue(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr StaticQueue(Iter, Iter);

    template <typename... Args>
    constexpr void Enqueue(Args&&...)
        requires ConstructibleFromT<T, Args...>;
    template <ForwardIteratableByT<T> Iter>
    constexpr void EnqueueRange(Iter, Iter);
    constexpr void EnqueueRange(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    constexpr void Assign(InitializerList<T>);
    constexpr T Dequeue();
    constexpr void RemoveFirst();
    constexpr void RemoveFirst(SizeT);
    constexpr void Clear();

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

    constexpr SizeT Capacity() const noexcept;
    constexpr SizeT Size() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool Full() const noexcept;
    constexpr bool ValidIndex(SizeT) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Reference operator[](SizeT);
    constexpr ConstReference operator[](SizeT) const;

    constexpr StaticQueue& operator=(StaticQueue const&);
    constexpr StaticQueue& operator=(StaticQueue&&) noexcept;
    constexpr StaticQueue& operator=(InitializerList<T>);

private:
    template <typename U>
    constexpr void EnqueueRangeWithSize(U, U, SizeT);
    template <typename U>
    constexpr void AssignRangeWithSize(U, U, SizeT);

    constexpr void AssertValidCapacity(SizeT) const noexcept;
    constexpr void AssertValidOffset(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue() noexcept
    : m_begin(0)
    , m_end(0)
    , m_size(0)
    , m_buffer()
{
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::~StaticQueue()
{
    Clear();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue(StaticQueue const& other)
    : m_buffer()
{
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_begin = 0;
    m_end = other.m_size;
    m_size = other.m_size;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue(StaticQueue&& other) noexcept
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

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue(InitializerList<T> init)
    : m_buffer()
{
    AssertValidCapacity(init.size());
    memory::ConstructRange(m_buffer.Data(), init.begin(), init.end());
    m_begin = 0;
    m_end = init.size();
    m_size = init.size();
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr StaticQueue<T, N>::StaticQueue(Iter first, Iter last)
    : m_begin(0)
    , m_end(0)
    , m_size(0)
    , m_buffer()
{
    Assign(first, last);
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticQueue<T, N>::Enqueue(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_size + 1);
    memory::ConstructAt(m_buffer.Data() + m_end, ForwardArg<Args>(args)...);
    m_end = (m_end + 1) % m_buffer.Capacity();
    ++m_size;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void StaticQueue<T, N>::EnqueueRange(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Enqueue(ForwardArg<typename Iter::Value>(*first)); return;
        default: break;
    }

    EnqueueRangeWithSize(first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticQueue<T, N>::EnqueueRange(InitializerList<T> init)
{
    EnqueueRangeWithSize(init.begin(), init.end(), init.size());
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void StaticQueue<T, N>::Assign(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticQueue<T, N>::Assign(InitializerList<T> init)
{
    SizeT size = init.size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(init.begin(), init.end(), size);
}

template <MovableT T, SizeT N>
inline constexpr T StaticQueue<T, N>::Dequeue()
{
    Pointer begin = m_buffer.Data() + m_begin;
    T ele = MoveArg(*begin);
    memory::DestructAt(begin);
    m_begin = (m_begin + 1) % m_buffer.Capacity();
    --m_size;
    return ele;
}

template <MovableT T, SizeT N>
inline constexpr void StaticQueue<T, N>::RemoveFirst()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_begin);
    m_begin = (m_begin + 1) % m_buffer.Capacity();
    --m_size;
}

template <MovableT T, SizeT N>
constexpr void StaticQueue<T, N>::RemoveFirst(SizeT count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    SizeT oldSize = Size();
    if (count >= oldSize) {
        count = oldSize;
    }

    Pointer begin = m_buffer.Data() + m_begin;
    SizeT frontCap = m_buffer.Capacity() - m_begin;
    if (frontCap >= count) {
        memory::DestructRange(begin, begin + count);
        m_begin += count;
    } else {
        Pointer bufBegin = m_buffer.Data();
        memory::DestructRange(begin, begin + frontCap);
        memory::DestructRange(bufBegin, bufBegin + count - frontCap);
        m_begin = count - frontCap;
    }

    m_size -= count;
}

template <MovableT T, SizeT N>
constexpr void StaticQueue<T, N>::Clear()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    Pointer buf = m_buffer.Data();
    Pointer begin = buf + m_begin;
    Pointer end = buf + m_end;

    if (m_begin < m_end) {
        memory::DestructRange(begin, end);
    } else {
        memory::DestructRange(begin, buf + (OffsetT)m_buffer.Capacity());
        memory::DestructRange(buf, end);
    }

    m_begin = 0;
    m_end = 0;
    m_size = 0;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Pointer StaticQueue<T, N>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstPointer StaticQueue<T, N>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Iterator StaticQueue<T, N>::Begin() noexcept
{
    return Iterator((SizeT)m_begin, m_buffer.Capacity(), m_buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstIterator StaticQueue<T, N>::Begin() const noexcept
{
    return ConstIterator((SizeT)m_begin, m_buffer.Capacity(), m_buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Iterator StaticQueue<T, N>::End() noexcept
{
    SizeT cap = m_buffer.Capacity();
    SizeT endIdx = m_begin == m_end  ? m_begin + m_size
                   : m_begin < m_end ? (SizeT)m_end
                                     : (SizeT)m_end + cap;

    return Iterator(endIdx, cap, m_buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstIterator StaticQueue<T, N>::End() const noexcept
{
    SizeT cap = m_buffer.Capacity();
    SizeT endIdx = m_begin == m_end  ? m_begin + m_size
                   : m_begin < m_end ? (SizeT)m_end
                                     : (SizeT)m_end + cap;

    return ConstIterator(endIdx, cap, m_buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::First()
{
    AssertValidOffset(m_begin);
    return *(m_buffer.Data() + m_begin);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::First() const
{
    AssertValidOffset(m_begin);
    return *(m_buffer.Data() + m_begin);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::Last()
{
    SizeT cap = m_buffer.Capacity();
    SizeT offset = (m_end - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::Last() const
{
    SizeT cap = m_buffer.Capacity();
    SizeT offset = (m_end - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::At(SizeT index)
{
    SizeT offset = (m_begin + index) % m_buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::At(SizeT index) const
{
    SizeT offset = (m_begin + index) % m_buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr SizeT StaticQueue<T, N>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr SizeT StaticQueue<T, N>::Size() const noexcept
{
    return m_size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::Full() const noexcept
{
    return m_size == m_buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::ValidIndex(SizeT index) const noexcept
{
    return index < m_size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::ValidIterator(ConstIterator iter) const noexcept
{
    SizeT begin = (SizeT)m_begin;
    SizeT end = m_begin < m_end ? (SizeT)m_end : (SizeT)m_end + m_buffer.Capacity();
    return iter.m_offset >= begin && iter.m_offset < end;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::ValidRange(ConstIterator begin,
                                                    ConstIterator end) const noexcept
{
    SizeT bufferBegin = (SizeT)m_begin;
    SizeT bufferEnd = m_begin < m_end ? (SizeT)m_end : (SizeT)m_end + m_buffer.Capacity();
    return (begin.m_offset >= bufferBegin && begin.m_offset < bufferEnd) &&
           (end.m_offset > bufferBegin && end.m_offset < bufferEnd + 1);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::operator[](SizeT index)
{
    return *(m_buffer.Data() + ((m_begin + index) % m_buffer.Capacity()));
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::operator[](SizeT index) const
{
    return *(m_buffer.Data() + ((m_begin + index) % m_buffer.Capacity()));
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>& StaticQueue<T, N>::operator=(StaticQueue const& other)
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>& StaticQueue<T, N>::operator=(StaticQueue&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>& StaticQueue<T, N>::operator=(InitializerList<T> init)
{
    Assign(init);
    return *this;
}

template <MovableT T, SizeT N>
template <typename U>
inline constexpr void StaticQueue<T, N>::EnqueueRangeWithSize(U first, U last, SizeT len)
{
    AssertValidCapacity(m_size + len);
    Pointer end = m_buffer.Data() + m_end;
    SizeT backCap = m_buffer.Capacity() - m_end;

    if (backCap >= len) {
        memory::ConstructRange(end, first, last);
        m_end += len;
    } else {
        SizeT frontInsertCnt = len - backCap;
        Pointer begin = m_buffer.Data();

        memory::ConstructRange(end, first, first + (OffsetT)backCap);
        memory::ConstructRange(begin, first + (OffsetT)backCap, last);
        m_end = frontInsertCnt;
    }

    m_size += len;
}

template <MovableT T, SizeT N>
template <typename U>
inline constexpr void StaticQueue<T, N>::AssignRangeWithSize(U first, U last, SizeT len)
{
    AssertValidCapacity(len);
    OffsetT size = (OffsetT)m_size;
    Iterator begin = Begin();

    if (len > m_size) {
        memory::CopyRange(begin, first, first + size);
        memory::ConstructRange(begin + size, first + size, last);
    } else {
        memory::CopyRange(begin, first, last);
        memory::DestructRange(begin + (OffsetT)len, End());
    }

    m_end = (m_begin + len) % m_buffer.Capacity();
    m_size = len;
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidCapacity([[maybe_unused]] SizeT cap) const noexcept
{
    ASSERT(cap <= m_buffer.Capacity(), "invalid capacity");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidOffset([[maybe_unused]] SizeT offset) const noexcept
{
    ASSERT(m_size != 0, "invalid access on empty queue");

    if (m_begin < m_end) {
        ASSERT(offset >= m_begin && offset < m_end, "invalid offset");
    } else {
        ASSERT(offset < m_end || offset >= m_begin && offset < m_buffer.Capacity(),
               "invalid offset");
    }
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                    [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end), "invalid range");
}

export template <MovableT T, SizeT CapT, MovableT U, SizeT CapU>
inline constexpr bool operator==(StaticQueue<T, CapT> const& l,
                                 StaticQueue<U, CapU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (l.Size() != r.Size()) {
        return false;
    } else if (l.Size() == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(l.Begin(), l.End(), r.Begin(), r.End());
}

} // namespace mini