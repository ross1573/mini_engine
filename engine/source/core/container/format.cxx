module;

#include <format>

export module mini.core:format;

import :string_view;

namespace mini {

export template <typename... Args>
auto Format(StringView msg, Args&&... args)
{
    return std::vformat(msg, std::make_format_args(args...));
}

} // namespace mini