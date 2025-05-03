module;

#include "assertion.h"

export module mini.core:static_queue;

import :type;
import :utility;
import :memory;
import :algorithm;
import :static_buffer;
import :array_iterator;
import :circular_iterator;

export namespace mini {

template <MovableT T, SizeT CapacityN>
class StaticQueue {
private:
    typedef StaticBuffer<T, CapacityN> Buffer;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;
    typedef CircularIterator<T, StaticQueue> Iterator;
    typedef CircularIterator<T const, StaticQueue const> ConstIterator;

private:
    StaticSize<CapacityN> m_Begin;
    StaticSize<CapacityN> m_End;
    StaticSize<CapacityN> m_Size;
    Buffer m_Buffer;

public:
    constexpr StaticQueue() noexcept;
    constexpr ~StaticQueue();
    constexpr StaticQueue(StaticQueue const&);
    constexpr StaticQueue(StaticQueue&&) noexcept;
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr StaticQueue(Iter, Iter);

    template <typename... Args>
    constexpr void Enqueue(Args&&...)
        requires ConstructibleFromT<T, Args...>;
    template <ForwardIteratableByT<T> Iter>
    constexpr void EnqueueRange(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    constexpr T Dequeue();
    constexpr void RemoveFirst();
    constexpr void RemoveFirst(SizeT);
    constexpr void Clear();

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

    constexpr SizeT Capacity() const noexcept;
    constexpr SizeT Size() const noexcept;
    constexpr bool IsEmpty() const noexcept;
    constexpr bool IsFull() const noexcept;
    constexpr bool IsValidIndex(SizeT) const noexcept;
    constexpr bool IsValidIterator(ConstIterator) const noexcept;

    constexpr Ref operator[](SizeT);
    constexpr ConstRef operator[](SizeT) const;

    template <EqualityComparableWithT<T> U, SizeT OtherN>
    constexpr bool operator==(StaticQueue<U, OtherN> const&) const;

    constexpr StaticQueue& operator=(StaticQueue const&);
    constexpr StaticQueue& operator=(StaticQueue&&) noexcept;

private:
    constexpr void AssertValidCapacity(SizeT) const noexcept;
    constexpr void AssertValidOffset(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
};

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue() noexcept
    : m_Begin(0)
    , m_End(0)
    , m_Size(0)
    , m_Buffer()
{
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::~StaticQueue()
{
    Clear();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue(StaticQueue const& other)
    : m_Begin(0)
    , m_End(0)
    , m_Size(0)
    , m_Buffer()
{
    memory::ConstructRange(m_Buffer.Data(), other.Begin(), other.End());
    m_Begin = other.m_Begin;
    m_End = other.m_End;
    m_Size = other.m_Size;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::StaticQueue(StaticQueue&& other) noexcept
    : m_Begin(0)
    , m_End(0)
    , m_Size(0)
    , m_Buffer()
{
    Ptr otherBegin = other.m_Buffer.Data() + other.m_Begin;
    Ptr otherEnd = other.m_Buffer.Data() + other.m_End;

    memory::MoveConstructRange(m_Buffer.Data(), otherBegin, otherEnd);
    memory::DestructRange(otherBegin, otherEnd);
    m_Begin = other.m_Begin;
    m_End = other.m_End;
    m_Size = other.m_Size;
    other.m_Begin = 0;
    other.m_End = 0;
    other.m_Size = 0;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr StaticQueue<T, N>::StaticQueue(Iter first, Iter last)
    : m_Begin(0)
    , m_End(0)
    , m_Size(0)
    , m_Buffer()
{
    Assign(first, last);
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticQueue<T, N>::Enqueue(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_Size + 1);
    memory::ConstructAt(m_Buffer.Data() + m_End, ForwardArg<T>(args)...);
    m_End = (m_End + 1) % m_Buffer.Capacity();
    ++m_Size;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticQueue<T, N>::EnqueueRange(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:  Enqueue(ForwardArg<typename Iter::Value>(*first)); return;
        default: break;
    }

    AssertValidCapacity(m_Size + distance);

    Ptr end = m_Buffer.Data() + m_End;
    SizeT backCap = m_Buffer.Capacity() - m_End;
    if (backCap >= distance) {
        memory::ConstructRange(end, first, last);
        m_End += distance;
    }
    else {
        SizeT frontInsertCnt = distance - backCap;
        Ptr begin = m_Buffer.Data();

        memory::ConstructRange(end, first, first + (OffsetT)backCap);
        memory::ConstructRange(begin, first + (OffsetT)backCap, last);
        m_End = frontInsertCnt;
    }

    m_Size += distance;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticQueue<T, N>::Assign(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssertValidCapacity(distance);
    OffsetT size = (OffsetT)m_Size;
    Iterator begin = Begin();

    if (distance > m_Size) {
        memory::CopyRange(begin, first, first + size);
        memory::ConstructRange(begin + size, first + size, last);
    }
    else {
        memory::CopyRange(begin, first, last);
        memory::DestructRange(begin + (OffsetT)distance, End());
    }

    m_End = (m_Begin + distance) % m_Buffer.Capacity();
    m_Size = distance;
}

template <MovableT T, SizeT N>
inline constexpr T StaticQueue<T, N>::Dequeue()
{
    Ptr begin = m_Buffer.Data() + m_Begin;
    T ele = MoveArg(*begin);
    memory::DestructAt(begin);
    m_Begin = (m_Begin + 1) % m_Buffer.Capacity();
    --m_Size;
    return ele;
}

template <MovableT T, SizeT N>
inline constexpr void StaticQueue<T, N>::RemoveFirst()
{
    if (IsEmpty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_Buffer.Data() + m_Begin);
    m_Begin = (m_Begin + 1) % m_Buffer.Capacity();
    --m_Size;
}

template <MovableT T, SizeT N>
constexpr void StaticQueue<T, N>::RemoveFirst(SizeT count)
{
    if (IsEmpty() || count == 0) [[unlikely]] {
        return;
    }

    SizeT oldSize = Size();
    if (count >= oldSize) {
        count = oldSize;
    }

    Ptr begin = m_Buffer.Data() + m_Begin;
    SizeT frontCap = m_Buffer.Capacity() - m_Begin;
    if (frontCap >= count) {
        memory::DestructRange(begin, begin + count);
        m_Begin += count;
    }
    else {
        Ptr bufBegin = m_Buffer.Data();
        memory::DestructRange(begin, begin + frontCap);
        memory::DestructRange(bufBegin, bufBegin + count - frontCap);
        m_Begin = count - frontCap;
    }

    m_Size -= count;
}

template <MovableT T, SizeT N>
constexpr void StaticQueue<T, N>::Clear()
{
    if (IsEmpty()) [[unlikely]] {
        return;
    }

    Ptr buf = m_Buffer.Data();
    Ptr begin = buf + m_Begin;
    Ptr end = buf + m_End;

    if (m_Begin < m_End) {
        memory::DestructRange(begin, end);
    }
    else {
        memory::DestructRange(begin, buf + (OffsetT)m_Buffer.Capacity());
        memory::DestructRange(buf, end);
    }

    m_Begin = 0;
    m_End = 0;
    m_Size = 0;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Ptr StaticQueue<T, N>::Data() noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstPtr StaticQueue<T, N>::Data() const noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Iterator StaticQueue<T, N>::Begin() noexcept
{
    return Iterator((SizeT)m_Begin, m_Buffer.Capacity(), m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstIterator StaticQueue<T, N>::Begin() const noexcept
{
    return ConstIterator((SizeT)m_Begin, m_Buffer.Capacity(), m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::Iterator StaticQueue<T, N>::End() noexcept
{
    SizeT cap = m_Buffer.Capacity();
    SizeT endIdx = m_Begin < m_End ? (SizeT)m_End : (SizeT)m_End + cap;
    return Iterator(endIdx, cap, m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>::ConstIterator StaticQueue<T, N>::End() const noexcept
{
    SizeT cap = m_Buffer.Capacity();
    SizeT endIdx = m_Begin < m_End ? (SizeT)m_End : (SizeT)m_End + cap;
    return ConstIterator(endIdx, cap, m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::First()
{
    AssertValidOffset(m_Begin);
    return *(m_Buffer.Data() + m_Begin);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::First() const
{
    AssertValidOffset(m_Begin);
    return *(m_Buffer.Data() + m_Begin);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::Last()
{
    SizeT cap = m_Buffer.Capacity();
    SizeT offset = (m_End - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_Buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::Last() const
{
    SizeT cap = m_Buffer.Capacity();
    SizeT offset = (m_End - 1 + cap) % cap;
    AssertValidOffset(offset);
    return *(m_Buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::At(SizeT index)
{
    SizeT offset = (m_Begin + index) % m_Buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_Buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::At(SizeT index) const
{
    SizeT offset = (m_Begin + index) % m_Buffer.Capacity();
    AssertValidOffset(offset);
    return *(m_Buffer.Data() + offset);
}

template <MovableT T, SizeT N>
inline constexpr SizeT StaticQueue<T, N>::Capacity() const noexcept
{
    return m_Buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr SizeT StaticQueue<T, N>::Size() const noexcept
{
    return m_Size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::IsEmpty() const noexcept
{
    return m_Size == 0;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::IsFull() const noexcept
{
    return m_Size == m_Buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::IsValidIndex(SizeT index) const noexcept
{
    return index < m_Size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticQueue<T, N>::IsValidIterator(ConstIterator iter) const noexcept
{
    SizeT begin = (SizeT)m_Begin;
    SizeT end = m_Begin < m_End ? (SizeT)m_End : (SizeT)m_End + m_Buffer.Capacity();
    return iter.m_Offset >= begin && iter.m_Offset < end;
}

template <MovableT T, SizeT N>
inline constexpr T& StaticQueue<T, N>::operator[](SizeT index)
{
    return *(m_Buffer.Data() + ((m_Begin + index) % m_Buffer.Capacity()));
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticQueue<T, N>::operator[](SizeT index) const
{
    return *(m_Buffer.Data() + ((m_Begin + index) % m_Buffer.Capacity()));
}

template <MovableT T, SizeT N>
template <EqualityComparableWithT<T> U, SizeT OtherN>
inline constexpr bool StaticQueue<T, N>::operator==(StaticQueue<U, OtherN> const& other) const
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return true;
    }
    else if (m_Size != other.m_Size) {
        return false;
    }
    else if (m_Size == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(Begin(), other.Begin(), other.End());
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>& StaticQueue<T, N>::operator=(StaticQueue const& other)
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr StaticQueue<T, N>& StaticQueue<T, N>::operator=(StaticQueue&& other) noexcept
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidCapacity([[maybe_unused]] SizeT cap) const noexcept
{
    ASSERT(cap <= m_Buffer.Capacity(), "invalid capacity");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidOffset([[maybe_unused]] SizeT offset) const noexcept
{
    ASSERT(m_Size != 0, "invalid access on empty queue");

    [[maybe_unused]] bool isValid = false;
    if (m_Begin < m_End) {
        isValid = offset > m_Begin && offset < m_End;
    }
    else {
        isValid = (offset >= 0 && offset < m_End) ||
                  (offset >= m_Begin && offset < m_Buffer.Capacity());
    }

    ASSERT(isValid, "invalid offset");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticQueue<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(IsValidIterator(iter), "invalid range");
}

} // namespace mini