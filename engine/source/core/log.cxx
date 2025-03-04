module;

#include <source_location>
#include <format>

export module mini.core:log;

import :type;
import :string;

export CORE_API void LogMessage(char const* msg);

export namespace mini::log
{

template <typename... Args>
void Info(StringView msg, Args&& ...args)
{
    LogMessage(std::format("[ Info ] {}\n", std::vformat(msg, std::make_format_args(args...))).c_str());
}

template <typename... Args>
void Warning(StringView msg, Args&& ...args)
{
    LogMessage(std::format("[ Warning ] {}\n", std::vformat(msg, std::make_format_args(args...))).c_str());
}

template <typename... Args>
void Error(StringView msg, Args&& ...args)
{
    LogMessage(std::format("[ Error ] {}\n", std::vformat(msg, std::make_format_args(args...))).c_str());
}

} // namespace mini::log