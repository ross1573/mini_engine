export module mini.core:string_convert;

import convert_utf;
import :type;
import :string;
import :string_view;

namespace mini {

template <CharT From, CharT To, AllocatorT<To> AllocT>
constexpr BasicString<To, AllocT> convertImpl(BasicStringView<From> from)
{
    if (from.size() == 0) {
        return BasicString<To, AllocT>{};
    }

    From const* begin = from.data();
    From const* end = from.data() + from.size();

    size_t size = static_cast<size_t>(utf::ConvertLength(begin, end, To{0}));
    BasicString<To, AllocT> result;
    result.resize(size);

    To* dest = result.data();
    utf::Convert(begin, end, dest, dest + size);
    return result;
}

export template <CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr BasicString<To, AllocT> convert(BasicStringView<char> from)
{
    return convertImpl<char, To, AllocT>(from);
}

export template <CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr BasicString<To, AllocT> convert(BasicStringView<wchar_t> from)
{
    return convertImpl<wchar_t, To, AllocT>(from);
}

export template <CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr BasicString<To, AllocT> convert(BasicStringView<char8> from)
{
    return convertImpl<char8, To, AllocT>(from);
}

export template <CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr BasicString<To, AllocT> convert(BasicStringView<char16> from)
{
    return convertImpl<char16, To, AllocT>(from);
}

export template <CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr BasicString<To, AllocT> convert(BasicStringView<char32> from)
{
    return convertImpl<char32, To, AllocT>(from);
}

export template <CharT From, CharT To, AllocatorT<To> AllocT = Allocator<To>>
constexpr void convertToImpl(BasicString<To, AllocT>& to, BasicStringView<From> from)
{
    if (from.size() == 0) {
        return;
    }

    From const* begin = from.data();
    From const* end = from.data() + from.size();

    size_t size = static_cast<size_t>(utf::ConvertLength(begin, end, To{0}));
    size_t prevSize = to.size();

    to.resize(size + prevSize);
    To const* dest = to.data() + prevSize;
    utf::Convert(from.begin(), from.end(), dest, dest + size);
}

} // namespace mini