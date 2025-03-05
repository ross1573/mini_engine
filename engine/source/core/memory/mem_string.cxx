export module mini.core:mem_string;

import :type;

export namespace mini::memory
{

CORE_API SizeT ConcatStrings(char* dest, SizeT destLen, char const** src, SizeT srcCount);

CORE_API SizeT IntegerToASCII(char* dest, SizeT destLen, SizeT src);

CORE_API SizeT SourceLocationToString(char* dest, SizeT destLen, std::source_location const& loc);

} // namespace mini::memory