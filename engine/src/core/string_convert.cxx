module;

#include <source_location>
#include "core/define.h"

export module mini.core:string;

import :type;

export namespace mini::memory
{

#ifdef MSVC

SizeT ConvertLength(char const* src, SizeT srcLen);
SizeT ConvertLength(wchar_t const* src, SizeT srcLen);
void Convert(char const* src, SizeT srcLen, wchar_t* dst, SizeT dstLen);
void Convert(wchar_t const* src, SizeT srcLen, char* dst, SizeT dstLen);

#endif // MSVC

SizeT ConcatStrings(char* dest, SizeT destLen, char const** src, SizeT srcCount);

SizeT IntegerToASCII(char* dest, SizeT destLen, SizeT src);
SizeT SourceLocationToString(char* dest, SizeT destLen, std::source_location const& loc);

} // namespace mini::memory