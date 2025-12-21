export module mini.array:static_array;

import mini.type;
import mini.utility;
import mini.memory;
import :iterator;

namespace mini {

export template <MovableT T, SizeT CapacityN>
class StaticArray {
private:
    typedef memory::StaticBuffer<T, CapacityN> Buffer;
    typedef memory::StaticSize<CapacityN> BufferSize;

    template <MovableT U, SizeT CapU>
    friend class StaticArray;

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
    BufferSize m_Size;
    Buffer m_Buffer;

public:
    constexpr StaticArray() noexcept;
    constexpr ~StaticArray();
    constexpr StaticArray(StaticArray const&);
    constexpr StaticArray(StaticArray&&) noexcept;
    constexpr StaticArray(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr StaticArray(Iter, Iter);

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
    constexpr bool IsValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Ref operator[](SizeT);
    constexpr ConstRef operator[](SizeT) const;

    constexpr StaticArray& operator=(StaticArray const&);
    constexpr StaticArray& operator=(StaticArray&&) noexcept;
    constexpr StaticArray& operator=(InitializerList<T>);

private:
    template <typename U>
    constexpr void AssignRangeWithSize(U, U, SizeT);
    template <typename U>
    constexpr void AppendRangeWithSize(U, U, SizeT);
    template <typename U>
    constexpr void InsertRangeWithSize(SizeT, U, U, SizeT);

    constexpr void AssertValidCapacity(SizeT) const noexcept;
    constexpr void AssertValidIndex(SizeT) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
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
inline constexpr StaticArray<T, N>::StaticArray(InitializerList<T> init)
{
    AssertValidCapacity(init.size());
    memory::ConstructRange(m_Buffer.Data(), init.begin(), init.end());
    m_Size = init.size();
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr StaticArray<T, N>::StaticArray(Iter first, Iter last)
    : m_Size(0)
{
    Append(first, last);
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticArray<T, N>::Push(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_Size + 1);
    memory::ConstructAt(m_Buffer.Data() + m_Size, ForwardArg<Args>(args)...);
    ++m_Size;
}

template <MovableT T, SizeT N>
template <typename... Args>
inline constexpr void StaticArray<T, N>::Insert(SizeT index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + (OffsetT)index, ForwardArg<Args>(args)...);
}

template <MovableT T, SizeT N>
template <typename... Args>
constexpr void StaticArray<T, N>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
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

    // without the copy, invalid reference can get copied
    Value temp(ForwardArg<Args>(args)...);

    memory::ConstructAt(end, MoveArg(*last));
    memory::MoveBackward(end, loc, last);
    memory::DestructAt(loc);
    memory::ConstructAt(loc, MoveArg(temp));
    ++m_Size;
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

    AssignRangeWithSize(first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::Assign(InitializerList<T> init)
{
    SizeT size = init.size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(init.begin(), init.end(), size);
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticArray<T, N>::Append(Iter first, Iter last)
{
    SizeT distance = Distance(first, last);
    AppendRangeWithSize(first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::Append(InitializerList<T> init)
{
    AppendRangeWithSize(init.begin(), init.end(), init.size());
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void StaticArray<T, N>::InsertRange(SizeT index, Iter first, Iter last)
{
    if (index == m_Size) {
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

    InsertRangeWithSize(index, first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::InsertRange(SizeT index, InitializerList<T> init)
{
    if (index == m_Size) {
        Append(init);
        return;
    }

    InsertRangeWithSize(index, init.begin(), init.end(), init.size());
}

template <MovableT T, SizeT N>
template <ForwardIteratableByT<T> Iter>
constexpr void StaticArray<T, N>::InsertRange(ConstIterator iter, Iter first, Iter last)
{
    SizeT locDiff = static_cast<SizeT>(iter - Begin());
    if (locDiff == m_Size) {
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

    InsertRangeWithSize(locDiff, first, last, distance);
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::InsertRange(ConstIterator iter, InitializerList<T> init)
{
    SizeT locDiff = static_cast<SizeT>(iter - Begin());
    if (locDiff == m_Size) {
        Append(init);
        return;
    }

    InsertRangeWithSize(locDiff, init.begin(), init.end(), init.size());
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
    RemoveAt(Begin() + static_cast<OffsetT>(index));
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

    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_Size;
}

template <MovableT T, SizeT N>
inline constexpr void StaticArray<T, N>::RemoveRange(SizeT first, SizeT last)
{
    Iterator begin = Begin();
    RemoveRange(begin + static_cast<OffsetT>(first), begin + static_cast<OffsetT>(last));
}

template <MovableT T, SizeT N>
constexpr void StaticArray<T, N>::RemoveRange(ConstIterator first, ConstIterator last)
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
    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;
    Ptr loc = begin + (first - iterBegin);

    memory::MoveRange(loc, loc + distance, end);
    memory::DestructRange(end - distance, end);
    m_Size -= distance;
}

template <MovableT T, SizeT N>
template <typename... Args>
constexpr void StaticArray<T, N>::Resize(SizeT size, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_Size == size) [[unlikely]] {
        return;
    }

    AssertValidCapacity(size);
    Ptr begin = m_Buffer.Data();
    Ptr end = begin + m_Size;

    if (m_Size < size) {
        Value temp(ForwardArg<Args>(args)...);
        memory::ConstructRangeArgs(end, begin + size, temp);
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
    return Iterator(m_Buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::ConstIterator StaticArray<T, N>::Begin() const noexcept
{
    return ConstIterator(m_Buffer.Data(), this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::Iterator StaticArray<T, N>::End() noexcept
{
    return Iterator(m_Buffer.Data() + m_Size, this);
}

template <MovableT T, SizeT N>
inline constexpr StaticArray<T, N>::ConstIterator StaticArray<T, N>::End() const noexcept
{
    return ConstIterator(m_Buffer.Data() + m_Size, this);
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
    SizeT index = static_cast<SizeT>(iter.m_Ptr - m_Buffer.Data());
    return index < m_Size;
}

template <MovableT T, SizeT N>
inline constexpr bool StaticArray<T, N>::IsValidRange(ConstIterator begin,
                                                      ConstIterator end) const noexcept
{
    ConstPtr buffer = m_Buffer.Data();
    SizeT beginIdx = static_cast<SizeT>(begin.m_Ptr - buffer);
    SizeT endIdx = static_cast<SizeT>(end.m_Ptr - buffer);
    return (beginIdx < m_Size) && (endIdx < m_Size + 1);
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
inline constexpr StaticArray<T, N>& StaticArray<T, N>::operator=(InitializerList<T> init)
{
    Assign(init);
    return *this;
}

template <MovableT T, SizeT N>
template <typename U>
inline constexpr void StaticArray<T, N>::AssignRangeWithSize(U first, U last, SizeT len)
{
    AssertValidCapacity(len);

    OffsetT size = (OffsetT)m_Size;
    Ptr begin = m_Buffer.Data();

    if (m_Size < len) {
        memory::CopyRange(begin, first, first + size);
        memory::ConstructRange(begin + size, first + size, last);
    }
    else {
        memory::CopyRange(begin, first, last);
        memory::DestructRange(begin + len, begin + size);
    }

    m_Size = len;
}

template <MovableT T, SizeT N>
template <typename U>
inline constexpr void StaticArray<T, N>::AppendRangeWithSize(U first, U last, SizeT len)
{
    SizeT newSize = m_Size + len;
    AssertValidCapacity(newSize);
    memory::ConstructRange(m_Buffer.Data() + m_Size, first, last);
    m_Size = newSize;
}

template <MovableT T, SizeT N>
template <typename U>
inline constexpr void StaticArray<T, N>::InsertRangeWithSize(SizeT index, U first, U last,
                                                             SizeT len)
{
    SizeT newSize = m_Size + len;
    AssertValidCapacity(newSize);
    AssertValidIndex(index);

    Ptr begin = m_Buffer.Data();
    Ptr loc = begin + index;
    Ptr end = begin + (OffsetT)m_Size;

    if (static_cast<SizeT>(end - loc) > len) {
        memory::MoveConstructBackward(end + len, end - len, end);
        memory::MoveBackward(end, loc, end - len);
        memory::DestructRange(loc, loc + len);
    }
    else {
        memory::MoveConstructBackward(end + len, loc, end);
        memory::DestructRange(loc, end);
    }

    m_Size = newSize;
    memory::ConstructRange(loc, first, last);
}

template <MovableT T, SizeT N>
inline constexpr void
StaticArray<T, N>::AssertValidCapacity([[maybe_unused]] SizeT cap) const noexcept
{
    ASSERT(cap <= m_Buffer.Capacity(), "invalid capacity");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticArray<T, N>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(IsValidIndex(index), "invalid index");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticArray<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(IsValidIterator(iter), "invalid iterator");
}

template <MovableT T, SizeT N>
inline constexpr void
StaticArray<T, N>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                    [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(IsValidRange(begin, end), "invalid range");
}

export template <MovableT T, SizeT CapT, MovableT U, SizeT CapU>
inline constexpr bool operator==(StaticArray<T, CapT> const& l,
                                 StaticArray<U, CapU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (l.Size() != r.Size()) {
        return false;
    }
    else if (l.Data() == r.Data()) [[unlikely]] {
        return true;
    }
    else if (l.Size() == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(l.Begin(), l.End(), r.Begin(), r.End());
}

} // namespace mini