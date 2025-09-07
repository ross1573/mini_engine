export module mini.core:log;

import :type;
import :string;
import :string_view;
import :format;

export CORE_API void LogMessage(char const* msg);

namespace mini::log {

export template <typename... Args>
void Info(StringView msg, Args&&... args)
{
    String str(static_cast<SizeT>(8 + msg.Size()));
    str.Append("[Info] ");
    str.Append(Format(msg, ForwardArg<Args>(args)...).c_str());
    str.Push('\n');
    LogMessage(str.Data());
}

export template <typename... Args>
void Warning(StringView msg, Args&&... args)
{
    String str(static_cast<SizeT>(11 + msg.Size()));
    str.Append("[Warning] ");
    str.Append(Format(msg, ForwardArg<Args>(args)...).c_str());
    str.Push('\n');
    LogMessage(str.Data());
}

export template <typename... Args>
void Error(StringView msg, Args&&... args)
{
    String str(static_cast<SizeT>(9 + msg.Size()));
    str.Append("[Error] ");
    str.Append(Format(msg, ForwardArg<Args>(args)...).c_str());
    str.Push('\n');
    LogMessage(str.Data());
}

} // namespace mini::log