export module mini.core:format;

import :type;
import :allocator;
import :string;
import :string_view;
import fmt;

namespace mini {

inline constexpr auto WriteFormatError(String& to, StringView const& msg,
                                       fmt::format_error const& error)
{
    StringView errorMsg = error.what();
    StringView pre = errorMsg.IsEmpty() ? "format failed" : "format failed with error: ";
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
    }
    catch (fmt::format_error const& error) {
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
    }
    catch (fmt::format_error const& error) {
        String result;
        WriteFormatError(result, msg, error);
        return result;
    }

    return { buf.data(), buf.size() };
}

export template <CharT T, AllocatorT<T> AllocT>
inline constexpr auto format_as(BasicString<T, AllocT> const& str)
{
    return fmt::basic_string_view<T>(str.Data(), str.Size());
}

export template <CharT T>
inline constexpr auto format_as(BasicStringView<T> const& str)
{
    return fmt::basic_string_view<T>(str.Data(), str.Size());
}

} // namespace mini