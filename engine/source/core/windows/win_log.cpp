module;

#include <windows.h>

module mini.core;

void PlatformLog(char const* msg)
{
    OutputDebugStringA(msg);
}