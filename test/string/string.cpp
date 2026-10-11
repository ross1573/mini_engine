#include <string>

#include "string_define.h"

import mini.test;

using mini::char16;
using mini::char32;
using mini::char8;
using mini::wchar;

using mini::Allocator;
using mini::BasicString;

[[maybe_unused]] constexpr void StringConstraints()
{
    static_assert(sizeof(BasicString<char>) == 24);
    static_assert(sizeof(BasicString<wchar>) == 24);
    static_assert(sizeof(BasicString<char8>) == 24);
    static_assert(sizeof(BasicString<char16>) == 24);
    static_assert(sizeof(BasicString<char32>) == 24);
}

template <typename T>
constexpr int testSSO()
{
    using CStr = CStr_inline<T>;
    constexpr auto ssoSize = (sizeof(BasicString<T>) / sizeof(T)) - 2;

    TEST_ENSURE(BasicString<T>(CStr::ch, 4).capacity() == ssoSize);
    TEST_ENSURE(BasicString<T>(CStr::l, 30).capacity() == 30);
    TEST_ENSURE(BasicString<T>(CStr::ch, ssoSize - 1).capacity() == ssoSize);

    BasicString<T> str(CStr::l);
    str.resize(4);
    str.shrink();
    TEST_ENSURE(str.capacity() == ssoSize);

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtor()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>{} == CStr::e);
    TEST_ENSURE(BasicString<T>(alloc) == CStr::e);

    TEST_ENSURE(BasicString<T>(CStr::ch, 2).size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 2, alloc).size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32).size() == 32);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32, alloc).size() == 32);

    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s)) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l)) == CStr::l);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l), alloc) == CStr::l);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::s))) == CStr::s);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::s)), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::l))) == CStr::l);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::l)), alloc) == CStr::l);

    TEST_ENSURE(BasicString<T>(CStr::s) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::s, alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::l) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::l, alloc) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::s, 10) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30) == CStr::l_30);
    TEST_ENSURE(BasicString<T>(CStr::s, 10, alloc) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30, alloc) == CStr::l_30);

    BasicString<T> str(CStr::s);
    BasicString<T> str2(str.begin(), str.end());
    BasicString<T> str3(str2.begin(), str2.end(), alloc);
    BasicString<T> str4(CStr::l);
    BasicString<T> str5(str4.begin(), str4.end());
    BasicString<T> str6(str5.begin(), str5.end(), alloc);

    TEST_ENSURE(str == CStr::s);
    TEST_ENSURE(str2 == str);
    TEST_ENSURE(str3 == str);
    TEST_ENSURE(str4 == CStr::l);
    TEST_ENSURE(str5 == str4);
    TEST_ENSURE(str6 == str4);

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssign()
{
    BasicString<T> str;
    BasicString<T> str2;

    str.assign(CStr::s);
    str2.assign(str);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::s);

    str.assign(CStr::l);
    str2.assign(str);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::l);

    str.assign(CStr::s, 10);
    str2.assign(str);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::s_10);

    str.assign(CStr::l, 30);
    str2.assign(str);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::l_30);

    str = str2;
    str2.assign(str.begin(), str.end());
    TEST_ENSURE(str == str2);

    return 0;
}

template <typename T, typename CStr>
constexpr int testAppend()
{
    BasicString<T> str;
    BasicString<T> str2;
    std::basic_string<T> stdstr;

    str.append(CStr::s);
    str2.append(CStr::s);
    stdstr.append(CStr::s);
    TEST_ENSURE(str == CStr::s);
    TEST_ENSURE(str2 == stdstr.data());

    str.clear();
    str.append(CStr::l);
    str2.append(CStr::l);
    stdstr.append(CStr::l);
    TEST_ENSURE(str == CStr::l);
    TEST_ENSURE(str2 == stdstr.data());

    str.clear();
    str.append(CStr::s, 10);
    str2.append(CStr::s, 10);
    stdstr.append(CStr::s, 10);
    TEST_ENSURE(str == CStr::s_10);
    TEST_ENSURE(str2 == stdstr.data());

    str.clear();
    str.append(CStr::l, 30);
    str2.append(CStr::l, 30);
    stdstr.append(CStr::l, 30);
    TEST_ENSURE(str == CStr::l_30);
    TEST_ENSURE(str2 == stdstr.data());

    str2 = CStr::s;
    str.clear();
    str.append(str2);
    TEST_ENSURE(str == CStr::s);

    str2 = CStr::l;
    str.clear();
    str.append(str2.begin(), str2.end());
    TEST_ENSURE(str == CStr::l);

    return 0;
}

template <typename T, typename CStr>
constexpr int testPush()
{
    BasicString<T> str;
    std::basic_string<T> stdstr;

    str.pushBack(CStr::ch);
    stdstr.push_back(CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.pushBack(CStr::ch, 5);
    stdstr.append(5, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.pushBack(CStr::ch, 16);
    stdstr.append(16, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsert()
{
    BasicString<T> str;
    std::basic_string<T> stdstr;

    str.insert(0, CStr::ch);
    stdstr.insert(0, 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(1, CStr::ch, 2);
    stdstr.insert(1, 2, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(2, CStr::s);
    stdstr.insert(2, CStr::s);
    TEST_ENSURE(str == stdstr.data());

    str.insert(3, CStr::s, 10);
    stdstr.insert(3, CStr::s, 0, 10);
    TEST_ENSURE(str == stdstr.data());

    str.insert(4, str);
    stdstr.insert(4, stdstr);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin(), CStr::ch);
    stdstr.insert(stdstr.begin(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), CStr::ch);
    stdstr.insert(stdstr.end(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin() + 1, CStr::s);
    stdstr.insert(1, CStr::s);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin() + 2, CStr::s, 10);
    stdstr.insert(2, CStr::s, 10);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin() + 3, str);
    stdstr.insert(3, stdstr);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(4, str.begin() + 1, str.begin() + 8);
    stdstr.insert(stdstr.begin() + 4, stdstr.begin() + 1, stdstr.begin() + 8);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(str.begin(), str.begin(), str.begin());
    stdstr.insert(stdstr.begin(), stdstr.begin(), stdstr.begin());
    TEST_ENSURE(str == stdstr.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemove()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.popBack();
    stdstr.pop_back();
    TEST_ENSURE(str == stdstr.data());

    str.popBack(5);
    stdstr.erase(stdstr.size() - 5, 5);
    TEST_ENSURE(str == stdstr.data());

    str.remove(1);
    stdstr.erase(stdstr.begin() + 1);
    TEST_ENSURE(str == stdstr.data());

    str.remove(str.begin());
    stdstr.erase(stdstr.begin());
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(2, 4);
    stdstr.erase(stdstr.begin() + 2, stdstr.begin() + 6);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.end(), 5);
    stdstr.erase(stdstr.end(), stdstr.end()); // MSVC refuses to handle it
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.begin(), 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.begin(), str.begin() + 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testModify()
{
    BasicString<char> str;
    BasicString<char> str2;

    str.resize(42, 'a');
    TEST_ENSURE(str.size() == 42 && str.capacity() == 42);

    str.reserve(62);
    TEST_ENSURE(str.capacity() == 62);

    str.shrink();
    TEST_ENSURE(str.capacity() == 42);

    str.clear();
    TEST_ENSURE(str.capacity() == 42 && str.empty());

    str2.swap(str);
    TEST_ENSURE(str2.empty() && str2.capacity() == 42);

    return 0;
}

template <typename T, typename CStr>
constexpr int testOperator()
{
    BasicString<T> str;

    str = CStr::e;
    TEST_ENSURE(str == static_cast<T const*>(nullptr));
    TEST_ENSURE((str == CStr::e && CStr::e == str));

    str = CStr::s;
    TEST_ENSURE(str[10] == str.at(10));
    TEST_ENSURE((str == CStr::s && CStr::s == str));

    str = CStr::l;
    TEST_ENSURE(str[10] == str.at(10));
    TEST_ENSURE((str == CStr::l && CStr::l == str));

    str.clear();
    str += CStr::s_10;
    TEST_ENSURE(str == CStr::s_10);

    str += CStr::s[10];
    str += CStr::s[11];
    str += CStr::s[12];
    TEST_ENSURE(str == CStr::s);

    return 0;
}

int main()
{
    // small-buffer-optimization is only applied on runtime
    TEST_ENSURE((testSSO<char>() == 0));
    TEST_ENSURE((testSSO<wchar>() == 0));
    TEST_ENSURE((testSSO<char8>() == 0));
    TEST_ENSURE((testSSO<char16>() == 0));
    TEST_ENSURE((testSSO<char32>() == 0));

    TEST_STRING(testCtor, char);
    TEST_STRING(testCtor, wchar);
    TEST_STRING(testCtor, char8);
    TEST_STRING(testCtor, char16);
    TEST_STRING(testCtor, char32);

    TEST_STRING(testAssign, char);
    TEST_STRING(testAssign, wchar);
    TEST_STRING(testAssign, char8);
    TEST_STRING(testAssign, char16);
    TEST_STRING(testAssign, char32);

    TEST_STRING(testAppend, char);
    TEST_STRING(testAppend, wchar);
    TEST_STRING(testAppend, char8);
    TEST_STRING(testAppend, char16);
    TEST_STRING(testAppend, char32);

    TEST_STRING(testPush, char);
    TEST_STRING(testPush, wchar);
    TEST_STRING(testPush, char8);
    TEST_STRING(testPush, char16);
    TEST_STRING(testPush, char32);

    TEST_STRING(testInsert, char);
    TEST_STRING(testInsert, wchar);
    TEST_STRING(testInsert, char8);
    TEST_STRING(testInsert, char16);
    TEST_STRING(testInsert, char32);

    TEST_STRING(testRemove, char);
    TEST_STRING(testRemove, wchar);
    TEST_STRING(testRemove, char8);
    TEST_STRING(testRemove, char16);
    TEST_STRING(testRemove, char32);

    TEST_STRING(testModify, char);
    TEST_STRING(testModify, wchar);
    TEST_STRING(testModify, char8);
    TEST_STRING(testModify, char16);
    TEST_STRING(testModify, char32);

    TEST_STRING(testOperator, char);
    TEST_STRING(testOperator, wchar);
    TEST_STRING(testOperator, char8);
    TEST_STRING(testOperator, char16);
    TEST_STRING(testOperator, char32);

    return 0;
}