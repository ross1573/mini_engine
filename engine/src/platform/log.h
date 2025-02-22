#pragma once

#include "container/string.h"

namespace mini
{

void Print(StringView const msg);
void PrintLine(StringView const msg = "");

struct Log
{
    template <typename... Args>
    static void Info(StringView msg, Args&& ...args)
    {
        Print("[ Info ] ");
        PrintLine(Format(msg, args...));
    }

    template <typename... Args>
    static void Warning(StringView msg, Args&& ...args)
    {
        Print("[ Warning ] ");
        PrintLine(Format(msg, args...));
    }

    template <typename... Args>
    static void Error(StringView msg, Args&& ...args)
    {
        Print("[ Error ] ");
        PrintLine(Format(msg, args...));
    }
};

} // namespace mini