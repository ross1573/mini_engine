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
    constexpr ArrayView(ConstPointer src, size_t count) noexcept;
    constexpr ArrayView(InitializerList<Value> initList) noexcept;
    template <size_t SizeN>
    constexpr ArrayView(Value (&array)[SizeN]) noexcept;

    constexpr ArrayView subFront(size_t count) const noexcept;
    constexpr ArrayView subBack(size_t count) const noexcept;
    constexpr ArrayView subView(size_t index, size_t count) const noexcept;
    constexpr ArrayView subView(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;

    constexpr void popFront() noexcept;
    constexpr void popFront(size_t count) noexcept;
    constexpr void popBack() noexcept;
    constexpr void popBack(size_t count) noexcept;

    [[nodiscard]] constexpr ConstPointer data() const noexcept;
    [[nodiscard]] constexpr ConstIterator begin() const noexcept;
    [[nodiscard]] constexpr ConstIterator end() const noexcept;
    [[nodiscard]] constexpr ConstReference first() const;
    [[nodiscard]] constexpr ConstReference last() const;
    [[nodiscard]] constexpr ConstReference at(size_t index) const;

    [[nodiscard]] constexpr size_t size() const noexcept;
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr bool validIndex(size_t index) const noexcept;
    [[nodiscard]] constexpr bool validIterator(ConstIterator iter) const noexcept;
    [[nodiscard]] constexpr bool validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;

    [[nodiscard]] constexpr ConstReference operator[](size_t index) const;

    template <typename U>
    constexpr operator ArrayView<U>() const noexcept
        requires ConvertibleToT<T const*, U const*>;

    constexpr ArrayView& operator=(ArrayView const& other) noexcept = default;
    constexpr ArrayView& operator=(InitializerList<Value> initList) noexcept;
    template <size_t SizeN>
    constexpr ArrayView& operator=(Value (&array)[SizeN]) noexcept;

public:
    ArrayView(nullptr_t, size_t) = delete;

private:
    constexpr void assertValidIndex(size_t index) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
};

template <typename T>
constexpr ArrayView<T>::ArrayView() noexcept
    : m_data(nullptr)
    , m_size(0)
{
}

template <typename T>
constexpr ArrayView<T>::ArrayView(ConstPointer src, size_t count) noexcept
    : m_data(src)
    , m_size(count)
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
constexpr ArrayView<T> ArrayView<T>::subFront(size_t count) const noexcept
{
    size_t len = m_size < count ? m_size : count;
    return ArrayView{m_data, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::subBack(size_t count) const noexcept
{
    size_t len = m_size < count ? m_size : count;
    return ArrayView{m_data + m_size - len, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::subView(size_t index, size_t count) const noexcept
{
    assertValidIndex(index);
    size_t endIdx = m_size - index;
    size_t len = endIdx < count ? endIdx : count;
    return ArrayView{m_data + index, len};
}

template <typename T>
constexpr ArrayView<T> ArrayView<T>::subView(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    assertValidRange(rangeBegin, rangeEnd);
    return ArrayView{rangeBegin.address(), static_cast<size_t>(rangeEnd - rangeBegin)};
}

template <typename T>
constexpr void ArrayView<T>::popFront() noexcept
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    ++m_data;
    --m_size;
}

template <typename T>
constexpr void ArrayView<T>::popFront(size_t count) noexcept
{
    if (m_size < count) [[unlikely]] {
        count = m_size;
    }

    m_data += count;
    m_size -= count;
}

template <typename T>
constexpr void ArrayView<T>::popBack() noexcept
{
    if (m_size == 0) [[unlikely]] {
        return;
    }

    --m_size;
}

template <typename T>
constexpr void ArrayView<T>::popBack(size_t count) noexcept
{
    if (m_size < count) [[unlikely]] {
        count = m_size;
    }

    m_size -= count;
}

template <typename T>
constexpr ArrayView<T>::ConstPointer ArrayView<T>::data() const noexcept
{
    return m_data;
}

template <typename T>
constexpr ArrayView<T>::ConstIterator ArrayView<T>::begin() const noexcept
{
    return ConstIterator(m_data, this);
}

template <typename T>
constexpr ArrayView<T>::ConstIterator ArrayView<T>::end() const noexcept
{
    return ConstIterator(m_data + m_size, this);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::first() const
{
    assertValidIndex(0);
    return *m_data;
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::last() const
{
    assertValidIndex(m_size - 1);
    return *(m_data + m_size - 1);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::at(size_t index) const
{
    assertValidIndex(index);
    return *(m_data + index);
}

template <typename T>
constexpr size_t ArrayView<T>::size() const noexcept
{
    return m_size;
}

template <typename T>
constexpr bool ArrayView<T>::empty() const noexcept
{
    return m_size == 0;
}

template <typename T>
constexpr bool ArrayView<T>::validIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <typename T>
constexpr bool ArrayView<T>::validIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.address() - m_data);
    return index < m_size;
}

template <typename T>
constexpr bool ArrayView<T>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    size_t beginIdx = static_cast<size_t>(rangeBegin.address() - m_data);
    size_t endIdx = static_cast<size_t>(rangeEnd.address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <typename T>
constexpr ArrayView<T>::ConstReference ArrayView<T>::operator[](size_t index) const
{
    assertValidIndex(index);
    return *(m_data + index);
}

template <typename T>
template <typename U>
constexpr ArrayView<T>::operator ArrayView<U>() const noexcept
    requires ConvertibleToT<T const*, U const*>
{
    return ArrayView<U>(m_data, m_size);
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
constexpr void ArrayView<T>::assertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(validIndex(index), "invalid index {}. view's length is {}", index, m_size);
}

template <typename T>
constexpr void ArrayView<T>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter), "invalid iterator at index {}. view's length is {}", iter.address() - m_data, m_size);
}

template <typename T>
constexpr void ArrayView<T>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                              [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd),
           "invalid range from {} to {}. view's length is {}",
           rangeBegin.address() - m_data,
           rangeEnd.address() - m_data,
           m_size);
}

export template <typename T, typename U>
constexpr bool operator==(ArrayView<T> lhs, ArrayView<U> rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    if (lhs.data() == rhs.data()) [[unlikely]] {
        return true;
    }

    return memory::equalRange(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace mini