#include <string>
#include <iostream>

import convert_utf;

void PrintResult(std::string cat, ConversionResult result) {
    std::cout << cat << ": " << (result == ConversionResult::conversionOK ? "success" : "failed") << '\n';
}

void test() {
    std::basic_string<char> str8 = "hello world! 안녕하세요! おげんきですか";
    
    size_t len32 = ConvertLength(&*str8.begin(), &*str8.end(), char32_t(0));
    std::basic_string<char32_t> str32(len32, '\0');
    PrintResult("char > char32_t", Convert(&*str8.begin(),
                                           &*str8.end(),
                                           &*str32.begin(),
                                           &*str32.end(),
                                           ConversionFlags::strictConversion));
    
    size_t len16 = ConvertLength(&*str8.begin(), &*str8.end(), char16_t(0));
    std::basic_string<char16_t> str16(len16, '\0');
    PrintResult("char > char16_t", Convert(&*str8.begin(),
                                           &*str8.end(),
                                           &*str16.begin(),
                                           &*str16.end(),
                                           ConversionFlags::strictConversion));
}

void test8() {
    std::basic_string<char8_t> stru8 = u8"hello world! 안녕하세요! おげんきですか";
    
    size_t len32 = ConvertLength(&*stru8.begin(), &*stru8.end(), char32_t(0));
    std::basic_string<char32_t> str32(len32, '\0');
    PrintResult("char8_t > char32_t", Convert(&*stru8.begin(),
                                              &*stru8.end(),
                                              &*str32.begin(),
                                              &*str32.end(),
                                              ConversionFlags::strictConversion));
    
    size_t len16 = ConvertLength(&*stru8.begin(), &*stru8.end(), char16_t(0));
    std::basic_string<char16_t> str16(len16, '\0');
    PrintResult("char8_t > char16_t", Convert(&*stru8.begin(),
                                              &*stru8.end(),
                                              &*str16.begin(),
                                              &*str16.end(),
                                              ConversionFlags::strictConversion));
}

void test16() {
    std::basic_string<char16_t> str16 = u"hello world! 안녕하세요! おげんきですか";
    
    size_t len32 = ConvertLength(&*str16.begin(), &*str16.end(), char32_t(0));
    std::basic_string<char32_t> str32(len32, '\0');
    PrintResult("char16 > char32_t", Convert(&*str16.begin(),
                                             &*str16.end(),
                                             &*str32.begin(),
                                             &*str32.end(),
                                             ConversionFlags::strictConversion));
    
    size_t len8 = ConvertLength(&*str16.begin(), &*str16.end(), char8_t(0));
    std::basic_string<char8_t> str8(len8, '\0');
    PrintResult("char16 > char8_t", Convert(&*str16.begin(),
                                            &*str16.end(),
                                            &*str8.begin(),
                                            &*str8.end(),
                                            ConversionFlags::strictConversion));
}

void test32() {
    std::basic_string<char32_t> str32 = U"hello world! 안녕하세요! おげんきですか";
    
    size_t len16 = ConvertLength(&*str32.begin(), &*str32.end(), char16_t(0));
    std::basic_string<char16_t> str16(len16, '\0');
    PrintResult("char32_t > char16_t", Convert(&*str32.begin(),
                                               &*str32.end(),
                                               &*str16.begin(),
                                               &*str16.end(),
                                               ConversionFlags::strictConversion));
    
    size_t len8 = ConvertLength(&*str32.begin(), &*str32.end(), char8_t(0));
    std::basic_string<char8_t> str8(len8, '\0');
    PrintResult("char32 > char8_t", Convert(&*str32.begin(),
                                            &*str32.end(),
                                            &*str8.begin(),
                                            &*str8.end(),
                                            ConversionFlags::strictConversion));
}

void testw() {
    std::wstring wstr = L"hello world! 안녕하세요! おげんきですか";
    
    size_t len8 = ConvertLength(&*wstr.begin(), &*wstr.end(), char8_t(0));
    std::basic_string<char8_t> str8(len8, '\0');
    PrintResult("wchar_t > char8_t", Convert(&*wstr.begin(),
                                             &*wstr.end(),
                                             &*str8.begin(),
                                             &*str8.end(),
                                             ConversionFlags::strictConversion));
    
    size_t len16 = ConvertLength(&*wstr.begin(), &*wstr.end(), char16_t(0));
    std::basic_string<char16_t> str16(len16, '\0');
    PrintResult("wchar_t > char16_t", Convert(&*wstr.begin(),
                                             &*wstr.end(),
                                             &*str16.begin(),
                                             &*str16.end(),
                                             ConversionFlags::strictConversion));
    
    size_t len32 = ConvertLength(&*wstr.begin(), &*wstr.end(), char32_t(0));
    std::basic_string<char32_t> str32(len32, '\0');
    PrintResult("wchar_t > char32_t", Convert(&*wstr.begin(),
                                             &*wstr.end(),
                                             &*str32.begin(),
                                             &*str32.end(),
                                             ConversionFlags::strictConversion));
}

int main() {
    test();
    test8();
    test16();
    test32();
    testw();
    return 0;
}
