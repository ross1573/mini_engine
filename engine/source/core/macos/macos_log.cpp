module;

#include <stdio.h>

module mini.core;

void LogMessage(char const* msg)
{
    fputs(msg, stdout);
}