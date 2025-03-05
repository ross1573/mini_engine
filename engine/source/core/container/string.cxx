module;

#include <string>
#include <string_view>
#include <format>

export module mini.core:string;

import :type;

export namespace mini
{

template <typename T>
concept CharT = AnyOfT<RemoveConstVolatileT<T>,
    char,
    wchar_t,
    char8_t,
    char16_t,
    char32_t>;

using String = std::basic_string<char>;
using StringView = std::basic_string_view<char>;

using WideString = std::basic_string<wchar_t>;
using WideStringView = std::basic_string_view<wchar_t>;

template <typename... Args>
auto Format(StringView msg, Args&&... args)
{
    return std::vformat(msg, std::make_format_args(args...));
}

} // namespace mini