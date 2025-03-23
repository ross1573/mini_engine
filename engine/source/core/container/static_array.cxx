module;

#include "assertion.h"

export module mini.core:static_array;

import :type;
import :utility;
import :memory;
import :algorithm;
import :static_buffer;
import :array_iterator;
import :move_iterator;

namespace mini {

export template <MovableT T, SizeT CapacityN>
class StaticArray {
private:
    typedef StaticBuffer<T, CapacityN> Buffer;

public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;
    typedef ArrayIterator<Value, StaticArray> Iterator;
    typedef ArrayIterator<ConstValue, StaticArray const> ConstIterator;

private:
    StaticSize<CapacityN> m_Size;
    Buffer m_Buffer;

public:
    constexpr StaticArray() noexcept;
    constexpr ~StaticArray();
    constexpr StaticArray(StaticArray const&);
    constexpr StaticArray(StaticArray&&) noexcept;
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr StaticArray(Iter, Iter);

    template <typename... Args>
    constexpr void Push(Args&&...) requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(SizeT, Args&&...) requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(ConstIterator, Args&&...) requires ConstructibleFromT<T, Args...>;
    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void AddRange(Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(SizeT, Iter, Iter);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator, Iter, Iter);
    constexpr void RemoveLast();
    constexpr void RemoveLast(SizeT);
    constexpr void RemoveAt(SizeT);
    constexpr void RemoveAt(ConstIterator);
    constexpr void RemoveRange(SizeT, SizeT);
    constexpr void RemoveRange(ConstIterator, ConstIterator);

    template <typename... Args>
    constexpr void Resize(SizeT, Args&&...) requires ConstructibleFromT<T, Args...>;
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

    consteval SizeT Capacity() const noexcept;
    constexpr SizeT Size() const noexcept;
    constexpr bool IsEmpty() const noexcept;
    constexpr bool IsFull() const noexcept;
    constexpr bool IsValidIndex(SizeT) const noexcept;
    constexpr bool IsValidIterator(ConstIterator) const noexcept;

    constexpr Ref operator[](SizeT);
    constexpr ConstRef operator[](SizeT) const;

    template <EqualityComparableWithT<T> U, SizeT OtherN>
    constexpr bool operator==(StaticArray<U, OtherN> const&) const;

    constexpr StaticArray& operator=(StaticArray const&);
    constexpr StaticArray& operator=(StaticArray&&) noexcept;

private:
    constexpr void AssertValidCapacity(SizeT) const noexcept;
    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
};

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::StaticArray() noexcept
    : m_Size(0)
    , m_Buffer()
{
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::~StaticArray()
{
    Clear();
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::StaticArray(StaticArray const& other)
{
    memory::ConstructRange(m_Buffer.Data(), other.Begin(), other.End());
    m_Size = other.m_Size;
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::StaticArray(StaticArray&& other) noexcept
{
    Ptr otherBegin = other.m_Buffer.Data();
    Ptr otherEnd = otherBegin + other.m_Size;

    memory::MoveConstructRange(m_Buffer.Data(), otherBegin, otherEnd);
    memory::DestructRange(otherBegin, otherEnd);
    m_Size = other.m_Size;
    other.m_Size = 0;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr StaticArray<T, N>::StaticArray(Iter first, Iter last)
{
    AddRange(first, last);
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticArray<T, N>::Push(Args&&... args) requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_Size + 1);
    memory::ConstructAt(m_Buffer.Data() + m_Size, ForwardArg<Args>(args)...);
    ++m_Size;
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticArray<T, N>::Insert(SizeT index, Args&&... args) requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + (OffsetT)index, ForwardArg<Args>(args)...);
}

template <MovableT T, SizeT N>
template <typename... Args>
constexpr void StaticArray<T, N>::Insert(ConstIterator iter, Args&&... args) requires ConstructibleFromT<T, Args...>
{
    OffsetT locDiff = iter - Begin();
    if (locDiff == (OffsetT)m_Size) {
        Push(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidCapacity(m_Size + 1);
    Ptr begin = m_Buffer.Data();
    Ptr loc = begin + locDiff;
    Ptr end = begin + (OffsetT)m_Size;
    Ptr last = end - 1;

    memory::ConstructAt(end, MoveArg(*last));
    memory::MoveBackward(end, loc, last);
    memory::DestructAt(loc);

    ++m_Size;
    memory::ConstructAt(loc, ForwardArg<Args>(args)...);
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void StaticArray<T, N>::Assign(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssertValidCapacity(distance);
    OffsetT size = (OffsetT)m_Size;
    Ptr begin = m_Buffer.Data();

    if (m_Size < distance) {
        memory::CopyRange(begin, first, first + size);
        memory::ConstructRange(begin + size, first + size, last);
    }
    else {
        memory::CopyRange(begin, first, last);
        memory::DestructRange(begin + distance, begin + size);
    }

    m_Size = distance;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticArray<T, N>::AddRange(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:
            Push(ForwardArg<typename Iter::Value>(*first));
            return;
        default:
            break;
    }

    SizeT newSize = m_Size + distance;
    AssertValidCapacity(newSize);
    memory::ConstructRange(m_Buffer.Data() + m_Size, first, last);
    m_Size += distance;
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void StaticArray<T, N>::InsertRange(SizeT index, Iter first, Iter last)
{
    InsertRange(Begin() + (OffsetT)index, first, last);
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticArray<T, N>::InsertRange(ConstIterator iter, Iter first, Iter last)
{
    OffsetT locDiff = iter - Begin();
    if (locDiff == (OffsetT)m_Size) {
        AddRange(first, last);
        return;
    }

    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:
            Insert(iter, ForwardArg<typename Iter::Value>(*first));
            return;
        default:
            break;
    }

    SizeT newSize = m_Size + distance;
    AssertValidCapacity(newSize);

    Ptr begin = m_Buffer.Data();
    Ptr loc = begin + locDiff;
    Ptr end = begin + (OffsetT)m_Size;

    if (static_cast<SizeT>(end - loc) > distance) {
        memory::MoveConstructBackward(end + distance, end - distance, end);
        memory::MoveBackward(end, loc, end - distance);
        memory::DestructRange(loc, loc + distance);
    }
    else {
        memory::MoveConstructBackward(end + distance, loc, end);
        memory::DestructRange(loc, end);
    }

    m_Size += distance;
    memory::ConstructRange(loc, first, last);
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::RemoveLast()
{
    if (IsEmpty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_Buffer.Data() + m_Size - 1);
    --m_Size;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::RemoveLast(SizeT count)
{
    if (IsEmpty() || count == 0) [[unlikely]] {
        return;
    }

    SizeT removeCnt = m_Size < count ? (SizeT)m_Size : count;
    Ptr end = m_Buffer.Data() + m_Size;

    memory::DestructRange(end - removeCnt, end);
    m_Size -= count;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::RemoveAt(SizeT index)
{
    RemoveAt(Begin() + (OffsetT)index);
}

template <MovableT T, SizeT N>
constexpr void StaticArray<T, N>::RemoveAt(ConstIterator iter)
{
    OffsetT locDiff = iter - Begin();
    if (locDiff == (OffsetT)m_Size) {
        RemoveLast();
        return;
    }

    AssertValidIterator(iter);
    Ptr begin = m_Buffer.Data();
    Ptr loc = begin + locDiff;
    Ptr end = begin + (OffsetT)m_Size;

    memory::DestructAt(loc);
    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_Size;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::RemoveRange(SizeT first, SizeT last)
{
    Iterator begin = Begin();
    RemoveRange(begin + (OffsetT)first, begin + (OffsetT)last);
}

template <MovableT T, SizeT N>
constexpr void StaticArray<T, N>::RemoveRange(ConstIterator first, ConstIterator last)
{
    SizeT distance = Distance(first, last);
    switch (distance) {
        [[unlikely]] case 0:
            return;
        case 1:
            RemoveAt(first);
            return;
        default:
            break;
    }

    AssertValidIterator(first);
    AssertValidIterator(last - 1);

    Iterator iterBegin = Begin();
    Ptr begin = m_Buffer.Data();
    Ptr locFirst = begin + (first - iterBegin);
    Ptr locLast = begin + (last - iterBegin);

    memory::DestructRange(locFirst, locLast);

    Ptr end = begin + m_Size;
    if (locLast != end) {
        memory::MoveRange(locFirst, locFirst + distance, end);
        memory::DestructRange(end - distance, end);
    }

    m_Size -= distance;
}

template <MovableT T, SizeT N>
template <typename... Args>
constexpr void StaticArray<T, N>::Resize(SizeT size, Args&&... args) requires ConstructibleFromT<T, Args...>
{
    if (m_Size == size) [[unlikely]] {
        return;
    }

    AssertValidCapacity(size);
    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;

    if (m_Size < size) {
        memory::ConstructRangeArgs(end, begin + size, ForwardArg<Args>(args)...);
    }
    else {
        memory::DestructRange(begin + size, end);
    }

    m_Size = size;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::Clear()
{
    if (m_Size == 0) [[unlikely]] {
        return;
    }

    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;

    memory::DestructRange(begin, end);
    m_Size = 0;
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::Ptr StaticArray<T, N>::Data() noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::ConstPtr StaticArray<T, N>::Data() const noexcept
{
    return m_Buffer.Data();
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::Iterator StaticArray<T, N>::Begin() noexcept
{
    return Iterator(m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::ConstIterator StaticArray<T, N>::Begin() const noexcept
{
    return ConstIterator(m_Buffer.Data(), 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::Iterator StaticArray<T, N>::End() noexcept
{
    return Iterator(m_Buffer.Data() + m_Size, 0, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::ConstIterator StaticArray<T, N>::End() const noexcept
{
    return ConstIterator(m_Buffer.Data() + m_Size, 0, this);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticArray<T, N>::First()
{
    return At(0);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticArray<T, N>::First() const
{
    return At(0);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticArray<T, N>::Last()
{
    return At(m_Size - 1);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticArray<T, N>::Last() const
{
    return At(m_Size - 1);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticArray<T, N>::At(SizeT index)
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticArray<T, N>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, SizeT N>
inline consteval SizeT StaticArray<T, N>::Capacity() const noexcept
{
    return m_Buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr SizeT StaticArray<T, N>::Size() const noexcept
{
    return m_Size.Get();
}

template <MovableT T, SizeT N>
inline constexpr bool StaticArray<T, N>::IsEmpty() const noexcept
{
    return m_Size == 0;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticArray<T, N>::IsFull() const noexcept
{
    return (SizeT)m_Size == m_Buffer.Capacity();
}

template <MovableT T, SizeT N>
inline constexpr bool StaticArray<T, N>::IsValidIndex(SizeT index) const noexcept
{
    return index < m_Size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticArray<T, N>::IsValidIterator(ConstIterator iter) const noexcept
{
    OffsetT diff = iter.m_Ptr - m_Buffer.Data();
    return diff >= 0 && diff < static_cast<OffsetT>(m_Size);
}

template <MovableT T, SizeT N>
inline constexpr T& StaticArray<T, N>::operator[](SizeT index)
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, SizeT N>
inline constexpr T const& StaticArray<T, N>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Buffer.Data() + index);
}

template <MovableT T, SizeT N>
template <EqualityComparableWithT<T> U, SizeT OtherN>
inline constexpr bool StaticArray<T, N>::operator==(StaticArray<U, OtherN> const& other) const
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
inline constexpr StaticArray<T, N>& StaticArray<T, N>::operator=(StaticArray const& other)
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>& StaticArray<T, N>::operator=(StaticArray&& other) noexcept
{
    if (m_Buffer == other.m_Buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::AssertValidCapacity([[maybe_unused]] SizeT cap) const noexcept
{
    CONSTEXPR_ASSERT(cap <= m_Buffer.Capacity(), "invalid capacity");
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    CONSTEXPR_ASSERT(IsValidIndex(index), "invalid index");
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    CONSTEXPR_ASSERT(IsValidIterator(iter), "invalid range");
}

} // namespace mini