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

namespace fmt {

export template <mini::CharT T, mini::AllocatorT<T> AllocT>
auto format_as(mini::BasicString<T, AllocT> const& str)
{
    return fmt::basic_string_view<T>(str.Data(), str.Size());
}

} // namespace fmt