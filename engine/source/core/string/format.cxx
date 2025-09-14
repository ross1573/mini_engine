export module mini.core:format;

import :type;
import :allocator;
import :string;
import :string_view;
import fmt;

namespace mini {

template <CharT T, AllocatorT<T> AllocT>
class StringAppender {
private:
    BasicString<T, AllocT>& m_Str;

public:
    constexpr StringAppender(BasicString<T, AllocT>& buf) noexcept
        : m_Str(buf)
    {
    }

    constexpr StringAppender& operator=(T c)
    {
        m_Str.Push(c);
        return *this;
    }

    constexpr StringAppender& operator*() noexcept { return *this; }
    constexpr StringAppender& operator++() noexcept { return *this; }
    constexpr StringAppender operator++(int) const noexcept { return *this; }
};

export template <typename... Args>
inline constexpr void FormatTo(String& to, StringView msg, Args&&... args)
{
    fmt::vformat_to(StringAppender(to), msg.Data(), fmt::make_format_args(args...));
}

export template <typename... Args>
inline constexpr String Format(StringView msg, Args&&... args)
{
    constexpr SizeT msgDefaultSize = 1 << 6;
    SizeT len = (msg.Size() + msgDefaultSize - 1) &
                ~static_cast<SizeT>(msgDefaultSize - 1); // round up to default size

    String string(len);
    FormatTo(string, msg, ForwardArg<Args>(args)...);
    return string;
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