module;

#include <format>
#include <string>

export module mini.core:format;

import :type;
import :string;
import :string_view;
import fmt;

namespace mini {

export template <typename... Args>
auto Format(StringView msg, Args&&... args)
{
    return fmt::vformat(msg, fmt::make_format_args(args...));
}

} // namespace mini

namespace std {

// temporary formatter
export template <mini::CharT T>
struct formatter<mini::BasicString<T>> : formatter<std::string> {
public:
    auto format(mini::BasicString<T> const& str, format_context& ctx) const
    {
        return formatter<std::string>::format(str.Data(), ctx);
    }
};

} // namespace std