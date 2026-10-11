export module mini.core:string_view;

import :type;
import :assert;
import :cmemory;
import :algorithm_memory;
import :string_memory;
import :array_iterator;

namespace mini {

export template <CharT T>
class BasicStringView;

export using StringView = BasicStringView<char>;
export using WStringView = BasicStringView<wchar>;
export using U8StringView = BasicStringView<char8>;
export using U16StringView = BasicStringView<char16>;
export using U32StringView = BasicStringView<char32>;

export template <typename T, typename U>
concept StringLikeT = ConvertibleToT<T, BasicStringView<U>>;

template <CharT T>
class BasicStringView {
public:
    typedef T Value;
    typedef T* Pointer;
    typedef T& Reference;
    typedef T const ConstValue;
    typedef T const* ConstPointer;
    typedef T const& ConstReference;
    using ConstIterator = ArrayIterator<ConstValue, BasicStringView const>;

private:
    ConstPointer m_data;
    size_t m_size;

    static constexpr T emptyStr[1] = {'\0'};

public:
    constexpr BasicStringView() noexcept;
    constexpr ~BasicStringView() = default;
    constexpr BasicStringView(BasicStringView const& other) noexcept = default;
    constexpr BasicStringView(ConstPointer src) noexcept;
    constexpr BasicStringView(ConstPointer src, size_t size) noexcept;

    constexpr void copy(Pointer dest, size_t index, size_t size) const noexcept;
    constexpr void copy(Pointer dest, ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
    constexpr BasicStringView subFront(size_t size) const noexcept;
    constexpr BasicStringView subBack(size_t size) const noexcept;
    constexpr BasicStringView subView(size_t index, size_t size) const noexcept;
    constexpr BasicStringView subView(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;

    constexpr void popFront() noexcept;
    constexpr void popFront(size_t size) noexcept;
    constexpr void popBack() noexcept;
    constexpr void popBack(size_t size) noexcept;

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

    constexpr BasicStringView& operator=(BasicStringView const& other) noexcept = default;
    constexpr BasicStringView& operator=(ConstPointer ptr) noexcept;

public:
    BasicStringView(nullptr_t) = delete;
    BasicStringView(nullptr_t, size_t) = delete;
    BasicStringView& operator=(nullptr_t) = delete;

private:
    constexpr void assertValidIndex(size_t index) const noexcept;
    constexpr void assertValidIterator(ConstIterator iter) const noexcept;
    constexpr void assertValidRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept;
};

template <CharT T>
constexpr BasicStringView<T>::BasicStringView() noexcept
    : m_data(emptyStr)
    , m_size(0)
{
}

template <CharT T>
constexpr BasicStringView<T>::BasicStringView(ConstPointer src) noexcept
{
    if (src == nullptr) [[unlikely]] {
        m_data = emptyStr;
        m_size = 0;
        return;
    }

    m_data = src;
    m_size = memory::strlen(src);
}

template <CharT T>
constexpr BasicStringView<T>::BasicStringView(ConstPointer src, size_t size) noexcept
{
    if (src == nullptr) [[unlikely]] {
        m_data = emptyStr;
        m_size = 0;
        return;
    }

    m_data = src;
    m_size = size;
}

template <CharT T>
constexpr void BasicStringView<T>::copy(Pointer dest, size_t index, size_t size) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    assertValidIndex(index);
    size_t rangeEnd = m_size - index;
    size_t len = rangeEnd < size ? rangeEnd : size;
    memory::memcopy(dest, m_data + index, len);
}

template <CharT T>
constexpr void BasicStringView<T>::copy(Pointer dest, ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    assertValidRange(rangeBegin, rangeEnd);
    memory::memcopy(dest, rangeBegin.address(), static_cast<size_t>(rangeEnd - rangeBegin));
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::subFront(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return BasicStringView(m_data, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::subBack(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return BasicStringView(m_data + m_size - len, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::subView(size_t index, size_t size) const noexcept
{
    assertValidIndex(index);
    size_t rangeEnd = m_size - index;
    size_t len = rangeEnd < size ? rangeEnd : size;
    return BasicStringView(m_data + index, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::subView(ConstIterator rangeBegin,
                                                         ConstIterator rangeEnd) const noexcept
{
    assertValidRange(rangeBegin, rangeEnd);
    return BasicStringView(rangeBegin.address(), static_cast<size_t>(rangeEnd - rangeBegin));
}

template <CharT T>
constexpr void BasicStringView<T>::popFront() noexcept
{
    if (m_size != 0) [[likely]] {
        ++m_data;
        --m_size;
    }
}

template <CharT T>
constexpr void BasicStringView<T>::popFront(size_t size) noexcept
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_data += size;
    m_size -= size;
}

template <CharT T>
constexpr void BasicStringView<T>::popBack() noexcept
{
    if (m_size != 0) [[likely]] {
        --m_size;
    }
}

template <CharT T>
constexpr void BasicStringView<T>::popBack(size_t size) noexcept
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_size -= size;
}

template <CharT T>
constexpr BasicStringView<T>::ConstPointer BasicStringView<T>::data() const noexcept
{
    return m_data;
}

template <CharT T>
constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::begin() const noexcept
{
    return ConstIterator(m_data, this);
}

template <CharT T>
constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::end() const noexcept
{
    return ConstIterator(m_data + m_size, this);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::first() const
{
    assertValidIndex(0);
    return *m_data;
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::last() const
{
    assertValidIndex(m_size - 1);
    return *(m_data + m_size - 1);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::at(size_t index) const
{
    assertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
constexpr size_t BasicStringView<T>::size() const noexcept
{
    return m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::empty() const noexcept
{
    return m_size == 0;
}

template <CharT T>
constexpr bool BasicStringView<T>::validIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::validIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.address() - m_data);
    return index < m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::validRange(ConstIterator rangeBegin, ConstIterator rangeEnd) const noexcept
{
    size_t beginIdx = static_cast<size_t>(rangeBegin.address() - m_data);
    size_t endIdx = static_cast<size_t>(rangeEnd.address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::operator[](size_t index) const
{
    assertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
constexpr BasicStringView<T>& BasicStringView<T>::operator=(ConstPointer ptr) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_data = emptyStr;
        m_size = 0;
        return *this;
    }

    m_data = ptr;
    m_size = memory::strlen(ptr);
    return *this;
}

template <CharT T>
constexpr void BasicStringView<T>::assertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(validIndex(index), "invalid index {}. view's length is {}", index, m_size);
}

template <CharT T>
constexpr void BasicStringView<T>::assertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(validIterator(iter), "invalid iterator at index {}. view's length is {}", iter.Addreess() - m_data, m_size);
}

template <CharT T>
constexpr void BasicStringView<T>::assertValidRange([[maybe_unused]] ConstIterator rangeBegin,
                                                    [[maybe_unused]] ConstIterator rangeEnd) const noexcept
{
    ASSERT(validRange(rangeBegin, rangeEnd),
           "invalid range from {} to {}. view's length is {}",
           rangeBegin.address() - m_data,
           rangeEnd.address() - m_data,
           m_size);
}

export template <CharT T>
constexpr bool operator==(BasicStringView<T> lhs, BasicStringView<T> rhs) noexcept
{
    size_t size = lhs.size();
    if (size != rhs.size()) {
        return false;
    }

    using ConstPointer = BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = lhs.data();
    ConstPointer rbuf = rhs.data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::strcomp(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U>
constexpr bool operator==(BasicStringView<T> lhs, BasicStringView<U> rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = lhs.size();
    if (size != rhs.size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = lhs.data();
    typename BasicStringView<U>::ConstPointer rbuf = rhs.data();
    return memory::equalRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, StringLikeT<T> U>
constexpr bool operator==(BasicStringView<T> str, U const& src) noexcept
{
    BasicStringView<T> view = src;
    size_t size = str.size();

    if (size != view.size()) {
        return false;
    }

    using ConstPointer = BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = str.data();
    ConstPointer rbuf = view.data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::strcomp(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U, StringLikeT<U> ViewU>
constexpr bool operator==(BasicStringView<T> str, ViewU const& src) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<U> view = src;
    size_t size = str.size();

    if (size != view.size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = str.data();
    typename BasicStringView<U>::ConstPointer rbuf = view.data();
    return memory::equalRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
constexpr bool operator==(BasicStringView<T> str, nullptr_t) = delete;

} // namespace mini