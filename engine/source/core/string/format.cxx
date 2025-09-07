export module mini.core:format;

import :type;
import :string;
import :string_view;
import fmt;

namespace mini {

export template <typename... Args>
auto Format(StringView msg, Args&&... args)
{
    return fmt::vformat(msg.Data(), fmt::make_format_args(args...));
}

export template <mini::CharT T, mini::AllocatorT<T> AllocT>
auto format_as(mini::BasicString<T, AllocT> const& str)
{
    return fmt::basic_string_view<T>(str.Data(), str.Size());
}

export template <mini::CharT T>
auto format_as(mini::BasicStringView<T> const& str)
{
    return fmt::basic_string_view<T>(str.Data(), str.Size());
}

} // namespace mini