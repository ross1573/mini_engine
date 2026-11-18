#include "test_macro.h"

import mini.test;
import convert_utf;

using mini::BasicString;
using mini::BasicStringView;
using utf::ConversionFlags;
using utf::ConversionResult;

struct Mixed {
    static constexpr char const* str = "hello world! 안녕하세요! おげんきですか";
    static constexpr mini::wchar const* wstr = L"hello world! 안녕하세요! おげんきですか";
    static constexpr mini::char8 const* str8 = u8"hello world! 안녕하세요! おげんきですか";
    static constexpr mini::char16 const* str16 = u"hello world! 안녕하세요! おげんきですか";
    static constexpr mini::char32 const* str32 = U"hello world! 안녕하세요! おげんきですか";
};

struct Supplementary {
    static constexpr char const* str = "emoji \U0001F600\U0001F680 math \U0001D49C end";
    static constexpr mini::wchar const* wstr = L"emoji \U0001F600\U0001F680 math \U0001D49C end";
    static constexpr mini::char8 const* str8 = u8"emoji \U0001F600\U0001F680 math \U0001D49C end";
    static constexpr mini::char16 const* str16 = u"emoji \U0001F600\U0001F680 math \U0001D49C end";
    static constexpr mini::char32 const* str32 = U"emoji \U0001F600\U0001F680 math \U0001D49C end";
};

struct Boundary {
    static constexpr char const* str = "\u007F\u0080߿ࠀ퟿�";
    static constexpr mini::wchar const* wstr = L"\u007F\u0080߿ࠀ퟿�";
    static constexpr mini::char8 const* str8 = u8"\u007F\u0080߿ࠀ퟿�";
    static constexpr mini::char16 const* str16 = u"\u007F\u0080߿ࠀ퟿�";
    static constexpr mini::char32 const* str32 = U"\u007F\u0080߿ࠀ퟿�";
};

struct BmpEnd {
    static constexpr char const* str = "a\uFFFFb";
    static constexpr mini::wchar const* wstr = L"a\uFFFFb";
    static constexpr mini::char8 const* str8 = u8"a\uFFFFb";
    static constexpr mini::char16 const* str16 = u"a\uFFFFb";
    static constexpr mini::char32 const* str32 = U"a\uFFFFb";
};

struct PlaneBoundary {
    static constexpr char const* str = "\U00010000 \U0010FFFF";
    static constexpr mini::wchar const* wstr = L"\U00010000 \U0010FFFF";
    static constexpr mini::char8 const* str8 = u8"\U00010000 \U0010FFFF";
    static constexpr mini::char16 const* str16 = u"\U00010000 \U0010FFFF";
    static constexpr mini::char32 const* str32 = U"\U00010000 \U0010FFFF";
};

struct Empty {
    static constexpr char const* str = "";
    static constexpr mini::wchar const* wstr = L"";
    static constexpr mini::char8 const* str8 = u8"";
    static constexpr mini::char16 const* str16 = u"";
    static constexpr mini::char32 const* str32 = U"";
};

template <typename T, typename U>
constexpr bool ensureConvertUTF(T const* from, U const* to)
{
    BasicString<T> src(from);
    BasicString<U> target(to);

    mini::size_t len = utf::ConvertLength(src.data(), src.data() + src.size(), U(0));
    if (len != target.size()) {
        return false;
    }

    BasicString<U> dst(U(0), len);

    auto result = utf::Convert(src.data(),
                               src.data() + src.size(),
                               dst.data(),
                               dst.data() + dst.size(),
                               ConversionFlags::StrictConversion);

    return result == ConversionResult::ConversionOK && dst == target;
}

template <typename T, typename U>
constexpr bool ensureStringConvert(T const* from, U const* to)
{
    BasicString<U> c = mini::convert<U>(from);
    if (c != to) {
        return false;
    }

    BasicString<T> r = mini::convert<T>(c);
    return r == from;
}

template <typename T>
struct ConvertAlloc : mini::Allocator<T> { };

template <typename Set>
constexpr mini::int32 testConvertUTF()
{
    TEST_ENSURE(ensureConvertUTF(Set::str, Set::str));
    TEST_ENSURE(ensureConvertUTF(Set::str, Set::wstr));
    TEST_ENSURE(ensureConvertUTF(Set::str, Set::str8));
    TEST_ENSURE(ensureConvertUTF(Set::str, Set::str16));
    TEST_ENSURE(ensureConvertUTF(Set::str, Set::str32));
    TEST_ENSURE(ensureConvertUTF(Set::wstr, Set::str));
    TEST_ENSURE(ensureConvertUTF(Set::wstr, Set::wstr));
    TEST_ENSURE(ensureConvertUTF(Set::wstr, Set::str8));
    TEST_ENSURE(ensureConvertUTF(Set::wstr, Set::str16));
    TEST_ENSURE(ensureConvertUTF(Set::wstr, Set::str32));
    TEST_ENSURE(ensureConvertUTF(Set::str8, Set::str));
    TEST_ENSURE(ensureConvertUTF(Set::str8, Set::wstr));
    TEST_ENSURE(ensureConvertUTF(Set::str8, Set::str8));
    TEST_ENSURE(ensureConvertUTF(Set::str8, Set::str16));
    TEST_ENSURE(ensureConvertUTF(Set::str8, Set::str32));
    TEST_ENSURE(ensureConvertUTF(Set::str16, Set::str));
    TEST_ENSURE(ensureConvertUTF(Set::str16, Set::wstr));
    TEST_ENSURE(ensureConvertUTF(Set::str16, Set::str8));
    TEST_ENSURE(ensureConvertUTF(Set::str16, Set::str16));
    TEST_ENSURE(ensureConvertUTF(Set::str16, Set::str32));
    TEST_ENSURE(ensureConvertUTF(Set::str32, Set::str));
    TEST_ENSURE(ensureConvertUTF(Set::str32, Set::wstr));
    TEST_ENSURE(ensureConvertUTF(Set::str32, Set::str8));
    TEST_ENSURE(ensureConvertUTF(Set::str32, Set::str16));
    TEST_ENSURE(ensureConvertUTF(Set::str32, Set::str32));

    return 0;
}

template <typename Set>
constexpr mini::int32 testStringConvert()
{
    TEST_ENSURE(ensureStringConvert(Set::str, Set::wstr));
    TEST_ENSURE(ensureStringConvert(Set::str, Set::str8));
    TEST_ENSURE(ensureStringConvert(Set::str, Set::str16));
    TEST_ENSURE(ensureStringConvert(Set::str, Set::str32));
    TEST_ENSURE(ensureStringConvert(Set::wstr, Set::str));
    TEST_ENSURE(ensureStringConvert(Set::wstr, Set::str8));
    TEST_ENSURE(ensureStringConvert(Set::wstr, Set::str16));
    TEST_ENSURE(ensureStringConvert(Set::wstr, Set::str32));
    TEST_ENSURE(ensureStringConvert(Set::str8, Set::str));
    TEST_ENSURE(ensureStringConvert(Set::str8, Set::wstr));
    TEST_ENSURE(ensureStringConvert(Set::str8, Set::str16));
    TEST_ENSURE(ensureStringConvert(Set::str8, Set::str32));
    TEST_ENSURE(ensureStringConvert(Set::str16, Set::str));
    TEST_ENSURE(ensureStringConvert(Set::str16, Set::wstr));
    TEST_ENSURE(ensureStringConvert(Set::str16, Set::str8));
    TEST_ENSURE(ensureStringConvert(Set::str16, Set::str32));
    TEST_ENSURE(ensureStringConvert(Set::str32, Set::str));
    TEST_ENSURE(ensureStringConvert(Set::str32, Set::wstr));
    TEST_ENSURE(ensureStringConvert(Set::str32, Set::str8));
    TEST_ENSURE(ensureStringConvert(Set::str32, Set::str16));

    return 0;
}

template <typename Set>
constexpr mini::int32 testSameTypeConvert()
{
    TEST_ENSURE(mini::convert<char>(BasicStringView<char>(Set::str)) == Set::str);
    TEST_ENSURE(mini::convert<mini::wchar>(BasicStringView<mini::wchar>(Set::wstr)) == Set::wstr);
    TEST_ENSURE(mini::convert<mini::char8>(BasicStringView<mini::char8>(Set::str8)) == Set::str8);
    TEST_ENSURE(mini::convert<mini::char16>(BasicStringView<mini::char16>(Set::str16)) == Set::str16);
    TEST_ENSURE(mini::convert<mini::char32>(BasicStringView<mini::char32>(Set::str32)) == Set::str32);

    return 0;
}

constexpr mini::int32 testConvertTo()
{
    BasicString<mini::char16> str16(u"abc");
    mini::convertToImpl(str16, BasicStringView<char>(Mixed::str));
    TEST_ENSURE(str16 == (BasicString<mini::char16>(u"abc") + BasicStringView<mini::char16>(Mixed::str16)));

    BasicString<char> str;
    mini::convertToImpl(str, BasicStringView<mini::char32>(Supplementary::str32));
    TEST_ENSURE(str == Supplementary::str);

    mini::convertToImpl(str, BasicStringView<mini::char32>());
    TEST_ENSURE(str == Supplementary::str);

    BasicString<mini::char32> str32(U"x");
    mini::convertToImpl(str32, BasicStringView<mini::char8>(PlaneBoundary::str8));
    TEST_ENSURE(str32 == (BasicString<mini::char32>(U"x") + BasicStringView<mini::char32>(PlaneBoundary::str32)));

    return 0;
}

constexpr mini::int32 testConvertAllocator()
{
    auto str32 = mini::convert<mini::char32, ConvertAlloc<mini::char32>>(Mixed::str);
    TEST_ENSURE(str32 == Mixed::str32);

    auto str = mini::convert<char, ConvertAlloc<char>>(str32);
    TEST_ENSURE(str == Mixed::str);

    return 0;
}

constexpr mini::int32 testConvertSizedView()
{
    BasicString<mini::char16> hello16 = mini::convert<mini::char16>(BasicStringView<char>(Mixed::str, 5));
    TEST_ENSURE(hello16.size() == 5);
    TEST_ENSURE(hello16 == u"hello");
    TEST_ENSURE(hello16.data()[hello16.size()] == mini::char16(0));

    BasicStringView<mini::char32> korean32(Mixed::str32 + 13, 5);
    BasicString<mini::char8> korean8 = mini::convert<mini::char8>(korean32);
    TEST_ENSURE(korean8.size() == 15);
    TEST_ENSURE(korean8 == u8"안녕하세요");
    TEST_ENSURE(korean8.data()[korean8.size()] == mini::char8(0));

    BasicString<mini::char32> back32 = mini::convert<mini::char32>(BasicStringView<mini::char8>(korean8));
    TEST_ENSURE(back32 == korean32);

    BasicString<mini::wchar> wide = mini::convert<mini::wchar>(BasicStringView<mini::char8>(korean8.data(), 6));
    TEST_ENSURE(wide.size() == 2);
    TEST_ENSURE(wide == L"안녕");

    return 0;
}

constexpr mini::int32 testBmpEndEncoding()
{
    BasicString<mini::char16> str16 = mini::convert<mini::char16>(BasicStringView<mini::char8>(BmpEnd::str8));
    TEST_ENSURE(str16.size() == 3);
    TEST_ENSURE(str16 == BmpEnd::str16);

    BasicString<mini::char8> str8 = mini::convert<mini::char8>(BasicStringView<mini::char32>(BmpEnd::str32));
    TEST_ENSURE(str8.size() == 5);
    TEST_ENSURE(str8 == BmpEnd::str8);

    return 0;
}

constexpr mini::int32 testUtf16ToUtf8DropsNonCharacters()
{
    mini::char16 const nonCharacters[] = {
        u'a',
        mini::char16{0xFFFF},
        u'b',
        mini::char16{0xFEFF},
        u'c',
        mini::char16{0xFFFE},
    };

    BasicString<mini::char8> fromUtf16 = mini::convert<mini::char8>(BasicStringView<mini::char16>(BmpEnd::str16));
    TEST_ENSURE(fromUtf16.size() == 2);
    TEST_ENSURE(fromUtf16 == u8"ab");

    BasicString<char> fromWide = mini::convert<char>(BasicStringView<mini::wchar>(BmpEnd::wstr));
    TEST_ENSURE(fromWide == "ab");

    BasicString<mini::char8> filtered = mini::convert<mini::char8>(BasicStringView<mini::char16>(nonCharacters, 6));
    TEST_ENSURE(filtered == u8"abc");

    return 0;
}

constexpr mini::int32 testPlaneBoundaryEncoding()
{
    BasicString<mini::char16> str16 = mini::convert<mini::char16>(BasicStringView<mini::char8>(PlaneBoundary::str8));
    TEST_ENSURE(str16.size() == 5);
    TEST_ENSURE(str16 == PlaneBoundary::str16);
    TEST_ENSURE(str16[0] == mini::char16(0xD800));
    TEST_ENSURE(str16[1] == mini::char16(0xDC00));
    TEST_ENSURE(str16[3] == mini::char16(0xDBFF));
    TEST_ENSURE(str16[4] == mini::char16(0xDFFF));

    BasicString<mini::char8> str8 = mini::convert<mini::char8>(BasicStringView<mini::char32>(PlaneBoundary::str32));
    TEST_ENSURE(str8.size() == 9);
    TEST_ENSURE(str8 == PlaneBoundary::str8);
    TEST_ENSURE(str8[0] == mini::char8(0xF0));
    TEST_ENSURE(str8[1] == mini::char8(0x90));
    TEST_ENSURE(str8[2] == mini::char8(0x80));
    TEST_ENSURE(str8[3] == mini::char8(0x80));

    BasicString<mini::char8> fromUtf16 = mini::convert<mini::char8>(BasicStringView<mini::char16>(PlaneBoundary::str16));
    TEST_ENSURE(fromUtf16.size() == 9);
    TEST_ENSURE(fromUtf16 == PlaneBoundary::str8);

    BasicString<mini::char32> str32 = mini::convert<mini::char32>(BasicStringView<mini::char16>(PlaneBoundary::str16));
    TEST_ENSURE(str32.size() == 3);
    TEST_ENSURE(str32[0] == mini::char32(0x10000));
    TEST_ENSURE(str32[2] == mini::char32(0x10FFFF));

    return 0;
}

constexpr mini::int32 testConvertEmbeddedNull()
{
    char const src[] = {'a', '\0', 'b'};
    BasicString<mini::char32> str32 = mini::convert<mini::char32>(BasicStringView<char>(src, 3));
    TEST_ENSURE(str32.size() == 3);
    TEST_ENSURE(str32[0] == U'a');
    TEST_ENSURE(str32[1] == U'\0');
    TEST_ENSURE(str32[2] == U'b');

    BasicString<char> back = mini::convert<char>(BasicStringView<mini::char32>(str32));
    TEST_ENSURE(back.size() == 3);
    TEST_ENSURE(back[1] == '\0');
    TEST_ENSURE(back[2] == 'b');

    return 0;
}

int main()
{
    TEST_CONSTEXPR_TEMPLATE(testConvertUTF, Mixed);
    TEST_CONSTEXPR_TEMPLATE(testConvertUTF, Boundary);
    TEST_CONSTEXPR_TEMPLATE(testConvertUTF, Empty);
    TEST_CONSTEXPR_TEMPLATE(testConvertUTF, Supplementary);
    TEST_CONSTEXPR_TEMPLATE(testConvertUTF, PlaneBoundary);

    TEST_CONSTEXPR_TEMPLATE(testStringConvert, Mixed);
    TEST_CONSTEXPR_TEMPLATE(testStringConvert, Boundary);
    TEST_CONSTEXPR_TEMPLATE(testStringConvert, Empty);
    TEST_CONSTEXPR_TEMPLATE(testStringConvert, Supplementary);
    TEST_CONSTEXPR_TEMPLATE(testStringConvert, PlaneBoundary);

    TEST_CONSTEXPR_TEMPLATE(testSameTypeConvert, Mixed);

    TEST_CONSTEXPR(testConvertTo);
    TEST_CONSTEXPR(testConvertAllocator);
    TEST_CONSTEXPR(testConvertSizedView);

    TEST_CONSTEXPR(testBmpEndEncoding);
    TEST_CONSTEXPR(testUtf16ToUtf8DropsNonCharacters);
    TEST_CONSTEXPR(testPlaneBoundaryEncoding);
    TEST_CONSTEXPR(testConvertEmbeddedNull);

    return 0;
}
