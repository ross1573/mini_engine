#include "test_macro.h"

import mini.test;
import convert_utf;

#define TEST_CONVERT_UTF(from, to)           \
    static_assert(testConvertUTF(from, to)); \
    TEST_ENSURE(testConvertUTF(from, to));

#define TEST_STRING_CONVERT(from, to)           \
    static_assert(testStringConvert(from, to)); \
    TEST_ENSURE(testStringConvert(from, to));

using mini::BasicString;
using mini::BasicStringConvert;
using mini::BasicStringView;

using mini::char16;
using mini::char32;
using mini::char8;
using mini::wchar;

constexpr char const* str = "hello world! 안녕하세요! おげんきですか";
constexpr wchar const* wstr = L"hello world! 안녕하세요! おげんきですか";
constexpr char8 const* str8 = u8"hello world! 안녕하세요! おげんきですか";
constexpr char16 const* str16 = u"hello world! 안녕하세요! おげんきですか";
constexpr char32 const* str32 = U"hello world! 안녕하세요! おげんきですか";

template <typename T, typename U>
constexpr bool testConvertUTF(T const* from, U const* to)
{
    BasicString<T> src(from);
    BasicString<U> target(to);

    size_t len = utf::ConvertLength(src.data(), src.data() + src.size(), U(0));
    BasicString<U> dst(U(0), len);

    auto result = utf::Convert(src.data(),
                               src.data() + src.size(),
                               dst.data(),
                               dst.data() + dst.size(),
                               utf::ConversionFlags::StrictConversion);
    return result == utf::ConversionResult::ConversionOK && dst == target;
}

template <typename T, typename U>
constexpr bool testStringConvert(T const* from, U const* to)
{
    BasicString<U> c = BasicStringConvert<U>(BasicString<T>(from)).toString();
    if (c != to) {
        return false;
    }

    BasicStringConvert<U> convert(from);
    return static_cast<BasicStringView<U>>(convert) == to;
}

int main()
{
    TEST_CONVERT_UTF(str, str);
    TEST_CONVERT_UTF(str, wstr);
    TEST_CONVERT_UTF(str, str8);
    TEST_CONVERT_UTF(str, str16);
    TEST_CONVERT_UTF(str, str32);

    TEST_CONVERT_UTF(wstr, str);
    TEST_CONVERT_UTF(wstr, wstr);
    TEST_CONVERT_UTF(wstr, str8);
    TEST_CONVERT_UTF(wstr, str16);
    TEST_CONVERT_UTF(wstr, str32);

    TEST_CONVERT_UTF(str8, str);
    TEST_CONVERT_UTF(str8, wstr);
    TEST_CONVERT_UTF(str8, str8);
    TEST_CONVERT_UTF(str8, str16);
    TEST_CONVERT_UTF(str8, str32);

    TEST_CONVERT_UTF(str16, str);
    TEST_CONVERT_UTF(str16, wstr);
    TEST_CONVERT_UTF(str16, str8);
    TEST_CONVERT_UTF(str16, str16);
    TEST_CONVERT_UTF(str16, str32);

    TEST_CONVERT_UTF(str32, str);
    TEST_CONVERT_UTF(str32, wstr);
    TEST_CONVERT_UTF(str32, str8);
    TEST_CONVERT_UTF(str32, str16);
    TEST_CONVERT_UTF(str32, str32);

    TEST_STRING_CONVERT(str, wstr);
    TEST_STRING_CONVERT(str, str8);
    TEST_STRING_CONVERT(str, str16);
    TEST_STRING_CONVERT(str, str32);

    TEST_STRING_CONVERT(wstr, str);
    TEST_STRING_CONVERT(wstr, str8);
    TEST_STRING_CONVERT(wstr, str16);
    TEST_STRING_CONVERT(wstr, str32);

    TEST_STRING_CONVERT(str8, str);
    TEST_STRING_CONVERT(str8, wstr);
    TEST_STRING_CONVERT(str8, str16);
    TEST_STRING_CONVERT(str8, str32);

    TEST_STRING_CONVERT(str16, str);
    TEST_STRING_CONVERT(str16, wstr);
    TEST_STRING_CONVERT(str16, str8);
    TEST_STRING_CONVERT(str16, str32);

    TEST_STRING_CONVERT(str32, str);
    TEST_STRING_CONVERT(str32, wstr);
    TEST_STRING_CONVERT(str32, str8);
    TEST_STRING_CONVERT(str32, str16);

    return 0;
}
