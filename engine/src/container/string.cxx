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
// TODO: might be a bug..? 
// https://developercommunity.visualstudio.com/t/Spurious-C4686-warning-when-using-import/10825076?sort=newest&type=idea
#ifdef MSVC
#   pragma warning(push)
#   pragma warning(disable: 4686)
#endif
    return std::vformat(msg, std::make_format_args(args...));
#ifdef MSVC
#   pragma warning(pop)
#endif
}

} // namespace mini