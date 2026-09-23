export module mini.core:array_view;

import :type;
import :algorithm;
import :array_iterator;
import :initializer_list;

namespace mini {

export template <typename T>
class ArrayView;

export template <typename T, typename ValueT>
concept ArrayLikeT = ConvertibleToT<T, ArrayView<ValueT>>;

template <typename T>
class ArrayView {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    using ConstIterator = ArrayIterator<ConstValue, ArrayView const>;

private:
    ConstPointer m_data;
    size_t m_size;

public:
    constexpr ArrayView() noexcept;
    constexpr ~ArrayView() noexcept = default;
    constexpr ArrayView(ArrayView const& other) noexcept = default;
    constexpr ArrayView(ConstPointer src, size_t size) noexcept;
    constexpr ArrayView(InitializerList<Value> initList) noexcept;
    template <size_t SizeN>
    constexpr ArrayView(Value (&array)[SizeN]) noexcept;

    constexpr ArrayView SubFront(size_t size) const noexcept;
    constexpr ArrayView SubBack(size_t size) const noexcept;
    constexpr ArrayView SubView(size_t index, size_t size) const noexcept;
    constexpr ArrayView SubView(ConstIterator begin, ConstIterator end) const noexcept;

    constexpr void PopFront() noexcept;
    constexpr void PopFront(size_t size) noexcept;
    constexpr void PopBack() noexcept;
    constexpr void PopBack(size_t size) noexcept;

    [[nodiscard]] constexpr ConstPointer Data() const noexcept;
    [[nodiscard]] constexpr ConstIterator Begin() const noexcept;
    [[nodiscard]] constexpr ConstIterator End() const noexcept;
    [[nodiscard]] constexpr ConstReference First() const;
    [[nodiscard]] constexpr ConstReference Last() const;
    [[nodiscard]] constexpr ConstReference At(size_t index) const;

    [[nodiscard]] constexpr size_t Size() const noexcept;
    [[nodiscard]] constexpr bool Empty() const noexcept;
    [[nodiscard]] constexpr bool ValidIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool ValidIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool ValidRange(ConstIterator begin, ConstIterator end) const noexcept;

    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    constexpr ArrayView& operator=(ArrayView const& other) noexcept = default;
    constexpr ArrayView& operator=(InitializerList<Value> initList) noexcept;
    template <size_t SizeN>
    constexpr ArrayView& operator=(Value (&array)[SizeN]) noexcept;

public:
    ArrayView(nullptr_t, size_t) = delete;

private:
    constexpr void AssertValidIndex(size_t index) const noexcept;
    constexpr void AssertValidIterator(ConstIterator iter) const noexcept;
    constexpr void AssertValidRange(ConstIterator begin, ConstIterator end) const noexcept;
};

template <typename T>
constexpr ArrayView<T>::ArrayView() noexcept
    : m_data(nullptr)
    , m_size(0)
{
}

template <typename T>
constexpr ArrayView<T>::ArrayView(ConstPointer src, size_t size) noexcept
    : m_data(src)
    , m_size(size)
{
}

template <typename T>
constexpr ArrayView<T>::ArrayView(InitializerList<T> initList) noexcept
    : m_data(initList.begin())
    , m_size(initList.size())
{
}

template <typename T>
template <size_t SizeN>
constexpr ArrayView<T>::ArrayView(Value (&array)[SizeN]) noexcept
    : m_data(array)
    , m_size(SizeN)
{
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::SubFront(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return ArrayView{m_data, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::SubBack(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return ArrayView{m_data + m_size - len, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::SubView(size_t index, size_t size) const noexcept
{
    AssertValidIndex(index);
    size_t end = m_size - index;
    size_t len = end < size ? end : size;
    return ArrayView{m_data + index, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::SubView(ConstIterator begin, ConstIterator end) const noexcept
{
    AssertValidRange(begin, end);
    return ArrayView{begin.Address(), static_cast<size_t>(end - begin)};
}

template <typename T>
constexpr void ArrayView<T>::PopFront() noexcept
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    ++m_data;
    --m_size;
}

template <typename T>
constexpr void ArrayView<T>::PopFront(size_t size) noexcept
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_data += size;
    m_size -= size;
}

template <typename T>
constexpr void ArrayView<T>::PopBack() noexcept
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    --m_size;
}

template <typename T>
constexpr void ArrayView<T>::PopBack(size_t size) noexcept
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_size -= size;
}

template <typename T>
constexpr ArrayView<T>::ConstPointer ArrayView<T>::Data() const noexcept
{
    return m_data;
}

template <typename T>
constexpr ArrayView<T>::ConstIterator ArrayView<T>::Begin() const noexcept
{
    return ConstIterator(m_data, this);
}

template <typename T>
constexpr ArrayView<T>::ConstIterator ArrayView<T>::End() const noexcept
{
    return ConstIterator(m_data + m_size, this);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::First() const
{
    AssertValidIndex(0);
    return *m_data;
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::Last() const
{
    AssertValidIndex(m_size - 1);
    return *(m_data + m_size - 1);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <typename T>
constexpr size_t ArrayView<T>::Size() const noexcept
{
    return m_size;
}

template <typename T>
constexpr bool ArrayView<T>::Empty() const noexcept
{
    return m_size == 0;
}

template <typename T>
constexpr bool ArrayView<T>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <typename T>
constexpr bool ArrayView<T>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.Address() - m_data);
    return index < m_size;
}

template <typename T>
constexpr bool ArrayView<T>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    size_t beginIdx = static_cast<size_t>(begin.Address() - m_data);
    size_t endIdx = static_cast<size_t>(end.Address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <typename T>
constexpr ArrayView<T>& ArrayView<T>::operator=(InitializerList<Value> initList) noexcept
{
    m_data = initList.begin();
    m_size = initList.size();
    return *this;
}

template <typename T>
template <size_t SizeN>
constexpr ArrayView<T>& ArrayView<T>::operator=(Value (&array)[SizeN]) noexcept
{
    m_data = array;
    m_size = SizeN;
    return *this;
}

template <typename T>
constexpr void ArrayView<T>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. view's length is {}", index, m_size);
}

template <typename T>
constexpr void ArrayView<T>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at index {}. view's length is {}", iter.Address() - m_data, m_size);
}

template <typename T>
constexpr void ArrayView<T>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                              [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}. view's length is {}",
           begin.Address() - m_data,
           end.Address() - m_data,
           m_size);
}

export template <typename T, typename U>
constexpr bool operator==(ArrayView<T> lhs, ArrayView<U> rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.Size() != rhs.Size()) {
        return false;
    }

    if (lhs.Data() == rhs.Data()) [[unlikely]] {
        return true;
    }

    return memory::EqualRange(lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End());
}

} // namespace mini