module;

#include <string_view>

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
    constexpr BasicStringView(BasicStringView const&) noexcept;
    constexpr BasicStringView(ConstPointer) noexcept;
    constexpr BasicStringView(ConstPointer, size_t) noexcept;

    constexpr void Copy(Pointer, size_t, size_t) const noexcept;
    constexpr void Copy(Pointer, ConstIterator, ConstIterator) const noexcept;
    constexpr BasicStringView SubFirst(size_t) const noexcept;
    constexpr BasicStringView SubLast(size_t) const noexcept;
    constexpr BasicStringView SubString(size_t, size_t) const noexcept;
    constexpr BasicStringView SubString(ConstIterator, ConstIterator) const noexcept;

    constexpr void RemoveFirst();
    constexpr void RemoveFirst(size_t);
    constexpr void RemoveLast();
    constexpr void RemoveLast(size_t);

    constexpr ConstPointer Data() const noexcept;
    constexpr ConstIterator Begin() const noexcept;
    constexpr ConstIterator End() const noexcept;
    constexpr ConstReference First() const;
    constexpr ConstReference Last() const;
    constexpr ConstReference At(size_t) const;

    constexpr size_t Size() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool ValidIndex(size_t) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr ConstReference operator[](size_t) const;

    constexpr BasicStringView& operator=(BasicStringView const&) noexcept;
    constexpr BasicStringView& operator=(ConstPointer) noexcept;

    constexpr operator std::basic_string_view<T>() const noexcept;

    constexpr BasicStringView(std::basic_string_view<T> const&) noexcept;

private:
    BasicStringView(nullptr_t) = delete;
    BasicStringView(nullptr_t, size_t) = delete;
    BasicStringView& operator=(nullptr_t) = delete;

    constexpr void AssertValidIndex(size_t) const noexcept;
    constexpr void AssertValidIterator(ConstIterator) const noexcept;
    constexpr void AssertValidRange(ConstIterator, ConstIterator) const noexcept;
};

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView() noexcept
    : m_data(empty)
    , m_size(0)
{
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(BasicStringView const& other) noexcept
    : m_data(other.m_data)
    , m_size(other.m_size)
{
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(ConstPointer ptr) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_data = empty;
        m_size = 0;
        return;
    }

    m_data = ptr;
    m_size = memory::StringLength(ptr);
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(ConstPointer ptr, size_t len) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_data = empty;
        m_size = 0;
        return;
    }

    m_data = ptr;
    m_size = len;
}

template <CharT T>
inline constexpr void BasicStringView<T>::Copy(Pointer dest, size_t start, size_t count) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidIndex(start);
    size_t end = m_size - start;
    size_t len = end < count ? end : count;
    memory::MemCopy(dest, m_data + start, len);
}

template <CharT T>
inline constexpr void BasicStringView<T>::Copy(Pointer dest, ConstIterator begin, ConstIterator end) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidRange(begin, end);
    memory::MemCopy(dest, begin.Address(), static_cast<size_t>(end - begin));
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubFirst(size_t count) const noexcept
{
    size_t len = m_size < count ? m_size : count;
    return BasicStringView(m_data, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubLast(size_t count) const noexcept
{
    size_t len = m_size < count ? m_size : count;
    return BasicStringView(m_data + m_size - len, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubString(size_t start, size_t count) const noexcept
{
    AssertValidIndex(start);
    size_t end = m_size - start;
    size_t len = end < count ? end : count;
    return BasicStringView(m_data + start, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubString(ConstIterator begin, ConstIterator end) const noexcept
{
    AssertValidRange(begin, end);
    return BasicStringView(begin.Address(), static_cast<size_t>(end - begin));
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveFirst()
{
    if (m_size != 0) [[likely]] {
        ++m_data;
        --m_size;
    }
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveFirst(size_t count)
{
    if (m_size < count) [[unlikely]] {
        count = m_size;
    }

    m_data += count;
    m_size -= count;
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveLast()
{
    if (m_size != 0) [[likely]] {
        --m_size;
    }
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveLast(size_t count)
{
    if (m_size < count) [[unlikely]] {
        count = m_size;
    }

    m_size -= count;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstPointer BasicStringView<T>::Data() const noexcept
{
    return m_data;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::Begin() const noexcept
{
    return ConstIterator(m_data, this);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::End() const noexcept
{
    return ConstIterator(m_data + m_size, this);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstReference BasicStringView<T>::First() const
{
    AssertValidIndex(0);
    return *m_data;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstReference BasicStringView<T>::Last() const
{
    AssertValidIndex(m_size - 1);
    return *(m_data + m_size - 1);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstReference BasicStringView<T>::At(size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
inline constexpr size_t BasicStringView<T>::Size() const noexcept
{
    return m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::Empty() const noexcept
{
    return m_size == 0;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidIndex(size_t index) const noexcept
{
    return index < m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidIterator(ConstIterator iter) const noexcept
{
    size_t index = static_cast<size_t>(iter.Address() - m_data);
    return index < m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidRange(ConstIterator begin, ConstIterator end) const noexcept
{
    size_t beginIdx = static_cast<size_t>(begin.Address() - m_data);
    size_t endIdx = static_cast<size_t>(end.Address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstReference BasicStringView<T>::operator[](size_t index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
inline constexpr BasicStringView<T>& BasicStringView<T>::operator=(BasicStringView const& other) noexcept
{
    m_data = other.m_data;
    m_size = other.m_size;
    return *this;
}

template <CharT T>
inline constexpr BasicStringView<T>& BasicStringView<T>::operator=(ConstPointer ptr) noexcept
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
inline constexpr BasicStringView<T>::BasicStringView(std::basic_string_view<T> const& other) noexcept
    : m_data(other.data())
    , m_size(static_cast<size_t>(other.size()))
{
}

template <CharT T>
inline constexpr BasicStringView<T>::operator std::basic_string_view<T>() const noexcept
{
    return std::basic_string_view<T>(m_data, m_size);
}

export template <CharT T>
inline constexpr BasicStringView<T> ToStringView(std::basic_string_view<T> const& other) noexcept
{
    return BasicStringView<T>(other.data(), static_cast<size_t>(other.size()));
}

export template <CharT T>
inline constexpr std::basic_string_view<T> ToStdStringView(BasicStringView<T> const& other) noexcept
{
    return std::basic_string_view<T>(other.Data(), other.Size());
}

template <CharT T>
inline constexpr void BasicStringView<T>::AssertValidIndex([[maybe_unused]] size_t index) const noexcept
{
    ASSERT(ValidIndex(index), "invalid index {}. view's length is {}", index, m_size);
}

template <CharT T>
inline constexpr void BasicStringView<T>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter), "invalid iterator at index {}. view's length is {}", iter.Addreess() - m_data, m_size);
}

template <CharT T>
inline constexpr void BasicStringView<T>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                                           [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end),
           "invalid range from {} to {}. view's length is {}",
           begin.Address() - m_data,
           end.Address() - m_data,
           m_size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& l, BasicStringView<T> const& r) noexcept
{
    size_t size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    using ConstPointer = BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = l.Data();
    ConstPointer rbuf = r.Data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U>
inline constexpr bool operator==(BasicStringView<T> const& l, BasicStringView<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = l.Data();
    typename BasicStringView<U>::ConstPointer rbuf = r.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& l, std::basic_string_view<T> const& r) noexcept
{
    size_t size = l.Size();
    if (size != static_cast<size_t>(r.Size())) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = l.Data();
    typename std::basic_string_view<T>::const_pointer rbuf = r.data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U>
inline constexpr bool operator==(BasicStringView<T> const& l, std::basic_string_view<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    size_t size = l.Size();
    if (size != static_cast<size_t>(r.Size())) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = l.Data();
    typename std::basic_string_view<T>::const_pointer rbuf = r.data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T, StringLikeT<T> U>
inline constexpr bool operator==(BasicStringView<T> const& s, U const& src) noexcept
{
    BasicStringView<T> view = src;
    size_t size = s.Size();

    if (size != view.Size()) {
        return false;
    }

    using ConstPointer = typename BasicStringView<T>::ConstPointer;
    ConstPointer lbuf = s.Data();
    ConstPointer rbuf = view.Data();

    if (lbuf == rbuf) [[unlikely]] {
        return true;
    }

    return memory::StringCompare(lbuf, rbuf, size) == 0;
}

export template <CharT T, CharT U, StringLikeT<U> ViewU>
inline constexpr bool operator==(BasicStringView<T> const& s, ViewU const& src) noexcept
    requires EqualityComparableWithT<T, U>
{
    BasicStringView<U> view = src;
    size_t size = s.Size();

    if (size != view.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = s.Data();
    typename BasicStringView<U>::ConstPointer rbuf = view.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& s, nullptr_t) = delete;

} // namespace mini