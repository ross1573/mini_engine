import mini.test;
import convert_utf;

#define TEST_CONVERT_UTF(from, to)           \
    static_assert(TestConvertUTF(from, to)); \
    TEST_ENSURE(TestConvertUTF(from, to));

using namespace mini;

constexpr char const* str = "hello world! 안녕하세요! おげんきですか";
constexpr wchar const* wstr = L"hello world! 안녕하세요! おげんきですか";
constexpr char8 const* str8 = u8"hello world! 안녕하세요! おげんきですか";
constexpr char16 const* str16 = u"hello world! 안녕하세요! おげんきですか";
constexpr char32 const* str32 = U"hello world! 안녕하세요! おげんきですか";

template <typename T, typename U>
constexpr bool TestConvertUTF(T const* from, U const* to)
{
    BasicString<T> src(from);
    BasicString<U> target(to);

    size_t len = ConvertLength(src.Data(), src.Data() + src.Size(), U(0));
    BasicString<U> dst(U(0), static_cast<SizeT>(len));

    auto result = Convert(src.Data(), src.Data() + src.Size(), dst.Data(), dst.Data() + dst.Size(),
                          ConversionFlags::strictConversion);
    return result == ConversionResult::conversionOK && dst == target;
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

    return 0;
}
