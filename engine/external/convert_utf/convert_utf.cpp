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

#include <cstring>

module convert_utf;

constexpr int halfShift     = 10; /* used for shifting by 10 bits */
constexpr char32_t halfBase = 0x0010000UL;
constexpr char32_t halfMask = 0x3FFUL;

/* Some fundamental constants */
constexpr char32_t UNI_REPLACEMENT_CHAR = (char32_t)0x0000FFFD;
constexpr char32_t UNI_MAX_BMP          = (char32_t)0x0000FFFF;
constexpr char32_t UNI_MAX_UTF16        = (char32_t)0x0010FFFF;
constexpr char32_t UNI_MAX_UTF32        = (char32_t)0x7FFFFFFF;
constexpr char32_t UNI_MAX_LEGAL_UTF32  = (char32_t)0x0010FFFF;

constexpr char32_t UNI_SUR_HIGH_START = (char32_t)0xD800;
constexpr char32_t UNI_SUR_HIGH_END   = (char32_t)0xDBFF;
constexpr char32_t UNI_SUR_LOW_START  = (char32_t)0xDC00;
constexpr char32_t UNI_SUR_LOW_END    = (char32_t)0xDFFF;

/* --------------------------------------------------------------------- */

size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, char16_t) {
    size_t size            = 0;
    const char32_t* source = sourceStart;

    while (source < sourceEnd) {
        char32_t ch = *source++;

        if (ch <= UNI_MAX_BMP || ch > UNI_MAX_LEGAL_UTF32) {
            ++size;
        } else {
            size += 2;
        }
    }

    return size;
}

ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                         char16_t* targetStart, char16_t* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char32_t* source  = sourceStart;
    char16_t* target        = targetStart;

    while (source < sourceEnd) {
        char32_t ch = *source++;

        if (target >= targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == ConversionFlags::strictConversion) {
                    result = ConversionResult::sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = (char16_t)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_LEGAL_UTF32) {
            if (flags == ConversionFlags::strictConversion) {
                result = ConversionResult::sourceIllegal;
            } else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
             /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                result = ConversionResult::targetExhausted;
                break;
            }

            ch -= halfBase;
            *target++ = (char16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *target++ = (char16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }

    return result;
}

/* --------------------------------------------------------------------- */

size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, char32_t) {
    size_t size            = 0;
    const char16_t* source = sourceStart;
    char32_t ch, ch2;

    while (source < sourceEnd) {
        ch = *source++;

        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ++source;
                }
            } else { /* source exhausted */
                break;
            }
        }
        
        ++size;
    }
    
    return size;
}

ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                         char32_t* targetStart, char32_t* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char16_t* source  = sourceStart;
    char32_t* target        = targetStart;
    char32_t ch, ch2;

    while (source < sourceEnd) {
        ch = *source++;

        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++source;
                } else if (flags == ConversionFlags::strictConversion) { /* it's an unpaired high surrogate */
                    result = ConversionResult::sourceIllegal;
                    break;
                }
            } else {      /* We don't have the 16 bits following the high surrogate. */
                result = ConversionResult::sourceExhausted;
                break;
            }
        } else if (flags == ConversionFlags::strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                result = ConversionResult::sourceIllegal;
                break;
            }
        }

        if (target >= targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        *target++ = ch;
    }

    return result;
}

/* --------------------------------------------------------------------- */

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
constexpr char8_t trailingBytesForUTF8[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
constexpr char32_t offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL};

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
constexpr char8_t firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

/* --------------------------------------------------------------------- */
/* The interface converts a whole buffer to avoid function-call overhead.
 * Constants have been gathered. Loops & conditionals have been removed as
 * much as possible for efficiency, in favor of drop-through switches.
 * (See "Note A" at the bottom of the file for equivalent code.)
 * If your compiler supports it, the "isLegalUTF8" call can be turned
 * into an inline function.
 */
/* --------------------------------------------------------------------- */

size_t ConvertLength(const char16_t* sourceStart, const char16_t* sourceEnd, char) {
    size_t size            = 0;
    const char16_t* source = sourceStart;

    while (source < sourceEnd) {
        char32_t ch                 = *source++;
        unsigned short bytesToWrite = 0;

        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                char32_t ch2 = *source;
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ++source;
                }
            } else { /* source exhausted */
                break;
            }
        }

        // TPN: substitute all control characters except for NULL, TAB, LF or CR
        if (ch && (ch != (char32_t)0x09) && (ch != (char32_t)0x0a) && (ch != (char32_t)0x0d) && (ch < (char32_t)0x20)) {
            ch = (char32_t)0x3f;
        }

        // TPN: filter out byte order marks and invalid character 0xFFFF
        if ((ch == (char32_t)0xFEFF) || (ch == (char32_t)0xFFFE) || (ch == (char32_t)0xFFFF)) {
            continue;
        }

        /* Figure out how many bytes the result will require */
        if (ch < (char32_t)0x80) {
            bytesToWrite = 1;
        } else if (ch < (char32_t)0x800) {
            bytesToWrite = 2;
        } else if (ch < (char32_t)0x10000) {
            bytesToWrite = 3;
        } else if (ch < (char32_t)0x110000) {
            bytesToWrite = 4;
        } else {
            bytesToWrite = 3;
        }

        size += bytesToWrite;
    }

    return size;
}

ConversionResult Convert(const char16_t* sourceStart, const char16_t* sourceEnd,
                         char* targetStart, char* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char16_t* source  = sourceStart;
    char* target            = targetStart;

    while (source < sourceEnd) {
        char32_t ch                 = *source++;
        unsigned short bytesToWrite = 0;
        const char32_t byteMask     = 0xBF;
        const char32_t byteMark     = 0x80;

        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                char32_t ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++source;
                } else if (flags == ConversionFlags::strictConversion) { /* it's an unpaired high surrogate */
                    result = ConversionResult::sourceIllegal;
                    break;
                }
            } else {      /* We don't have the 16 bits following the high surrogate. */
                result = ConversionResult::sourceExhausted;
                break;
            }
        } else if (flags == ConversionFlags::strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                result = ConversionResult::sourceIllegal;
                break;
            }
        }

        // TPN: substitute all control characters except for NULL, TAB, LF or CR
        if (ch && (ch != (char32_t)0x09) && (ch != (char32_t)0x0a) && (ch != (char32_t)0x0d) && (ch < (char32_t)0x20)) {
            ch = (char32_t)0x3f;
        }

        // TPN: filter out byte order marks and invalid character 0xFFFF
        if ((ch == (char32_t)0xFEFF) || (ch == (char32_t)0xFFFE) || (ch == (char32_t)0xFFFF)) {
            continue;
        }

        /* Figure out how many bytes the result will require */
        if (ch < (char32_t)0x80) {
            bytesToWrite = 1;
        } else if (ch < (char32_t)0x800) {
            bytesToWrite = 2;
        } else if (ch < (char32_t)0x10000) {
            bytesToWrite = 3;
        } else if (ch < (char32_t)0x110000) {
            bytesToWrite = 4;
        } else {
            bytesToWrite = 3;
            ch           = UNI_REPLACEMENT_CHAR;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--target = (char)(ch | firstByteMark[bytesToWrite]);
        }
        target += bytesToWrite;
    }

    return result;
}

/* --------------------------------------------------------------------- */

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
bool isLegalUTF8(const char* source, int length)
{
    char8_t a;
    const char* srcptr = source + length;

    switch (length) {
        default:
            return false;
            /* Everything else falls through when "true"... */
        case 4:
            if ((a = (char8_t)(*--srcptr)) < 0x80 || a > 0xBF) {
                return false;
            }
        case 3:
            if ((a = (char8_t)(*--srcptr)) < 0x80 || a > 0xBF) {
                return false;
            }
        case 2:
            if ((a = (char8_t)(*--srcptr)) > 0xBF) {
                return false;
            }
            switch (*source) { /* no fall-through in this inner switch */
                case 0xE0:
                    if (a < 0xA0) {
                        return false;
                    }
                    break;
                case 0xED:
                    if (a > 0x9F) {
                        return false;
                    }
                    break;
                case 0xF0:
                    if (a < 0x90) {
                        return false;
                    }
                    break;
                case 0xF4:
                    if (a > 0x8F) {
                        return false;
                    }
                    break;
                default:
                    if (a < 0x80) {
                        return false;
                    }
            }
        case 1:
            if ((char8_t)(*source) >= 0x80 && (char8_t)(*source) < 0xC2) {
                return false;
            }
    }

    if ((char8_t)(*source) > 0xF4) {
        return false;
    }

    return true;
}
/* --------------------------------------------------------------------- */

/*
 * Exported function to return whether a UTF-8 sequence is legal or not.
 * This is not used here; it's just exported.
 */
bool isLegalUTF8Sequence(const char* source, const char* sourceEnd) {
    int length = trailingBytesForUTF8[(char8_t)*source] + 1;

    if (source + length > sourceEnd) {
        return false;
    }

    return isLegalUTF8(source, length);
}

/* --------------------------------------------------------------------- */

size_t ConvertLength(const char* sourceStart, const char* sourceEnd, char16_t) {
    size_t size        = 0;
    const char* source = sourceStart;

    while (source < sourceEnd) {
        char32_t ch                     = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[(char8_t)*source];

        if (source + extraBytesToRead >= sourceEnd) { /* source exhausted */
            break;
        }

        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(source, extraBytesToRead + 1)) {
            break;
        }

        /* The cases all fall through. See "Note A" below. */
        switch (extraBytesToRead) {
            case 5: ch += (char8_t)*source++; ch <<= 6; /* remember, illegal UTF-8 */
            case 4: ch += (char8_t)*source++; ch <<= 6; /* remember, illegal UTF-8 */
            case 3: ch += (char8_t)*source++; ch <<= 6;
            case 2: ch += (char8_t)*source++; ch <<= 6;
            case 1: ch += (char8_t)*source++; ch <<= 6;
            case 0: ch += (char8_t)*source++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (ch <= UNI_MAX_BMP || ch >= UNI_MAX_UTF16) {
            ++size;
        } else {
            size += 2;
        }
    }

    return size;
}

ConversionResult Convert(const char* sourceStart, const char* sourceEnd,
                         char16_t* targetStart, char16_t* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char* source      = sourceStart;
    char16_t* target        = targetStart;

    while (source < sourceEnd)
    {
        char32_t ch                     = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[(char8_t)*source];

        if (source + extraBytesToRead >= sourceEnd) {
            result = ConversionResult::sourceExhausted;
            break;
        }

        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(source, extraBytesToRead + 1)) {
            result = ConversionResult::sourceIllegal;
            break;
        }

        /* The cases all fall through. See "Note A" below. */
        switch (extraBytesToRead) {
            case 5: ch += (char8_t)*source++; ch <<= 6; /* remember, illegal UTF-8 */
            case 4: ch += (char8_t)*source++; ch <<= 6; /* remember, illegal UTF-8 */
            case 3: ch += (char8_t)*source++; ch <<= 6;
            case 2: ch += (char8_t)*source++; ch <<= 6;
            case 1: ch += (char8_t)*source++; ch <<= 6;
            case 0: ch += (char8_t)*source++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        if (ch <= UNI_MAX_BMP) {
            /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == ConversionFlags::strictConversion) {
                    result = ConversionResult::sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = (char16_t)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_UTF16) {
            if (flags == ConversionFlags::strictConversion) {
                result = ConversionResult::sourceIllegal;
                break;                            /* Bail out; shouldn't continue */
            } else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
             /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                result = ConversionResult::targetExhausted;
                break;
            }

            ch -= halfBase;
            *target++ = (char16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *target++ = (char16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }

    return result;
}

/* --------------------------------------------------------------------- */

size_t ConvertLength(const char32_t* sourceStart, const char32_t* sourceEnd, char) {
    size_t size            = 0;
    const char32_t* source = sourceStart;

    while (source < sourceEnd) {
        char32_t ch                 = *source++;
        unsigned short bytesToWrite = 0;

        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
         */
        if (ch < (char32_t)0x80) {
            bytesToWrite = 1;
        } else if (ch < (char32_t)0x800) {
            bytesToWrite = 2;
        } else if (ch < (char32_t)0x10000) {
            bytesToWrite = 3;
        } else if (ch <= UNI_MAX_LEGAL_UTF32) {
            bytesToWrite = 4;
        } else {
            bytesToWrite = 3;
        }

        size += bytesToWrite;
    }

    return size;
}

ConversionResult Convert(const char32_t* sourceStart, const char32_t* sourceEnd,
                         char* targetStart, char* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char32_t* source  = sourceStart;
    char* target            = targetStart;

    while (source < sourceEnd) {
        char32_t ch                 = *source++;
        unsigned short bytesToWrite = 0;
        const char32_t byteMask     = 0xBF;
        const char32_t byteMark     = 0x80;

        if (flags == ConversionFlags::strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                result = ConversionResult::sourceIllegal;
                break;
            }
        }

        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
         */
        if (ch < (char32_t)0x80) {
            bytesToWrite = 1;
        } else if (ch < (char32_t)0x800) {
            bytesToWrite = 2;
        } else if (ch < (char32_t)0x10000) {
            bytesToWrite = 3;
        } else if (ch <= UNI_MAX_LEGAL_UTF32) {
            bytesToWrite = 4;
        } else {
            bytesToWrite = 3;
            ch           = UNI_REPLACEMENT_CHAR;
            result       = ConversionResult::sourceIllegal;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--target = (char)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--target = (char)(ch | firstByteMark[bytesToWrite]);
        }

        target += bytesToWrite;
    }

    return result;
}

/* --------------------------------------------------------------------- */

size_t ConvertLength(const char* sourceStart, const char* sourceEnd, char32_t) {
    size_t size        = 0;
    const char* source = sourceStart;

    while (source < sourceEnd) {
        unsigned short extraBytesToRead = trailingBytesForUTF8[(char8_t)*source];

        if (source + extraBytesToRead >= sourceEnd) { /* source exhausted */
            break;
        }

        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(source, extraBytesToRead + 1)) {
            break;
        }

        source += extraBytesToRead + 1;
        ++size;
    }

    return size;
}

ConversionResult Convert(const char* sourceStart, const char* sourceEnd,
                         char32_t* targetStart, char32_t* targetEnd,
                         ConversionFlags flags) {
    ConversionResult result = ConversionResult::conversionOK;
    const char* source      = sourceStart;
    char32_t* target        = targetStart;

    while (source < sourceEnd) {
        char32_t ch                     = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[(char8_t)*source];

        if (source + extraBytesToRead >= sourceEnd) {
            result = ConversionResult::sourceExhausted;
            break;
        }

        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(source, extraBytesToRead + 1)) {
            result = ConversionResult::sourceIllegal;
            break;
        }

        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
            case 5: ch += (char8_t)*source++; ch <<= 6;
            case 4: ch += (char8_t)*source++; ch <<= 6;
            case 3: ch += (char8_t)*source++; ch <<= 6;
            case 2: ch += (char8_t)*source++; ch <<= 6;
            case 1: ch += (char8_t)*source++; ch <<= 6;
            case 0: ch += (char8_t)*source++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            result = ConversionResult::targetExhausted;
            break;
        }

        if (ch <= UNI_MAX_LEGAL_UTF32) {
            /*
             * UTF-16 surrogate values are illegal in UTF-32, and anything
             * over Plane 17 (> 0x10FFFF) is illegal.
             */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == ConversionFlags::strictConversion) {
                    result = ConversionResult::sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = ch;
            }
        } else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
            result    = ConversionResult::sourceIllegal;
            *target++ = UNI_REPLACEMENT_CHAR;
        }
    }

    return result;
}

/* --------------------------------------------------------------------- */

ConversionResult Copy(const void* sourceStart, const void* sourceEnd, void* targetStart, void* targetEnd) {
    size_t sourceLen        = (char*)sourceEnd - (char*)sourceStart;
    size_t targetLen        = (char*)targetEnd - (char*)targetStart;
    size_t copyLen          = sourceLen;
    ConversionResult result = ConversionResult::conversionOK;

    if (targetLen < sourceLen) {
        copyLen = targetLen;
        result = ConversionResult::targetExhausted;
    }
    
    memcpy(targetStart, sourceStart, copyLen);
    return result;
}

/* ---------------------------------------------------------------------
    Note A.
    The fall-through switches in UTF-8 reading code save a
    temp variable, some decrements & conditionals.  The switches
    are equivalent to the following loop:
        {
            int tmpBytesToRead = extraBytesToRead+1;
            do {
                ch += *source++;
                --tmpBytesToRead;
                if (tmpBytesToRead) ch <<= 6;
            } while (tmpBytesToRead > 0);
        }
    In UTF-8 writing code, the switches on "bytesToWrite" are
    similarly unrolled loops.
   --------------------------------------------------------------------- */
