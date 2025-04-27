#include <string>
#include <string_view>

#include "test_macro.h"

import mini.test;
import convert_utf;

constexpr std::basic_string_view<char> str = "hello world! 안녕하세요! おげんきですか";
constexpr std::basic_string_view<wchar_t> wstr = L"hello world! 안녕하세요! おげんきですか";
constexpr std::basic_string_view<char8_t> str8 = u8"hello world! 안녕하세요! おげんきですか";
constexpr std::basic_string_view<char16_t> str16 = u"hello world! 안녕하세요! おげんきですか";
constexpr std::basic_string_view<char32_t> str32 = U"hello world! 안녕하세요! おげんきですか";

template <typename T, typename U>
constexpr bool test_impl(std::basic_string_view<T> srcPtr, std::basic_string_view<U> dstPtr)
{
    std::basic_string<T> src(srcPtr);
    std::basic_string<U> target(dstPtr);

    size_t len = ConvertLength(&*(src.begin()), &*(src.end()), U(0));
    std::basic_string<U> dst(len, '\0');

    auto result = Convert(&*(src.begin()), &*(src.end()), &*(dst.begin()), &*(dst.end()),
                          ConversionFlags::strictConversion);

    return result == ConversionResult::conversionOK && dst == target;
}

constexpr void constexpr_test()
{
    static_assert(test_impl(str, str));
    static_assert(test_impl(str, wstr));
    static_assert(test_impl(str, str8));
    static_assert(test_impl(str, str16));
    static_assert(test_impl(str, str32));

    static_assert(test_impl(wstr, str));
    static_assert(test_impl(wstr, wstr));
    static_assert(test_impl(wstr, str8));
    static_assert(test_impl(wstr, str16));
    static_assert(test_impl(wstr, str32));

    static_assert(test_impl(str8, str));
    static_assert(test_impl(str8, wstr));
    static_assert(test_impl(str8, str8));
    static_assert(test_impl(str8, str16));
    static_assert(test_impl(str8, str32));

    static_assert(test_impl(str16, str));
    static_assert(test_impl(str16, wstr));
    static_assert(test_impl(str16, str8));
    static_assert(test_impl(str16, str16));
    static_assert(test_impl(str16, str32));

    static_assert(test_impl(str32, str));
    static_assert(test_impl(str32, wstr));
    static_assert(test_impl(str32, str8));
    static_assert(test_impl(str32, str16));
    static_assert(test_impl(str32, str32));
}

int test()
{
    TEST_ENSURE(test_impl(str, str));
    TEST_ENSURE(test_impl(str, wstr));
    TEST_ENSURE(test_impl(str, str8));
    TEST_ENSURE(test_impl(str, str16));
    TEST_ENSURE(test_impl(str, str32));

    TEST_ENSURE(test_impl(wstr, str));
    TEST_ENSURE(test_impl(wstr, wstr));
    TEST_ENSURE(test_impl(wstr, str8));
    TEST_ENSURE(test_impl(wstr, str16));
    TEST_ENSURE(test_impl(wstr, str32));

    TEST_ENSURE(test_impl(str8, str));
    TEST_ENSURE(test_impl(str8, wstr));
    TEST_ENSURE(test_impl(str8, str8));
    TEST_ENSURE(test_impl(str8, str16));
    TEST_ENSURE(test_impl(str8, str32));

    TEST_ENSURE(test_impl(str16, str));
    TEST_ENSURE(test_impl(str16, wstr));
    TEST_ENSURE(test_impl(str16, str8));
    TEST_ENSURE(test_impl(str16, str16));
    TEST_ENSURE(test_impl(str16, str32));

    TEST_ENSURE(test_impl(str32, str));
    TEST_ENSURE(test_impl(str32, wstr));
    TEST_ENSURE(test_impl(str32, str8));
    TEST_ENSURE(test_impl(str32, str16));
    TEST_ENSURE(test_impl(str32, str32));

    return 0;
}

int main()
{
    constexpr_test();
    return test();
}
