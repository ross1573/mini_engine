module;

#include <string_view>

export module mini.core:string_view;

import :type;
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
    SizeT m_size;

    static constexpr T empty[1] = { '\0' };

public:
    constexpr BasicStringView() noexcept;
    constexpr ~BasicStringView() = default;
    constexpr BasicStringView(BasicStringView const&) noexcept;
    constexpr BasicStringView(ConstPointer) noexcept;
    constexpr BasicStringView(ConstPointer, SizeT) noexcept;

    constexpr void Copy(Pointer, SizeT, SizeT) const noexcept;
    constexpr void Copy(Pointer, ConstIterator, ConstIterator) const noexcept;
    constexpr BasicStringView SubFirst(SizeT) const noexcept;
    constexpr BasicStringView SubLast(SizeT) const noexcept;
    constexpr BasicStringView SubString(SizeT, SizeT) const noexcept;
    constexpr BasicStringView SubString(ConstIterator, ConstIterator) const noexcept;

    constexpr void RemoveFirst();
    constexpr void RemoveFirst(SizeT);
    constexpr void RemoveLast();
    constexpr void RemoveLast(SizeT);

    constexpr ConstPointer Data() const noexcept;
    constexpr ConstIterator Begin() const noexcept;
    constexpr ConstIterator End() const noexcept;
    constexpr ConstReference First() const;
    constexpr ConstReference Last() const;
    constexpr ConstReference At(SizeT) const;

    constexpr SizeT Size() const noexcept;
    constexpr bool Empty() const noexcept;
    constexpr bool ValidIndex(SizeT) const noexcept;
    constexpr bool ValidIterator(ConstIterator) const noexcept;
    constexpr bool ValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr ConstReference operator[](SizeT) const;

    constexpr BasicStringView& operator=(BasicStringView const&) noexcept;
    constexpr BasicStringView& operator=(ConstPointer) noexcept;

    constexpr operator std::basic_string_view<T>() const noexcept;

    constexpr BasicStringView(std::basic_string_view<T> const&) noexcept;

private:
    BasicStringView(NullptrT) = delete;
    BasicStringView(NullptrT, SizeT) = delete;
    BasicStringView& operator=(NullptrT) = delete;

    constexpr void AssertValidIndex(SizeT) const noexcept;
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
inline constexpr BasicStringView<T>::BasicStringView(ConstPointer ptr, SizeT len) noexcept
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
inline constexpr void BasicStringView<T>::Copy(Pointer dest, SizeT start,
                                               SizeT count) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidIndex(start);
    SizeT end = m_size - start;
    SizeT len = end < count ? end : count;
    memory::MemCopy(dest, m_data + start, len);
}

template <CharT T>
inline constexpr void BasicStringView<T>::Copy(Pointer dest, ConstIterator begin,
                                               ConstIterator end) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidRange(begin, end);
    memory::MemCopy(dest, begin.Address(), static_cast<SizeT>(end - begin));
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubFirst(SizeT count) const noexcept
{
    SizeT len = m_size < count ? m_size : count;
    return BasicStringView(m_data, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubLast(SizeT count) const noexcept
{
    SizeT len = m_size < count ? m_size : count;
    return BasicStringView(m_data + m_size - len, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubString(SizeT start,
                                                                  SizeT count) const noexcept
{
    AssertValidIndex(start);
    SizeT end = m_size - start;
    SizeT len = end < count ? end : count;
    return BasicStringView(m_data + start, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubString(ConstIterator begin,
                                                                  ConstIterator end) const noexcept
{
    AssertValidRange(begin, end);
    return BasicStringView(begin.Address(), static_cast<SizeT>(end - begin));
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
inline constexpr void BasicStringView<T>::RemoveFirst(SizeT count)
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
inline constexpr void BasicStringView<T>::RemoveLast(SizeT count)
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
inline constexpr BasicStringView<T>::ConstReference BasicStringView<T>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
inline constexpr SizeT BasicStringView<T>::Size() const noexcept
{
    return m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::Empty() const noexcept
{
    return m_size == 0;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidIndex(SizeT index) const noexcept
{
    return index < m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.Address() - m_data);
    return index < m_size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::ValidRange(ConstIterator begin,
                                                     ConstIterator end) const noexcept
{
    SizeT beginIdx = static_cast<SizeT>(begin.Address() - m_data);
    SizeT endIdx = static_cast<SizeT>(end.Address() - m_data);
    return (beginIdx < m_size) && (endIdx <= m_size);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstReference
BasicStringView<T>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(m_data + index);
}

template <CharT T>
inline constexpr BasicStringView<T>&
BasicStringView<T>::operator=(BasicStringView const& other) noexcept
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
inline constexpr BasicStringView<T>::
    BasicStringView(std::basic_string_view<T> const& other) noexcept
    : m_data(other.data())
    , m_size(static_cast<SizeT>(other.size()))
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
    return BasicStringView<T>(other.data(), static_cast<SizeT>(other.size()));
}

export template <CharT T>
inline constexpr std::basic_string_view<T> ToStdStringView(BasicStringView<T> const& other) noexcept
{
    return std::basic_string_view<T>(other.Data(), other.Size());
}

template <CharT T>
inline constexpr void
BasicStringView<T>::AssertValidIndex([[maybe_unused]] SizeT index) const noexcept
{
    ASSERT(ValidIndex(index));
}

template <CharT T>
inline constexpr void
BasicStringView<T>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(ValidIterator(iter));
}

template <CharT T>
inline constexpr void
BasicStringView<T>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                     [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(ValidRange(begin, end));
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& l, BasicStringView<T> const& r) noexcept
{
    SizeT size = l.Size();
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
    SizeT size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = l.Data();
    typename BasicStringView<U>::ConstPointer rbuf = r.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& l,
                                 std::basic_string_view<T> const& r) noexcept
{
    SizeT size = l.Size();
    if (size != static_cast<SizeT>(r.Size())) {
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
inline constexpr bool operator==(BasicStringView<T> const& l,
                                 std::basic_string_view<U> const& r) noexcept
    requires EqualityComparableWithT<T, U>
{
    SizeT size = l.Size();
    if (size != static_cast<SizeT>(r.Size())) {
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
    SizeT size = s.Size();

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
    SizeT size = s.Size();

    if (size != view.Size()) {
        return false;
    }

    typename BasicStringView<T>::ConstPointer lbuf = s.Data();
    typename BasicStringView<U>::ConstPointer rbuf = view.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& s, NullptrT) = delete;

} // namespace mini