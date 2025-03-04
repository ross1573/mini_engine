module;

#include <string>
#include <string_view>
#include <format>

export module mini.core:string;

export namespace mini
{

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