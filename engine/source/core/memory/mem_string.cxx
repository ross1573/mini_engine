export module mini.core:mem_string;

import :type;

export namespace mini::memory
{

#ifdef _MSC_VER

CORE_API SizeT ConvertLength(char const* src, SizeT srcLen);
CORE_API SizeT ConvertLength(wchar_t const* src, SizeT srcLen);
CORE_API void Convert(char const* src, SizeT srcLen, wchar_t* dst, SizeT dstLen);
CORE_API void Convert(wchar_t const* src, SizeT srcLen, char* dst, SizeT dstLen);

#endif // _MSC_VER

CORE_API SizeT ConcatStrings(char* dest, SizeT destLen, char const** src, SizeT srcCount);

CORE_API SizeT IntegerToASCII(char* dest, SizeT destLen, SizeT src);
CORE_API SizeT SourceLocationToString(char* dest, SizeT destLen, std::source_location const& loc);

} // namespace mini::memory