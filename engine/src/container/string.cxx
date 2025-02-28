module;

#include <string>
#include <string_view>
#include <format>
#include "core/define.h"

export module mini.container:string;

export namespace mini
{

using String = std::basic_string<char>;
using StringView = std::basic_string_view<char>;

using WideString = std::basic_string<wchar_t>;
using WideStringView = std::basic_string_view<wchar_t>;

template <typename... Args>
constexpr auto Format(StringView msg, Args&&... args)
{
    return std::vformat(msg, std::make_format_args(args...));
}

} // namespace mini