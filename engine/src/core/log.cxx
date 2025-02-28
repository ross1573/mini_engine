module;

#include "core/define.h"

export module mini.log;

import mini.core;
import mini.container;

void LogMessage(char const* msg, bool endl = true);

export namespace mini
{

export template <typename... Args>
void Print(StringView const msg = "", Args&&... args)
{
    LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), false);
}

template <typename... Args>
void PrintLine(StringView const msg = "", Args&&... args)
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