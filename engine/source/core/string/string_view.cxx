module;

#include <string_view>

export module mini.core:string_view;

import :type;
import :memory;
import :string_utility;
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
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;
    using ConstIterator = ArrayIterator<ConstValue, BasicStringView const>;

private:
    ConstPtr m_Data;
    SizeT m_Size;

    static constexpr T m_Empty[1] = { '\0' };

public:
    constexpr BasicStringView() noexcept;
    constexpr ~BasicStringView() = default;
    constexpr BasicStringView(BasicStringView const&) noexcept;
    constexpr BasicStringView(ConstPtr) noexcept;
    constexpr BasicStringView(ConstPtr, SizeT) noexcept;

    constexpr void Copy(Ptr, SizeT, SizeT) const noexcept;
    constexpr void Copy(Ptr, ConstIterator, ConstIterator) const noexcept;
    constexpr BasicStringView SubFirst(SizeT) const noexcept;
    constexpr BasicStringView SubLast(SizeT) const noexcept;
    constexpr BasicStringView SubString(SizeT, SizeT) const noexcept;
    constexpr BasicStringView SubString(ConstIterator, ConstIterator) const noexcept;

    constexpr void RemoveFirst();
    constexpr void RemoveFirst(SizeT);
    constexpr void RemoveLast();
    constexpr void RemoveLast(SizeT);

    constexpr ConstPtr Data() const noexcept;
    constexpr ConstIterator Begin() const noexcept;
    constexpr ConstIterator End() const noexcept;
    constexpr ConstRef First() const;
    constexpr ConstRef Last() const;
    constexpr ConstRef At(SizeT) const;

    constexpr SizeT Size() const noexcept;
    constexpr bool IsEmpty() const noexcept;
    constexpr bool IsValidIndex(SizeT) const noexcept;
    constexpr bool IsValidIterator(ConstIterator) const noexcept;
    constexpr bool IsValidRange(ConstIterator, ConstIterator) const noexcept;

    constexpr ConstRef operator[](SizeT) const;

    constexpr BasicStringView& operator=(BasicStringView const&) noexcept;
    constexpr BasicStringView& operator=(ConstPtr) noexcept;

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
    : m_Data(m_Empty)
    , m_Size(0)
{
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(BasicStringView const& other) noexcept
    : m_Data(other.m_Data)
    , m_Size(other.m_Size)
{
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(ConstPtr ptr) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_Data = m_Empty;
        m_Size = 0;
        return;
    }

    m_Data = ptr;
    m_Size = memory::StringLength(ptr);
}

template <CharT T>
inline constexpr BasicStringView<T>::BasicStringView(ConstPtr ptr, SizeT len) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_Data = m_Empty;
        m_Size = 0;
        return;
    }

    m_Data = ptr;
    m_Size = len;
}

template <CharT T>
inline constexpr void BasicStringView<T>::Copy(Ptr dest, SizeT start, SizeT count) const noexcept
{
    if (dest == nullptr) [[unlikely]] {
        return;
    }

    AssertValidIndex(start);
    SizeT end = m_Size - start;
    SizeT len = end < count ? end : count;
    memory::MemCopy(dest, m_Data + start, len);
}

template <CharT T>
inline constexpr void BasicStringView<T>::Copy(Ptr dest, ConstIterator begin,
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
    SizeT len = m_Size < count ? m_Size : count;
    return BasicStringView(m_Data, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubLast(SizeT count) const noexcept
{
    SizeT len = m_Size < count ? m_Size : count;
    return BasicStringView(m_Data + m_Size - len, len);
}

template <CharT T>
inline constexpr BasicStringView<T> BasicStringView<T>::SubString(SizeT start,
                                                                  SizeT count) const noexcept
{
    AssertValidIndex(start);
    SizeT end = m_Size - start;
    SizeT len = end < count ? end : count;
    return BasicStringView(m_Data + start, len);
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
    if (m_Size != 0) [[likely]] {
        ++m_Data;
        --m_Size;
    }
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveFirst(SizeT count)
{
    if (m_Size < count) [[unlikely]] {
        count = m_Size;
    }

    m_Data += count;
    m_Size -= count;
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveLast()
{
    if (m_Size != 0) [[likely]] {
        --m_Size;
    }
}

template <CharT T>
inline constexpr void BasicStringView<T>::RemoveLast(SizeT count)
{
    if (m_Size < count) [[unlikely]] {
        count = m_Size;
    }

    m_Size -= count;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstPtr BasicStringView<T>::Data() const noexcept
{
    return m_Data;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::Begin() const noexcept
{
    return ConstIterator(m_Data, this);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstIterator BasicStringView<T>::End() const noexcept
{
    return ConstIterator(m_Data + m_Size, this);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstRef BasicStringView<T>::First() const
{
    AssertValidIndex(0);
    return *m_Data;
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstRef BasicStringView<T>::Last() const
{
    AssertValidIndex(m_Size - 1);
    return *(m_Data + m_Size - 1);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstRef BasicStringView<T>::At(SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Data + index);
}

template <CharT T>
inline constexpr SizeT BasicStringView<T>::Size() const noexcept
{
    return m_Size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::IsEmpty() const noexcept
{
    return m_Size == 0;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::IsValidIndex(SizeT index) const noexcept
{
    return index < m_Size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::IsValidIterator(ConstIterator iter) const noexcept
{
    SizeT index = static_cast<SizeT>(iter.Address() - m_Data);
    return index < m_Size;
}

template <CharT T>
inline constexpr bool BasicStringView<T>::IsValidRange(ConstIterator begin,
                                                       ConstIterator end) const noexcept
{
    SizeT beginIdx = static_cast<SizeT>(begin.Address() - m_Data);
    SizeT endIdx = static_cast<SizeT>(end.Address() - m_Data);
    return (beginIdx < m_Size) && (endIdx <= m_Size);
}

template <CharT T>
inline constexpr BasicStringView<T>::ConstRef BasicStringView<T>::operator[](SizeT index) const
{
    AssertValidIndex(index);
    return *(m_Data + index);
}

template <CharT T>
inline constexpr BasicStringView<T>&
BasicStringView<T>::operator=(BasicStringView const& other) noexcept
{
    m_Data = other.m_Data;
    m_Size = other.m_Size;
    return *this;
}

template <CharT T>
inline constexpr BasicStringView<T>& BasicStringView<T>::operator=(ConstPtr ptr) noexcept
{
    if (ptr == nullptr) [[unlikely]] {
        m_Data = m_Empty;
        m_Size = 0;
        return *this;
    }

    m_Data = ptr;
    m_Size = memory::StringLength(ptr);
    return *this;
}

template <CharT T>
inline constexpr BasicStringView<T>::
    BasicStringView(std::basic_string_view<T> const& other) noexcept
    : m_Data(other.data())
    , m_Size(static_cast<SizeT>(other.size()))
{
}

template <CharT T>
inline constexpr BasicStringView<T>::operator std::basic_string_view<T>() const noexcept
{
    return std::basic_string_view<T>(m_Data, m_Size);
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
    ASSERT(IsValidIndex(index));
}

template <CharT T>
inline constexpr void
BasicStringView<T>::AssertValidIterator([[maybe_unused]] ConstIterator iter) const noexcept
{
    ASSERT(IsValidIterator(iter));
}

template <CharT T>
inline constexpr void
BasicStringView<T>::AssertValidRange([[maybe_unused]] ConstIterator begin,
                                     [[maybe_unused]] ConstIterator end) const noexcept
{
    ASSERT(IsValidRange(begin, end));
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& l, BasicStringView<T> const& r) noexcept
{
    SizeT size = l.Size();
    if (size != r.Size()) {
        return false;
    }

    using ConstPtr = BasicStringView<T>::ConstPtr;
    ConstPtr lbuf = l.Data();
    ConstPtr rbuf = r.Data();

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

    typename BasicStringView<T>::ConstPtr lbuf = l.Data();
    typename BasicStringView<U>::ConstPtr rbuf = r.Data();
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

    typename BasicStringView<T>::ConstPtr lbuf = l.Data();
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

    typename BasicStringView<T>::ConstPtr lbuf = l.Data();
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

    using ConstPtr = typename BasicStringView<T>::ConstPtr;
    ConstPtr lbuf = s.Data();
    ConstPtr rbuf = view.Data();

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

    typename BasicStringView<T>::ConstPtr lbuf = s.Data();
    typename BasicStringView<U>::ConstPtr rbuf = view.Data();
    return memory::EqualRange(lbuf, rbuf, rbuf + size);
}

export template <CharT T>
inline constexpr bool operator==(BasicStringView<T> const& s, NullptrT) = delete;

} // namespace mini