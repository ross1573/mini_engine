module;

#include <windows.h>

export module mini.core:log;

import :type;
import :string;

//void LogMessage(char const* msg, bool endl = true);

CORE_API void LogMessage(char const* msg, bool endLine)
{
    OutputDebugStringA(msg);

    if (endLine)
    {
        OutputDebugStringA("\n");
    }
}

export namespace mini
{

template <typename... Args>
inline void Print(StringView const msg = "", Args&&... args)
{
    LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), false);
}

template <typename... Args>
inline void PrintLine(StringView const msg = "", Args&&... args)
{
    LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
}

struct Log
{
    template <typename... Args>
    static void Info(StringView msg, Args&& ...args)
    {
        LogMessage("[ Info ] ", false);
        LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }

    template <typename... Args>
    static void Warning(StringView msg, Args&& ...args)
    {
        LogMessage("[ Warning ] ", false);
        LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }

    template <typename... Args>
    static void Error(StringView msg, Args&& ...args)
    {
        LogMessage("[ Error ] ", false);
        LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }
};

} // namespace mini