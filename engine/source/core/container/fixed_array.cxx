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
    constexpr FixedArray(FixedArray const& other);
    constexpr FixedArray(FixedArray&& other) noexcept;
    constexpr FixedArray(InitializerList<T> initList);
    template <ForwardIteratableByT<T> Iter>
    explicit constexpr FixedArray(Iter begin, Iter end);

    template <typename... Args>
    constexpr void PushBack(Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(size_t index, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
    template <typename... Args>
    constexpr void Insert(ConstIterator iter, Args&&... args)
        requires ConstructibleFromT<T, Args...>;

    template <ForwardIteratableByT<T> Iter>
    constexpr void Assign(Iter begin, Iter end);
    constexpr void Assign(InitializerList<T> initList);
    template <ForwardIteratableByT<T> Iter>
    constexpr void Append(Iter begin, Iter end);
    constexpr void Append(InitializerList<T> initList);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(size_t index, Iter begin, Iter end);
    constexpr void InsertRange(size_t index, InitializerList<T> initList);
    template <ForwardIteratableByT<T> Iter>
    constexpr void InsertRange(ConstIterator iter, Iter begin, Iter end);
    constexpr void InsertRange(ConstIterator iter, InitializerList<T> initList);

    constexpr void PopBack();
    constexpr void PopBack(size_t count);
    constexpr void Remove(size_t index);
    constexpr void Remove(ConstIterator iter);
    constexpr void RemoveRange(ConstIterator begin, ConstIterator end);

    template <typename... Args>
    constexpr void Resize(size_t size, Args&&... args)
        requires ConstructibleFromT<T, Args...>;
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

    constexpr FixedArray& operator=(FixedArray const& other);
    constexpr FixedArray& operator=(FixedArray&& other) noexcept;
    constexpr FixedArray& operator=(InitializerList<T> initList);

private:
    template <typename U>
    constexpr void AssignRangeWithSize(U begin, U end, size_t len);
    template <typename U>
    constexpr void AppendRangeWithSize(U begin, U end, size_t len);
    template <typename U>
    constexpr void InsertRangeWithSize(size_t index, U begin, U end, size_t len);

    constexpr void AssertValidCapacity(size_t capacity) const noexcept;
    constexpr void AssertValidIndex(size_t index) const noexcept;
    constexpr void AssertValidIterator(ConstIterator iter) const noexcept;
    constexpr void AssertValidRange(ConstIterator begin, ConstIterator end) const noexcept;
};

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::FixedArray() noexcept
    : m_size(0)
    , m_buffer()
{
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::~FixedArray()
{
    Clear();
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::FixedArray(FixedArray const& other)
{
    memory::ConstructRange(m_buffer.Data(), other.Begin(), other.End());
    m_size = other.m_size;
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::FixedArray(FixedArray&& other) noexcept
{
    Pointer otherBegin = other.m_buffer.Data();
    Pointer otherEnd = otherBegin + other.m_size;

    memory::MoveConstructRange(m_buffer.Data(), otherBegin, otherEnd);
    memory::DestructRange(otherBegin, otherEnd);
    m_size = other.m_size;
    other.m_size = 0;
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::FixedArray(InitializerList<T> initList)
{
    AssertValidCapacity(initList.size());
    memory::ConstructRange(m_buffer.Data(), initList.begin(), initList.end());
    m_size = initList.size();
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr FixedArray<T, N>::FixedArray(Iter begin, Iter end)
    : m_size(0)
{
    Append(begin, end);
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedArray<T, N>::PushBack(Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    AssertValidCapacity(m_size + 1);
    memory::ConstructAt(m_buffer.Data() + m_size, ForwardArg<Args>(args)...);
    ++m_size;
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedArray<T, N>::Insert(size_t index, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    Insert(Begin() + static_cast<offset_t>(index), ForwardArg<Args>(args)...);
}

template <MovableT T, size_t N>
template <typename... Args>
constexpr void FixedArray<T, N>::Insert(ConstIterator iter, Args&&... args)
    requires ConstructibleFromT<T, Args...>
{
    offset_t locDiff = iter - Begin();
    if (locDiff == static_cast<offset_t>(m_size)) {
        PushBack(ForwardArg<Args>(args)...);
        return;
    }

    AssertValidCapacity(m_size + 1);
    Pointer begin = m_buffer.Data();
    Pointer loc = begin + locDiff;
    Pointer end = begin + static_cast<offset_t>(m_size);
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
constexpr void FixedArray<T, N>::Assign(Iter begin, Iter end)
{
    size_t distance = Distance(begin, end);
    if (distance == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(begin, end, distance);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::Assign(InitializerList<T> initList)
{
    size_t size = initList.size();
    if (size == 0) [[unlikely]] {
        Clear();
        return;
    }

    AssignRangeWithSize(initList.begin(), initList.end(), size);
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, N>::Append(Iter begin, Iter end)
{
    size_t distance = Distance(begin, end);
    AppendRangeWithSize(begin, end, distance);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::Append(InitializerList<T> initList)
{
    AppendRangeWithSize(initList.begin(), initList.end(), initList.size());
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, N>::InsertRange(size_t index, Iter begin, Iter end)
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

    InsertRangeWithSize(index, begin, end, distance);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::InsertRange(size_t index, InitializerList<T> initList)
{
    if (index == m_size) {
        Append(initList);
        return;
    }

    InsertRangeWithSize(index, initList.begin(), initList.end(), initList.size());
}

template <MovableT T, size_t N>
template <ForwardIteratableByT<T> Iter>
constexpr void FixedArray<T, N>::InsertRange(ConstIterator iter, Iter begin, Iter end)
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

    InsertRangeWithSize(locDiff, begin, end, distance);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::InsertRange(ConstIterator iter, InitializerList<T> initList)
{
    size_t locDiff = static_cast<size_t>(iter - Begin());
    if (locDiff == m_size) {
        Append(initList);
        return;
    }

    InsertRangeWithSize(locDiff, initList.begin(), initList.end(), initList.size());
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::PopBack()
{
    if (Empty()) [[unlikely]] {
        return;
    }

    memory::DestructAt(m_buffer.Data() + m_size - 1);
    --m_size;
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::PopBack(size_t count)
{
    if (Empty() || count == 0) [[unlikely]] {
        return;
    }

    size_t removeCnt = m_size < count ? static_cast<size_t>(m_size) : count;
    Pointer end = m_buffer.Data() + m_size;

    memory::DestructRange(end - removeCnt, end);
    m_size -= count;
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::Remove(size_t index)
{
    Remove(Begin() + static_cast<offset_t>(index));
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::Remove(ConstIterator iter)
{
    offset_t locDiff = iter - Begin();
    if (static_cast<offset_t>(m_size) == locDiff) {
        PopBack();
        return;
    }

    AssertValidIterator(iter);
    Pointer begin = m_buffer.Data();
    Pointer loc = begin + locDiff;
    Pointer end = begin + static_cast<offset_t>(m_size);

    memory::MoveRange(loc, loc + 1, end);
    memory::DestructAt(end - 1);
    --m_size;
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::RemoveRange(ConstIterator begin, ConstIterator end)
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
constexpr void FixedArray<T, N>::Clear()
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
constexpr FixedArray<T, N>::Pointer FixedArray<T, N>::Data() noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::ConstPointer FixedArray<T, N>::Data() const noexcept
{
    return m_buffer.Data();
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::Iterator FixedArray<T, N>::Begin() noexcept
{
    return Iterator(m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::ConstIterator FixedArray<T, N>::Begin() const noexcept
{
    return ConstIterator(m_buffer.Data(), this);
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::Iterator FixedArray<T, N>::End() noexcept
{
    return Iterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>::ConstIterator FixedArray<T, N>::End() const noexcept
{
    return ConstIterator(m_buffer.Data() + m_size, this);
}

template <MovableT T, size_t N>
constexpr T& FixedArray<T, N>::First()
{
    return At(0);
}

template <MovableT T, size_t N>
constexpr T const& FixedArray<T, N>::First() const
{
    return At(0);
}

template <MovableT T, size_t N>
constexpr T& FixedArray<T, N>::Last()
{
    return At(m_size - 1);
}

template <MovableT T, size_t N>
constexpr T const& FixedArray<T, N>::Last() const
{
    return At(m_size - 1);
}

template <MovableT T, size_t N>
constexpr T& FixedArray<T, N>::At(size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
constexpr T const& FixedArray<T, N>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
constexpr size_t FixedArray<T, N>::Capacity() const noexcept
{
    return m_buffer.Capacity();
}

template <MovableT T, size_t N>
constexpr size_t FixedArray<T, N>::Size() const noexcept
{
    return m_size.Get();
}

template <MovableT T, size_t N>
constexpr bool FixedArray<T, N>::Empty() const noexcept
{
    return m_size == 0;
}

template <MovableT T, size_t N>
constexpr bool FixedArray<T, N>::Full() const noexcept
{
    return static_cast<size_t>(m_size) == m_buffer.Capacity();
}

template <MovableT T, size_t N>
constexpr bool FixedArray<T, N>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedArray<T, N>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.m_ptr - m_buffer.Data());
    return index < m_size;
}

template <MovableT T, size_t N>
constexpr bool FixedArray<T, N>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    ConstPointer buffer = m_buffer.Data();
    size_t beginIdx = static_cast<size_t>(begin.m_ptr - buffer);
    size_t endIdx = static_cast<size_t>(end.m_ptr - buffer);
    return (beginIdx < m_size) && (endIdx < m_size + 1);
}

template <MovableT T, size_t N>
constexpr T& FixedArray<T, N>::operator[](size_t index)
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
constexpr T const& FixedArray<T, N>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_buffer.Data() + index);
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(FixedArray const& other)
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(other.Begin(), other.End());
    return *this;
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(FixedArray&& other) noexcept
{
    if (m_buffer == other.m_buffer) [[unlikely]] {
        return *this;
    }

    Assign(MoveIterator(other.Begin()), MoveIterator(other.End()));
    other.Clear();
    return *this;
}

template <MovableT T, size_t N>
constexpr FixedArray<T, N>& FixedArray<T, N>::operator=(InitializerList<T> init)
{
    Assign(init);
    return *this;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedArray<T, N>::AssignRangeWithSize(U begin, U end, size_t len)
{
    AssertValidCapacity(len);

    offset_t size = static_cast<offset_t>(m_size);
    Pointer ptrBegin = m_buffer.Data();

    if (m_size < len) {
        memory::CopyRange(ptrBegin, begin, end + size);
        memory::ConstructRange(ptrBegin + size, begin + size, end);
    } else {
        memory::CopyRange(ptrBegin, begin, end);
        memory::DestructRange(ptrBegin + len, ptrBegin + size);
    }

    m_size = len;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedArray<T, N>::AppendRangeWithSize(U begin, U end, size_t len)
{
    size_t newSize = m_size + len;
    AssertValidCapacity(newSize);
    memory::ConstructRange(m_buffer.Data() + m_size, begin, end);
    m_size = newSize;
}

template <MovableT T, size_t N>
template <typename U>
constexpr void FixedArray<T, N>::InsertRangeWithSize(size_t index, U begin, U end, size_t len)
{
    size_t newSize = m_size + len;
    AssertValidCapacity(newSize);
    AssertValidIndex(index);

    Pointer ptrBegin = m_buffer.Data();
    Pointer ptrLoc = ptrBegin + index;
    Pointer ptrEnd = ptrBegin + static_cast<offset_t>(m_size);

    if (static_cast<size_t>(ptrEnd - ptrLoc) > len) {
        memory::MoveConstructBackward(ptrEnd + len, ptrEnd - len, ptrEnd);
        memory::MoveBackward(ptrEnd, ptrLoc, ptrEnd - len);
        memory::DestructRange(ptrLoc, ptrLoc + len);
    } else {
        memory::MoveConstructBackward(ptrEnd + len, ptrLoc, ptrEnd);
        memory::DestructRange(ptrLoc, ptrEnd);
    }

    m_size = newSize;
    memory::ConstructRange(ptrLoc, begin, end);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::AssertValidCapacity([[maybe_unused]] size_t capacity) const noexcept
{
    ASSERT(capacity <= m_buffer.Capacity(), "invalid capacity {}. max capacity is {}", capacity, N);
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. current size is {}", index, m_size.Get());
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at index {}", iter.Address() - m_buffer.Data());
}

template <MovableT T, size_t N>
constexpr void FixedArray<T, N>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                  [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}",
           begin.Address() - m_buffer.Data(),
           end.Address() - m_buffer.Data());
}

export template <MovableT T, size_t CapT, MovableT U, size_t CapU>
constexpr bool operator==(FixedArray<T, CapT> const& lhs, FixedArray<U, CapU> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.Size() != rhs.Size()) {
        return false;
    }

    if (lhs.Data() == rhs.Data()) [[unlikely]] {
        return true;
    }

    if (lhs.Size() == 0) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End());
}

} // namespace mini