/*
 * Copyright 2001-2004 Unicode, Inc.
 *
 * Disclaimer
 *
 * This source code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 *
 * Limitations on Rights to Redistribute This Code
 *
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.
 */

/* ---------------------------------------------------------------------
    Conversions between UTF32, UTF-16, and UTF-8. Source code file.
    Author: Mark E. Davis, 1994.
    Rev History: Rick McGowan, fixes & updates May 2001.
    Sept 2001: fixed const & error conditions per
        mods suggested by S. Parent & A. Lillich.
    June 2002: Tim Dodd added detection and handling of incomplete
        source sequences, enhanced error detection, added casts
        to eliminate compiler warnings.
    July 2003: slight mods to back out aggressive FFFE detection.
    Jan 2004: updated switches in from-UTF8 conversions.
    Oct 2004: updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions.
------------------------------------------------------------------------ */

/* --------------------------------------------------------------------------- *
 * this file requires c++20 or over
 *
 * modified for C++ only usage (no longer supports C)
 * functions are added to support various C++ character types
 * platforms where wchar_t is not UTF-16 or UTF-32, are not supported
 *  - char
 *  - wchar_t (reinterpreted as platform specific type, char16_t or char32_t)
 *  - char8_t (reinterpreted as char)
 *  - char16_t
 *  - char32_t
 * --------------------------------------------------------------------------- */

module;

#include <cstddef>
#include "convert_utf_api.h"

export module convert_utf;

export using size_t = size_t;

export enum class ConversionResult
{
    conversionOK,    /* conversion successful */
    sourceExhausted, /* partial character in source, but hit end */
    targetExhausted, /* insuff. room in target for conversion */
    sourceIllegal    /* source sequence is illegal/malformed */
};

export enum class ConversionFlags
{
    strictConversion = 0,
    lenientConversion
};

ConversionResult Copy(const void* sourceStart, const void* sourceEnd, void* targetStart, void* targetEnd);

export
{

CVT_UTF_API size_t ConvertLength(const char* sourceStart, const char* sourceEnd, char16_t tag);
CVT_UTF_API size_t ConvertLength(const char* sourceStart, const char* sourceEnd, char32_t tag);

CVT_UTF_API size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, char tag);
CVT_UTF_API size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, char32_t tag);

CVT_UTF_API size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, char tag);
CVT_UTF_API size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, char16_t tag);

CVT_UTF_API ConversionResult Convert(const char* sourceStart, const char* sourceEnd,
                                     char16_t* targetStart, char16_t* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                                     char* targetStart, char* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API ConversionResult Convert(const char* sourceStart, const char* sourceEnd,
                                     char32_t* targetStart, char32_t* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                                     char* targetStart, char* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                                     char32_t* targetStart, char32_t* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                                     char16_t* targetStart, char16_t* targetEnd,
                                     ConversionFlags flags = ConversionFlags::lenientConversion);

CVT_UTF_API bool isLegalUTF8Sequence(const char* source, const char* sourceEnd);

CVT_UTF_API inline size_t ConvertLength(const char8_t* sourceStart, const char8_t* sourceEnd, char16_t tag) {
    return ConvertLength(reinterpret_cast<const char*>(sourceStart),
                         reinterpret_cast<const char*>(sourceEnd), tag);
}

CVT_UTF_API inline size_t ConvertLength(const char8_t* sourceStart, const char8_t* sourceEnd, char32_t tag) {
    return ConvertLength(reinterpret_cast<const char*>(sourceStart), 
                         reinterpret_cast<const char*>(sourceEnd), tag);
}

CVT_UTF_API inline size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, char8_t) {
    return ConvertLength(sourceStart, sourceEnd, (char)0);
}

CVT_UTF_API inline size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, char8_t) {
    return ConvertLength(sourceStart, sourceEnd, (char)0);
}

CVT_UTF_API inline size_t ConvertLength(const wchar_t* sourceStart, const wchar_t* sourceEnd, char) {
    if constexpr (sizeof(wchar_t) == 2) {
        return ConvertLength(reinterpret_cast<const char16_t*>(sourceStart),
                             reinterpret_cast<const char16_t*>(sourceEnd), (char)0);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return ConvertLength(reinterpret_cast<const char32_t*>(sourceStart),
                             reinterpret_cast<const char32_t*>(sourceEnd), (char)0);
    }
    return 0;
}

CVT_UTF_API inline size_t ConvertLength(const wchar_t* sourceStart, const wchar_t* sourceEnd, char8_t) {
    return ConvertLength(sourceStart, sourceEnd, (char)0);
}

CVT_UTF_API inline size_t ConvertLength(const wchar_t* sourceStart, const wchar_t* sourceEnd, char16_t) {
    if constexpr (sizeof(wchar_t) == 2) {
        return static_cast<size_t>(sourceEnd - sourceStart);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return ConvertLength(reinterpret_cast<const char32_t*>(sourceStart),
                             reinterpret_cast<const char32_t*>(sourceEnd), (char16_t)0);
    }
    return 0;
}

CVT_UTF_API inline size_t ConvertLength(const wchar_t* sourceStart, const wchar_t* sourceEnd, char32_t) {
    if constexpr (sizeof(wchar_t) == 2) {
        return ConvertLength(reinterpret_cast<const char16_t*>(sourceStart),
                             reinterpret_cast<const char16_t*>(sourceEnd), (char)0);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return static_cast<size_t>(sourceEnd - sourceStart);
    }
    return 0;
}

CVT_UTF_API inline size_t ConvertLength(const char* sourceStart, const char* sourceEnd, wchar_t) {
    if constexpr (sizeof(wchar_t) == 2) {
        return ConvertLength(sourceStart, sourceEnd, (char16_t)0);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return ConvertLength(sourceStart, sourceEnd, (char32_t)0);
    }
    return 0;
}

CVT_UTF_API inline size_t ConvertLength(const char8_t* sourceStart, const char8_t* sourceEnd, wchar_t tag) {
    return ConvertLength(reinterpret_cast<const char*>(sourceStart),
                         reinterpret_cast<const char*>(sourceEnd), tag);
}

CVT_UTF_API inline size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, wchar_t) {
    if constexpr (sizeof(wchar_t) == 2) {
        return static_cast<size_t>(sourceEnd - sourceStart);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return ConvertLength(sourceStart, sourceEnd, (char32_t)0);
    }
    return 0;
}

CVT_UTF_API inline size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, wchar_t) {
    if constexpr (sizeof(wchar_t) == 2) {
        return ConvertLength(sourceStart, sourceEnd, (char16_t)0);
    } else if constexpr (sizeof(wchar_t) == 4){
        return static_cast<size_t>(sourceEnd - sourceStart);
    }
    return 0;
}


CVT_UTF_API inline ConversionResult Convert(const char8_t* sourceStart, const char8_t* sourceEnd,
                                            char16_t* targetStart, char16_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(reinterpret_cast<const char*>(sourceStart), 
                    reinterpret_cast<const char*>(sourceEnd),
                   targetStart, targetEnd, flags);
}

CVT_UTF_API inline ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                                            char8_t* targetStart, char8_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(sourceStart, sourceEnd,
                   reinterpret_cast<char*>(targetStart), 
                   reinterpret_cast<char*>(targetEnd), flags);
}

CVT_UTF_API inline ConversionResult Convert(const char8_t* sourceStart, const char8_t* sourceEnd,
                                            char32_t* targetStart, char32_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(reinterpret_cast<const char*>(sourceStart), 
                   reinterpret_cast<const char*>(sourceEnd),
                   targetStart, targetEnd, flags);
}

CVT_UTF_API inline ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                                            char8_t* targetStart, char8_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(sourceStart, sourceEnd,
                   reinterpret_cast<char*>(targetStart), 
                   reinterpret_cast<char*>(targetEnd), flags);
}

CVT_UTF_API inline ConversionResult Convert(const wchar_t* sourceStart, const wchar_t* sourceEnd,
                                            char* targetStart, char* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Convert(reinterpret_cast<const char16_t*>(sourceStart), 
                       reinterpret_cast<const char16_t*>(sourceEnd),
                       targetStart, targetEnd, flags);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return Convert(reinterpret_cast<const char32_t*>(sourceStart),
                       reinterpret_cast<const char32_t*>(sourceEnd),
                       targetStart, targetEnd, flags);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline ConversionResult Convert(const wchar_t* sourceStart, const wchar_t* sourceEnd,
                                            char8_t* targetStart, char8_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(sourceStart, sourceEnd,
                   reinterpret_cast<char*>(targetStart),
                   reinterpret_cast<char*>(targetEnd), flags);
}

CVT_UTF_API inline ConversionResult Convert(const wchar_t* sourceStart, const wchar_t* sourceEnd,
                                            char16_t* targetStart, char16_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Copy((void*)sourceStart, (void*)sourceEnd, (void*)targetStart, (void*)targetEnd);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return Convert(reinterpret_cast<const char32_t*>(sourceStart),
                       reinterpret_cast<const char32_t*>(sourceEnd),
                       targetStart, targetEnd, flags);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline ConversionResult Convert(const wchar_t* sourceStart, const wchar_t* sourceEnd,
                                            char32_t* targetStart, char32_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Convert(reinterpret_cast<const char16_t*>(sourceStart),
                       reinterpret_cast<const char16_t*>(sourceEnd),
                       targetStart, targetEnd, flags);
    }
    else if constexpr (sizeof(wchar_t) == 4) {
        return Copy((void*)sourceStart, (void*)sourceEnd, (void*)targetStart, (void*)targetEnd);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline ConversionResult Convert(const char* sourceStart, const char* sourceEnd,
                                            wchar_t* targetStart, wchar_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Convert(sourceStart, sourceEnd,
                       reinterpret_cast<char16_t*>(targetStart),
                       reinterpret_cast<char16_t*>(targetEnd), flags);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return Convert(sourceStart, sourceEnd,
                       reinterpret_cast<char32_t*>(targetStart),
                       reinterpret_cast<char32_t*>(targetEnd), flags);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline ConversionResult Convert(const char8_t* sourceStart, const char8_t* sourceEnd,
                                            wchar_t* targetStart, wchar_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    return Convert(reinterpret_cast<const char*>(sourceStart),
                   reinterpret_cast<const char*>(sourceEnd),
                   targetStart, targetEnd, flags);
}

CVT_UTF_API inline ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                                            wchar_t* targetStart, wchar_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Copy((void*)sourceStart, (void*)sourceEnd, (void*)targetStart, (void*)targetEnd);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return Convert(sourceStart, sourceEnd,
                       reinterpret_cast<char32_t*>(targetStart),
                       reinterpret_cast<char32_t*>(targetEnd), flags);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                                            wchar_t* targetStart, wchar_t* targetEnd,
                                            ConversionFlags flags = ConversionFlags::lenientConversion) {
    if constexpr (sizeof(wchar_t) == 2) {
        return Convert(sourceStart, sourceEnd,
                       reinterpret_cast<char16_t*>(targetStart),
                       reinterpret_cast<char16_t*>(targetEnd), flags);
    } else if constexpr (sizeof(wchar_t) == 4) {
        return Copy((void*)sourceStart, (void*)sourceEnd, (void*)targetStart, (void*)targetEnd);
    }
    return ConversionResult::sourceIllegal;
}

CVT_UTF_API inline bool isLegalUTF8Sequence(const char8_t* source, const char8_t* sourceEnd) {
    return isLegalUTF8Sequence(reinterpret_cast<const char*>(source), 
                               reinterpret_cast<const char*>(sourceEnd));
}

}

/* --------------------------------------------------------------------- */
