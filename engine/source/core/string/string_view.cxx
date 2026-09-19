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

    static constexpr T empty[1] = { '\0' };

public:
    constexpr BasicStringView() noexcept;
    constexpr ~BasicStringView() = default;
    constexpr BasicStringView(BasicStringView const& other) noexcept = default;
    constexpr BasicStringView(ConstPointer src) noexcept;
    constexpr BasicStringView(ConstPointer src, size_t size) noexcept;

    constexpr void Copy(Pointer dest, size_t index, size_t size) const noexcept;
    constexpr void Copy(Pointer dest, ConstIterator begin, ConstIterator end) const noexcept;
    constexpr BasicStringView SubFirst(size_t size) const noexcept;
    constexpr BasicStringView SubLast(size_t size) const noexcept;
    constexpr BasicStringView SubString(size_t index, size_t size) const noexcept;
    constexpr BasicStringView SubString(ConstIterator begin, ConstIterator end) const noexcept;

    constexpr void RemoveFirst();
    constexpr void RemoveFirst(size_t size);
    constexpr void RemoveLast();
    constexpr void RemoveLast(size_t size);

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

    constexpr BasicStringView& operator=(BasicStringView const& other) noexcept = default;
    constexpr BasicStringView& operator=(ConstPointer ptr) noexcept;

public:
    BasicStringView(nullptr_t) = delete;
    BasicStringView(nullptr_t, size_t) = delete;
    BasicStringView& operator=(nullptr_t) = delete;

private:
    constexpr void AssertValidIndex(size_t index) const noexcept;
    constexpr void AssertValidIterator(ConstIterator iter) const noexcept;
    constexpr void AssertValidRange(ConstIterator begin, ConstIterator end) const noexcept;
};

template <CharT T>
constexpr BasicStringView<T>::BasicStringView() noexcept
    : m_data(empty)
    , m_size(0)
{
}

template <CharT T>
constexpr BasicStringView<T>::BasicStringView(ConstPointer src) noexcept
{
    if (src == nullptr) [[unlikely]] {
        m_data = empty;
        m_size = 0;
        return;
    }

    m_data = src;
    m_size = memory::StringLength(src);
}

template <CharT T>
constexpr BasicStringView<T>::BasicStringView(ConstPointer src, size_t size) noexcept
{
    if (src == nullptr) [[unlikely]] {
        m_data = empty;
        m_size = 0;
        return;
    }

    m_data = src;
    m_size = size;
}

template <CharT T>
constexpr void BasicStringView<T>::Copy(Pointer dest, size_t index, size_t size) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidIndex(index);
    size_t end = m_size - index;
    size_t len = end < size ? end : size;
    memory::MemCopy(dest, m_data + index, len);
}

template <CharT T>
constexpr void BasicStringView<T>::Copy(Pointer dest, ConstIterator begin, ConstIterator end) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidRange(begin, end);
    memory::MemCopy(dest, begin.Address(), static_cast<size_t>(end - begin));
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::SubFirst(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return BasicStringView(m_data, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::SubLast(size_t size) const noexcept
{
    size_t len = m_size < size ? m_size : size;
    return BasicStringView(m_data + m_size - len, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::SubString(size_t index, size_t size) const noexcept
{
    AssertValidIndex(index);
    size_t end = m_size - index;
    size_t len = end < size ? end : size;
    return BasicStringView(m_data + index, len);
}

template <CharT T>
constexpr BasicStringView<T> BasicStringView<T>::SubString(ConstIterator begin, ConstIterator end) const noexcept
{
    AssertValidRange(begin, end);
    return BasicStringView(begin.Address(), static_cast<size_t>(end - begin));
}

template <CharT T>
constexpr void BasicStringView<T>::RemoveFirst()
{
    if (m_size != 0) [[likely]] {
        ++m_data;
        --m_size;
    }
}

template <CharT T>
constexpr void BasicStringView<T>::RemoveFirst(size_t size)
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_data += size;
    m_size -= size;
}

template <CharT T>
constexpr void BasicStringView<T>::RemoveLast()
{
    if (m_size != 0) [[likely]] {
        --m_size;
    }
}

template <CharT T>
constexpr void BasicStringView<T>::RemoveLast(size_t size)
{
    if (m_size < size) [[unlikely]] {
        size = m_size;
    }

    m_size -= size;
}

template <CharT T>
constexpr BasicStringView<T>::ConstPointer BasicStringView<T>::Data() const noexcept
{
    return m_data;
}

template <CharT T>
constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::Begin() const noexcept
{
    return ConstIterator(m_data, this);
}

template <CharT T>
constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::End() const noexcept
{
    return ConstIterator(m_data + m_size, this);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::First() const
{
    AssertValidIndex(0);
    return *m_data;
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::Last() const
{
    AssertValidIndex(m_size - 1);
    return *(m_data + m_size - 1);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
constexpr size_t BasicStringView<T>::Size() const noexcept
{
    return m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::Empty() const noexcept
{
    return m_size == 0;
}

template <CharT T>
constexpr bool BasicStringView<T>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.Address() - m_data);
    return index < m_size;
}

template <CharT T>
constexpr bool BasicStringView<T>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    size_t beginIdx = static_cast<size_t>(begin.Address() - m_data);
    size_t endIdx = static_cast<size_t>(end.Address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <CharT T>
constexpr BasicStringView<T>::ConstReference BasicStringView<T>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
constexpr BasicStringView<T>& BasicStringView<T>::operator=(ConstPointer ptr) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_data = empty;
        m_size = 0;
        return *this;
    }

    m_data = ptr;
    m_size = memory::StringLength(ptr);
    return *this;
}

template <CharT T>
constexpr void BasicStringView<T>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. view's length is {}", index, m_size);
}

template <CharT T>
constexpr void BasicStringView<T>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at index {}. view's length is {}", iter.Addreess() - m_data, m_size);
}

template <CharT T>
constexpr void BasicStringView<T>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                    [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}. view's length is {}",
           begin.Address() - m_data,
           end.Address() - m_data,
           m_size);
}

export template <CharT T>
constexpr bool operator==(BasicStringView<T> const& lhs, BasicStringView<T> const& rhs) noexcept
{
    size_t size = lhs.Size();
    if (size != rhs.Size()) {
        return false;
    }

    using ConstPointer = BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = lhs.Data();
    ConstPointer rbuf = rhs.Data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U>
constexpr bool operator==(BasicStringView<T> const& lhs, BasicStringView<U> const& rhs) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = lhs.Size();
    if (size != rhs.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = lhs.Data();
    typename BasicStringView<U>::ConstPointer rbuf = rhs.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, StringLikeT<T> U>
constexpr bool operator==(BasicStringView<T> const& str, U const& src) noexcept
{
    BasicStringView<T> view = src;
    size_t size = str.Size();

    if (size != view.Size()) {
        return false;
    }

    using ConstPointer = BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = str.Data();
    ConstPointer rbuf = view.Data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U, StringLikeT<U> ViewU>
constexpr bool operator==(BasicStringView<T> const& str, ViewU const& src) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<U> view = src;
    size_t size = str.Size();

    if (size != view.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = str.Data();
    typename BasicStringView<U>::ConstPointer rbuf = view.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
constexpr bool operator==(BasicStringView<T> const& str, nullptr_t) = delete;

} // namespace mini