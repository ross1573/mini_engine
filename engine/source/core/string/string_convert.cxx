export module mini.core:string_convert;

import convert_utf;
import :type;
import :memory_operation;
import :string;
import :string_view;

namespace mini {

export template <CharT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class BasicStringConvert;

export using StringConvert = BasicStringConvert<char>;
export using WStringConvert = BasicStringConvert<wchar>;
export using U8StringConvert = BasicStringConvert<char8>;
export using U16StringConvert = BasicStringConvert<char16>;
export using U32StringConvert = BasicStringConvert<char32>;

template <CharT T, AllocatorT<T> AllocT>
class BasicStringConvert {
public:
    typedef BasicString<T, AllocT>::Value Value;
    typedef BasicString<T, AllocT>::Pointer Pointer;
    typedef BasicString<T, AllocT>::Reference Reference;
    typedef BasicString<T, AllocT>::ConstPointer ConstPointer;

private:
    BasicString<T, AllocT> m_data;

public:
    constexpr BasicStringConvert(BasicStringConvert&&) noexcept = default;
    constexpr ~BasicStringConvert() noexcept = default;

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(U const* src);

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(BasicString<U> const& src);

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(BasicStringView<U> src);

    [[nodiscard]] constexpr size_t size() const noexcept;
    [[nodiscard]] constexpr ConstPointer data() const noexcept;

    [[nodiscard]] constexpr BasicString<T, AllocT> toString() const;
    constexpr operator BasicString<T, AllocT>() const;
    constexpr operator BasicStringView<T>() const noexcept;

// https://stackoverflow.com/questions/78347691/overloading-ref-qualified-member-function-without-ref-qualifier
// seem like it has been valid recently, and msvc hasn't updated it yet.
#if !(MSVC)
    [[nodiscard]] constexpr BasicString<T, AllocT> toString() && noexcept;
    constexpr operator BasicString<T, AllocT>() && noexcept;
#endif

    constexpr BasicStringConvert& operator=(BasicStringConvert&&) noexcept = default;

public:
    BasicStringConvert(nullptr_t) = delete;
    BasicStringConvert(BasicStringConvert const&) = delete;
    BasicStringConvert& operator=(nullptr_t) = delete;
    BasicStringConvert& operator=(BasicStringConvert const&) = delete;

private:
    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr void convert(BasicStringView<U> const& str);
};

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(U const* src)
    : m_data{}
{
    BasicStringView<U> view = src;
    convert(view);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(BasicString<U> const& src)
    : m_data{}
{
    BasicStringView<U> view = src;
    convert(view);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(BasicStringView<U> src)
    : m_data{}
{
    convert(src);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr size_t BasicStringConvert<T, AllocT>::size() const noexcept
{
    return m_data.size();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicStringConvert<T, AllocT>::ConstPointer BasicStringConvert<T, AllocT>::data() const noexcept
{
    return m_data.data();
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT> BasicStringConvert<T, AllocT>::toString() const
{
    return m_data;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicStringConvert<T, AllocT>::operator BasicString<T, AllocT>() const
{
    return m_data;
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicStringConvert<T, AllocT>::operator BasicStringView<T>() const noexcept
{
    return static_cast<BasicStringView<T>>(m_data);
}

#if !(MSVC)
template <CharT T, AllocatorT<T> AllocT>
constexpr BasicString<T, AllocT> BasicStringConvert<T, AllocT>::toString() && noexcept
{
    return moveCast(m_data);
}

template <CharT T, AllocatorT<T> AllocT>
constexpr BasicStringConvert<T, AllocT>::operator BasicString<T, AllocT>() && noexcept
{
    return moveCast(m_data);
}
#endif

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
constexpr void BasicStringConvert<T, AllocT>::convert(BasicStringView<U> const& str)
{
    if (str.size() == 0) {
        return;
    }

    U const* begin = str.data();
    U const* end = str.data() + str.size();

    size_t size = static_cast<size_t>(utf::ConvertLength(begin, end, Value(0)));
    m_data.resize(size);
    T* dst = m_data.data();

    utf::Convert(begin, end, dst, dst + size);
}

export template <CharT T>
constexpr auto format_as(BasicStringConvert<T> const& convert) // NOLINT
{
    return static_cast<BasicStringView<T>>(convert);
}

} // namespace mini