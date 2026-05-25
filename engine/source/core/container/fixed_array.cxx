export module mini.core:fixed_array;

import :type;
import :initializer_list;
import :utility_operation;
import :memory_operation;
import :algorithm;
import :fixed_buffer;
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
    constexpr FixedArray(FixedArray const&);
    constexpr FixedArray(FixedArray&&) noexcept;
    constexpr FixedArray(InitializerList<T>);
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr FixedArray(Iter, Iter);

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
    constexpr Reference At(size_t);
    constexpr ConstReference At(size_t) const;

    consteval size_t Capacity() const noexcept;
    constexpr size_t Size() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool Full() const noexcept;
    constexpr bool ValidIndex(size_t) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr Reference operator[](size_t);
    constexpr ConstReference operator[](size_t) const;

    constexpr FixedArray& operator=(FixedArray const&);
    constexpr FixedArray& operator=(FixedArray&&) noexcept;
    constexpr FixedArray& operator=(InitializerList<T>);

private:
    template <typename U>
    constexpr void AssignRangeWithSize(U, U, size_t);
    template <typename U>
    constexpr void AppendRangeWithSize(U, U, size_t);
    template <typename U>
    constexpr void InsertRangeWithSize(size_t, U, U, size_t);

    constexpr void AssertValidCapacity(size_t) const noexcept;
    constexpr void AssertValidIndex(size_t) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::FixedArray() noexcept
    : m_size(0)
    , m_buffer()
{
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::~FixedArray()
{
    Clear();
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::FixedArray(FixedArray const& other)
{
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.m_size;
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::FixedArray(FixedArray&& other) noexcept
{
    Pointer otherBegin = other.m_buffer.Data();
    Pointer otherEnd = otherBegin + other.m_size;

    memory::MoveConstructRange(m_buffer.Data(), otherBegin, otherEnd);
    memory::DestructRange(otherBegin, otherEnd);
    m_size = other.m_size;
    other.m_size = 0;
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::FixedArray(InitializerList<T> init)
{
    AssertValidCapacity(init.size());
    memory::ConstructRange(m_buffer.Data(), init.begin(), init.end());
    m_size = init.size();
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
inline constexpr FixedArray<T, N>::FixedArray(Iter first, Iter last)
    : m_size(0)
{
    Append(first, last);
}

template <MovableT T, size_t N>
template <typename... Args>
inline constexpr void FixedArray<T, N>::Push(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_size + 1);
    memory::ConstructAt(m_buffer.Data() + m_size, ForwardArg<Args>(args)...);
    ++m_size;
}

template <MovableT T, size_t N>
template <typename... Args>
inline constexpr void FixedArray<T, N>::Insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + (offset_t)index, ForwardArg<Args>(args)...);
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedArray<T, N>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - Begin();
    if (locDiff == (offset_t)m_size) {
        Push(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidCapacity(m_size + 1);
    Pointer begin = m_buffer.Data();
    Pointer loc = begin + locDiff;
    Pointer end = begin + (offset_t)m_size;
    Pointer last = end - 1;

    // without the copy, invalid reference can get copied
    Value temp(ForwardArg<Args>(args)...);

    memory::ConstructAt(end, MoveArg(*last));
    memory::MoveBackward(end, loc, last);
    memory::DestructAt(loc);
    memory::ConstructAt(loc, MoveArg(temp));
    ++m_size;
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void FixedArray<T, N>::Assign(Iter first, Iter last)
{
    size_t distance = Distance(first, last);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(first, last, distance);
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::Assign(InitializerList<T> init)
{
    size_t size = init.size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(init.begin(), init.end(), size);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, N>::Append(Iter first, Iter last)
{
    size_t distance = Distance(first, last);
    AppendRangeWithSize(first, last, distance);
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::Append(InitializerList<T> init)
{
    AppendRangeWithSize(init.begin(), init.end(), init.size());
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
inline constexpr void FixedArray<T, N>::InsertRange(size_t index, Iter first, Iter last)
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

    InsertRangeWithSize(index, first, last, distance);
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::InsertRange(size_t index, InitializerList<T> init)
{
    if (index == m_size) {
        Append(init);
        return;
    }

    InsertRangeWithSize(index, init.begin(), init.end(), init.size());
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, N>::InsertRange(ConstIterator iter, Iter first, Iter last)
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

    InsertRangeWithSize(locDiff, first, last, distance);
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::InsertRange(ConstIterator iter, InitializerList<T> init)
{
    size_t locDiff = static_cast<size_t>(iter - Begin());
    if (locDiff == m_size) {
        Append(init);
        return;
    }

    InsertRangeWithSize(locDiff, init.begin(), init.end(), init.size());
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::RemoveLast()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_size - 1);
    --m_size;
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::RemoveLast(size_t count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = m_size < count ? (size_t)m_size : count;
    Pointer end = m_buffer.Data() + m_size;

    memory::DestructRange(end - removeCnt, end);
    m_size -= count;
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::RemoveAt(size_t index)
{
    RemoveAt(Begin() + static_cast<offset_t>(index));
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::RemoveAt(ConstIterator iter)
{
    offset_t locDiff = iter - Begin();
    if (locDiff == (offset_t)m_size) {
        RemoveLast();
        return;
    }

    AssertValidIterator(iter);
    Pointer begin = m_buffer.Data();
    Pointer loc = begin + locDiff;
    Pointer end = begin + (offset_t)m_size;

    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_size;
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::RemoveRange(size_t first, size_t last)
{
    Iterator begin = Begin();
    RemoveRange(begin + static_cast<offset_t>(first), begin + static_cast<offset_t>(last));
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::RemoveRange(ConstIterator first, ConstIterator last)
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

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedArray<T, N>::Resize(size_t size, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    if (m_size == size) [[unlikely]] {
        return;
    }

    AssertValidCapacity(size);
    Pointer begin = m_buffer.Data();
    Pointer end = begin + m_size;

    if (m_size < size) {
        Value temp(ForwardArg<Args>(args)...);
        memory::ConstructRangeArgs(end, begin + size, temp);
    } else {
        memory::DestructRange(begin + size, end);
    }

    m_size = size;
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::Clear()
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    Pointer begin = m_buffer.Data();
    Pointer end = begin + m_size;

    memory::DestructRange(begin, end);
    m_size = 0;
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::Pointer FixedArray<T, N>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::ConstPointer FixedArray<T, N>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::Iterator FixedArray<T, N>::Begin() noexcept
{
    return Iterator(m_buffer.Data(), this);
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::ConstIterator FixedArray<T, N>::Begin() const noexcept
{
    return ConstIterator(m_buffer.Data(), this);
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::Iterator FixedArray<T, N>::End() noexcept
{
    return Iterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>::ConstIterator FixedArray<T, N>::End() const noexcept
{
    return ConstIterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, size_t N>
inline constexpr T& FixedArray<T, N>::First()
{
    return At(0);
}

template <MovableT T, size_t N>
inline constexpr T const& FixedArray<T, N>::First() const
{
    return At(0);
}

template <MovableT T, size_t N>
inline constexpr T& FixedArray<T, N>::Last()
{
    return At(m_size - 1);
}

template <MovableT T, size_t N>
inline constexpr T const& FixedArray<T, N>::Last() const
{
    return At(m_size - 1);
}

template <MovableT T, size_t N>
inline constexpr T& FixedArray<T, N>::At(size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
inline constexpr T const& FixedArray<T, N>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
inline consteval size_t FixedArray<T, N>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, size_t N>
inline constexpr size_t FixedArray<T, N>::Size() const noexcept
{
    return m_size.Get();
}

template <MovableT T, size_t N>
inline constexpr bool FixedArray<T, N>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, size_t N>
inline constexpr bool FixedArray<T, N>::Full() const noexcept
{
    return (size_t)m_size == m_buffer.Capacity();
}

template <MovableT T, size_t N>
inline constexpr bool FixedArray<T, N>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, size_t N>
inline constexpr bool FixedArray<T, N>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.Data());
    return index < m_size;
}

template <MovableT T, size_t N>
inline constexpr bool FixedArray<T, N>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    ConstPointer buffer = m_buffer.Data();
    size_t beginIdx = static_cast<size_t>(begin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(end.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, size_t N>
inline constexpr T& FixedArray<T, N>::operator[](size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
inline constexpr T const& FixedArray<T, N>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(FixedArray const& other)
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(FixedArray&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, size_t N>
inline constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(InitializerList<T> init)
{
    Assign(init);
    return *this;
}

template <MovableT T, size_t N>
template <typename U>
inline constexpr void FixedArray<T, N>::AssignRangeWithSize(U first, U last, size_t len)
{
    AssertValidCapacity(len);

    offset_t size = (offset_t)m_size;
    Pointer begin = m_buffer.Data();

    if (m_size < len) {
        memory::CopyRange(begin, first, first + size);
        memory::ConstructRange(begin + size, first + size, last);
    } else {
        memory::CopyRange(begin, first, last);
        memory::DestructRange(begin + len, begin + size);
    }

    m_size = len;
}

template <MovableT T, size_t N>
template <typename U>
inline constexpr void FixedArray<T, N>::AppendRangeWithSize(U first, U last, size_t len)
{
    size_t newSize = m_size + len;
    AssertValidCapacity(newSize);
    memory::ConstructRange(m_buffer.Data() + m_size, first, last);
    m_size = newSize;
}

template <MovableT T, size_t N>
template <typename U>
inline constexpr void FixedArray<T, N>::InsertRangeWithSize(size_t index, U first, U last, size_t len)
{
    size_t newSize = m_size + len;
    AssertValidCapacity(newSize);
    AssertValidIndex(index);

    Pointer begin = m_buffer.Data();
    Pointer loc = begin + index;
    Pointer end = begin + (offset_t)m_size;

    if (static_cast<size_t>(end - loc) > len) {
        memory::MoveConstructBackward(end + len, end - len, end);
        memory::MoveBackward(end, loc, end - len);
        memory::DestructRange(loc, loc + len);
    } else {
        memory::MoveConstructBackward(end + len, loc, end);
        memory::DestructRange(loc, end);
    }

    m_size = newSize;
    memory::ConstructRange(loc, first, last);
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::AssertValidCapacity([[maybe_unused]] size_t cap) const noexcept
{
    ASSERT(cap <= m_buffer.Capacity(), "invalid capacity");
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index");
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator");
}

template <MovableT T, size_t N>
inline constexpr void FixedArray<T, N>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                         [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end), "invalid range");
}

export template <MovableT T, size_t CapT, MovableT U, size_t CapU>
inline constexpr bool operator==(FixedArray<T, CapT> const& l, FixedArray<U, CapU> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (l.Size() != r.Size()) {
        return false;
    } else if (l.Data() == r.Data()) [[unlikely]] {
        return true;
    } else if (l.Size() == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(l.Begin(), l.End(), r.Begin(), r.End());
}

} // namespace mini