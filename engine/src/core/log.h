#pragma once

#include "container/string.h"

namespace mini::detail
{

void LogMessage(char const* msg, bool endl = true);

} // namespace mini::detail

namespace mini
{

template <typename... Args>
void Print(StringView const msg = "", Args&&... args)
{
    detail::LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), false);
}

template <typename... Args>
void PrintLine(StringView const msg = "", Args&&... args)
{
    detail::LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
}

struct Log
{
    template <typename... Args>
    static void Info(StringView msg, Args&& ...args)
    {
        detail::LogMessage("[ Info ] ", false);
        detail::LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }

    template <typename... Args>
    static void Warning(StringView msg, Args&& ...args)
    {
        detail::LogMessage("[ Warning ] ", false);
        detail::LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }

    template <typename... Args>
    static void Error(StringView msg, Args&& ...args)
    {
        detail::LogMessage("[ Error ] ", false);
        detail::LogMessage(Format(msg, ForwardArg<Args>(args)...).data(), true);
    }
};

} // namespace mini
