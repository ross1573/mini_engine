export module mini.core:format;

export import fmt;

import :type;
import :memory_operation;
import :string;
import :string_view;

namespace mini {

inline constexpr auto WriteFormatError(String& to, StringView const& msg,
                                       fmt::format_error const& error)
{
    StringView errorMsg = error.what();
    StringView pre = errorMsg.Empty() ? "format failed" : "format failed with error: ";
    SizeT totalLen = to.Size() + msg.Size() + pre.Size() + errorMsg.Size() + 3;

    to.Reserve(totalLen);
    to.Append(msg);
    to.Append(" (", 2);
    to.Append(pre);
    to.Append(errorMsg);
    to.Push(')');
    return to;
}

export template <typename... Args>
inline constexpr void FormatTo(String& to, StringView msg, Args&&... args)
{
    auto fmtMsg = fmt::string_view(msg.Data(), msg.Size());
    auto buf = fmt::memory_buffer();

    try {
        fmt::vformat_to(fmt::appender(buf), fmtMsg, fmt::make_format_args(args...));
    } catch (fmt::format_error const& error) {
        WriteFormatError(to, msg, error);
        return;
    }

    return to.Append(buf.data(), buf.size());
}

export template <typename... Args>
inline constexpr String Format(StringView msg, Args&&... args)
{
    auto fmtMsg = fmt::string_view(msg.Data(), msg.Size());
    auto buf = fmt::memory_buffer();

    try {
        fmt::vformat_to(fmt::appender(buf), fmtMsg, fmt::make_format_args(args...));
    } catch (fmt::format_error const& error) {
        String result;
        WriteFormatError(result, msg, error);
        return result;
    }

    return { buf.data(), buf.size() };
}

export template <typename T>
inline constexpr String ToString(T const& val, StringView fmt = "{}")
{
    return Format(fmt, val);
}

} // namespace mini

namespace fmt {

export template <mini::CharT T, mini::AllocatorT<T> AllocT>
struct formatter<mini::BasicString<T, AllocT>, T> : formatter<basic_string_view<T>, T> {
    auto format(mini::BasicString<T, AllocT> const& str, format_context& ctx) const
    {
        basic_string_view<T> sv = fmt::basic_string_view<T>(str.Data(), str.Size());
        return formatter<basic_string_view<T>>::format(sv, ctx);
    }
};

export template <mini::CharT T, mini::AllocatorT<T> AllocT>
struct fmt::is_range<mini::BasicString<T, AllocT>, T> : mini::FalseT {};

export template <mini::CharT T>
struct formatter<mini::BasicStringView<T>, T> : formatter<basic_string_view<T>, T> {
    auto format(mini::BasicStringView<T> const& str, format_context& ctx) const
    {
        basic_string_view<T> sv = fmt::basic_string_view<T>(str.Data(), str.Size());
        return formatter<basic_string_view<T>>::format(sv, ctx);
    }
};

export template <mini::CharT T>
struct fmt::is_range<mini::BasicStringView<T>, T> : mini::FalseT {};

} // namespace fmt