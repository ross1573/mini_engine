export module mini.core:format;

export import fmt;

import :type;
import :memory_operation;
import :string;
import :string_view;

namespace mini {

constexpr String writeFormatError(String& to, StringView msg, fmt::format_error const& error)
{
    StringView errorMsg = error.what();
    StringView pre = errorMsg.empty() ? "format failed" : "format failed with error: ";
    size_t totalLen = to.size() + msg.size() + pre.size() + errorMsg.size() + 3;

    to.reserve(totalLen);
    to.append(msg);
    to.append(" (", 2);
    to.append(pre);
    to.append(errorMsg);
    to.pushBack(')');
    return to;
}

export template <typename... Args>
constexpr void formatTo(String& to, StringView msg, Args&&... args)
{
    auto fmtMsg = fmt::string_view(msg.data(), msg.size());
    auto buf = fmt::memory_buffer();

    try {
        fmt::vformat_to(fmt::appender(buf), fmtMsg, fmt::make_format_args(args...));
    } catch (fmt::format_error const& error) {
        writeFormatError(to, msg, error);
        return;
    }

    to.append(buf.data(), buf.size());
}

export template <typename... Args>
constexpr String format(StringView msg, Args&&... args)
{
    auto fmtMsg = fmt::string_view(msg.data(), msg.size());
    auto buf = fmt::memory_buffer();

    try {
        fmt::vformat_to(fmt::appender(buf), fmtMsg, fmt::make_format_args(args...));
    } catch (fmt::format_error const& error) {
        String result;
        writeFormatError(result, msg, error);
        return result;
    }

    return {buf.data(), buf.size()};
}

export template <typename T>
constexpr String toString(T const& val, StringView fmt = "{}")
{
    return format(fmt, val);
}

} // namespace mini

export namespace fmt {

template <mini::CharT T, mini::AllocatorT<T> AllocT>
struct formatter<mini::BasicString<T, AllocT>, T> : formatter<basic_string_view<T>, T> {
    auto format(mini::BasicString<T, AllocT> const& str, format_context& ctx) const
    {
        basic_string_view<T> sv = fmt::basic_string_view<T>(str.data(), str.size());
        return formatter<basic_string_view<T>>::format(sv, ctx);
    }
};

template <mini::CharT T, mini::AllocatorT<T> AllocT>
struct fmt::is_range<mini::BasicString<T, AllocT>, T> : mini::FalseT { };

template <mini::CharT T>
struct formatter<mini::BasicStringView<T>, T> : formatter<basic_string_view<T>, T> {
    auto format(mini::BasicStringView<T> const& str, format_context& ctx) const
    {
        basic_string_view<T> sv = fmt::basic_string_view<T>(str.data(), str.size());
        return formatter<basic_string_view<T>>::format(sv, ctx);
    }
};

template <mini::CharT T>
struct fmt::is_range<mini::BasicStringView<T>, T> : mini::FalseT { };

} // namespace fmt