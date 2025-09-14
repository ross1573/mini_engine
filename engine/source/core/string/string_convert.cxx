export module mini.core:string_convert;

import convert_utf;
import :type;
import :string;
import :string_view;
import :allocator;

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
    typedef typename BasicString<T, AllocT>::Value Value;
    typedef typename BasicString<T, AllocT>::Ptr Ptr;
    typedef typename BasicString<T, AllocT>::Ref Ref;
    typedef typename BasicString<T, AllocT>::ConstPtr ConstPtr;

private:
    BasicString<T, AllocT> m_Data;

public:
    constexpr BasicStringConvert(BasicStringConvert&&) noexcept = default;
    constexpr ~BasicStringConvert() noexcept = default;

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(U const*);

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(BasicString<U> const&);

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr BasicStringConvert(BasicStringView<U>);

    constexpr SizeT Size() const noexcept;
    constexpr ConstPtr Data() const noexcept;

    constexpr BasicString<T, AllocT> ToString() const;
    constexpr BasicString<T, AllocT> ToString() && noexcept;

    constexpr operator BasicString<T, AllocT>() const;
    constexpr operator BasicString<T, AllocT>() && noexcept;
    constexpr operator BasicStringView<T>() const noexcept;

    constexpr BasicStringConvert& operator=(BasicStringConvert&&) noexcept = default;

private:
    BasicStringConvert(NullptrT) = delete;
    BasicStringConvert(BasicStringConvert const&) = delete;
    BasicStringConvert& operator=(NullptrT) = delete;
    BasicStringConvert& operator=(BasicStringConvert const&) = delete;

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr void Convert(BasicStringView<U> const&);
};

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(U const* src)
    : m_Data{}
{
    BasicStringView<U> view = src;
    Convert(view);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(BasicString<U> const& src)
    : m_Data{}
{
    BasicStringView<U> view = src;
    Convert(view);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr BasicStringConvert<T, AllocT>::BasicStringConvert(BasicStringView<U> src)
    : m_Data{}
{
    Convert(src);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT BasicStringConvert<T, AllocT>::Size() const noexcept
{
    return m_Data.Size();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicStringConvert<T, AllocT>::ConstPtr
BasicStringConvert<T, AllocT>::Data() const noexcept
{
    return m_Data.Data();
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> BasicStringConvert<T, AllocT>::ToString() const
{
    return m_Data;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicString<T, AllocT> BasicStringConvert<T, AllocT>::ToString() && noexcept
{
    return MoveArg(m_Data);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicStringConvert<T, AllocT>::operator BasicString<T, AllocT>() const
{
    return m_Data;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicStringConvert<T, AllocT>::operator BasicString<T, AllocT>() && noexcept
{
    return MoveArg(m_Data);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr BasicStringConvert<T, AllocT>::operator BasicStringView<T>() const noexcept
{
    return static_cast<BasicStringView<T>>(m_Data);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr void BasicStringConvert<T, AllocT>::Convert(BasicStringView<U> const& str)
{
    if (str.Size() == 0) {
        return;
    }

    U const* begin = str.Data();
    U const* end = str.Data() + str.Size();

    SizeT size = (SizeT)utf::ConvertLength(begin, end, Value(0));
    m_Data.Resize(size);
    T* dst = m_Data.Data();

    utf::Convert(begin, end, dst, dst + size);
}

export template <CharT T>
inline constexpr auto format_as(BasicStringConvert<T> const& convert)
{
    return static_cast<BasicStringView<T>>(convert);
}

} // namespace mini