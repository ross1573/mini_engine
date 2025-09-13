module;

#include <iterator>

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
    constexpr StringAppender(BasicString<T, AllocT>& buf)
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
String Format(StringView msg, Args&&... args)
{
    constexpr SizeT msgDefaultSize = 1 << 6;

    SizeT len = (msg.Size() + msgDefaultSize) & ~SizeT(msgDefaultSize); // round up to default size
    String string(len);

    fmt::vformat_to(StringAppender(string), msg.Data(), fmt::make_format_args(args...));
    return string;
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