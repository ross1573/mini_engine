export module mini.core:log;

import :type;
import :string;
import :string_view;
import :format;

CORE_API void PlatformLog(char const* msg);

namespace mini::log {

export template <typename... Args>
void Message(StringView cat, StringView msg, Args&&... args)
{
    String str;
    SizeT catSize = cat.Size();

    if (catSize != 0) {
        str.Reserve(3 + catSize);
        str.Push('[');
        str.Append(cat);
        str.Append("] ", 2);
    }

    if constexpr (sizeof...(args) == 0) {
        str.Append(msg);
    }
    else {
        FormatTo(str, msg, ForwardArg<Args>(args)...);
    }

    str.Push('\n');
    PlatformLog(str.Data());
}

export template <typename... Args>
void Info(StringView msg, Args&&... args)
{
    Message("Info", msg, ForwardArg<Args>(args)...);
}

export template <typename... Args>
void Warning(StringView msg, Args&&... args)
{
    Message("Warning", msg, ForwardArg<Args>(args)...);
}

export template <typename... Args>
void Error(StringView msg, Args&&... args)
{
    Message("Error", msg, ForwardArg<Args>(args)...);
}

} // namespace mini::log