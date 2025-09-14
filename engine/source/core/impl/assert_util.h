#pragma once

#include <source_location>

namespace mini::detail {

int ConcatStrings(char* dest, int destLen, char const** src, int srcCount);
int IntegerToASCII(char* dest, int destLen, int src);
int SourceLocationToString(char* dest, int destLen, std::source_location const& loc);

} // namespace mini::detail