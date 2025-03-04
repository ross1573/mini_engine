module;

#include <windows.h>

module mini.core;

void LogMessage(char const* msg)
{
    OutputDebugStringA(msg);
}