NOTE: Code downloaded from http://www.unicode.org
The accompanying C source code file "ConvertUTF.c" and the associated header
file "ConvertUTF.h" provide for conversion between various transformation
formats of Unicode characters.  The following conversions are supported:
	UTF-32 to UTF-16
	UTF-32 to UTF-8
	UTF-16 to UTF-32
	UTF-16 to UTF-8
	UTF-8 to UTF-16
	UTF-8 to UTF-32
In addition, there is a test harness which runs various tests.
The files "CVTUTF7.C" and "CVTUTF7.H" are for archival and historical purposes
only. They have not been updated to Unicode 3.0 or later and should be
considered obsolescent. "CVTUTF7.C" contains two functions that can convert
between UCS2 (i.e., the BMP characters only) and UTF-7. Surrogates are
not supported, the code has not been tested, and should be considered
unsuitable for general purpose use.
Please submit any bug reports about these programs here:
	http://www.unicode.org/unicode/reporting.html
Version 1.0: initial version.
Version 1.1: corrected some minor problems; added stricter checks.
Version 1.2: corrected switch statements associated with "extraBytesToRead"
	in 4 & 5 byte cases, in functions for conversion from UTF8.
	Note: formally, the 4 & 5 byte cases are illegal in the latest
	UTF8, but the table and this code has always catered for those,
	cases since at one time they were legal.
Version 1.3: Updated UTF-8 legality check;
	updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions
	Updated UTF-8 legality tests in harness.c
 
Last update: October 19, 2004

--------------------------------------------------------------------------------
This version of ConvertUTF has been modified for cpp only, and requires c++20.
It adoptes c++20 modules, and provides only single file (convert_utf.cxx)
Most functions are converted into templated version, and all conversions support
constexpr. The following cpp types and conversions are supported.
	(wchar_t is supported only when it is 2 bytes or 4 bytes long)
	char	 -> char, wchar_t, char8_t, char16_t, char32_t
	wchar_t  -> char, wchar_t, char8_t, char16_t, char32_t
	char8_t  -> char, wchar_t, char8_t, char16_t, char32_t
	char16_t -> char, wchar_t, char8_t, char16_t, char32_t
	char32_t -> char, wchar_t, char8_t, char16_t, char32_t

Updated: May 04, 2025