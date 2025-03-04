module;

#include <iostream>

export module mini.core;

void LogMessage(char const* msg)
{
    std::cout << msg;
}