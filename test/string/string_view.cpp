#include <string_view>

#include "string_define.h"

import mini.test;

using mini::BasicString;
using mini::BasicStringView;

using mini::char16;
using mini::char32;
using mini::char8;
using mini::wchar;

[[maybe_unused]] constexpr void StringViewConstraints()
{
    static_assert(sizeof(BasicStringView<char>) == 16);
    static_assert(sizeof(BasicStringView<wchar>) == 16);
    static_assert(sizeof(BasicStringView<char8>) == 16);
    static_assert(sizeof(BasicStringView<char16>) == 16);
    static_assert(sizeof(BasicStringView<char32>) == 16);
}

template <typename T, typename CStr>
constexpr int testCtor()
{
    TEST_ENSURE(BasicStringView<T>{} == CStr::e);

    TEST_ENSURE(BasicStringView<T>(BasicString<T>(CStr::s)) == CStr::s);
    TEST_ENSURE(BasicStringView<T>(BasicStringView<T>(CStr::s)) == CStr::s);
    TEST_ENSURE(BasicStringView<T>(BasicString<T>(CStr::l)) == CStr::l);
    TEST_ENSURE(BasicStringView<T>(BasicStringView<T>(CStr::l)) == CStr::l);

    TEST_ENSURE(BasicStringView<T>(CStr::s) == CStr::s);
    TEST_ENSURE(BasicStringView<T>(CStr::l) == CStr::l);
    TEST_ENSURE(BasicStringView<T>(CStr::s, 10) == CStr::s_10);
    TEST_ENSURE(BasicStringView<T>(CStr::l, 30) == CStr::l_30);

    return 0;
}

template <typename T, typename CStr>
constexpr int TestCopy()
{
    BasicStringView<T> svs = CStr::s;
    BasicString<T> ss{};
    ss.resize(svs.size());

    svs.copy(ss.data(), 0, svs.size());
    TEST_ENSURE(svs == ss);

    svs.copy(ss.data(), svs.begin(), svs.end());
    TEST_ENSURE(svs == ss);

    svs.copy(ss.data(), 5, svs.size() - 5);
    ss.resize(svs.size() - 5);
    TEST_ENSURE(svs.subView(5, svs.size() - 5) == ss);

    svs.copy(ss.data(), svs.begin() + 5, svs.end());
    TEST_ENSURE(svs.subBack(svs.size() - 5) == ss);

    return 0;
}

template <typename T, typename CStr>
constexpr int TestSubString()
{
    BasicStringView<T> sv = CStr::l;

    BasicStringView<T> sv1 = sv.subFront(30);
    TEST_ENSURE(sv1 == CStr::l_30);

    BasicStringView<T> sv2 = sv.subBack(10);
    BasicString<T> s2(CStr::l + sv.size() - 10);
    TEST_ENSURE(sv2 == s2);

    BasicStringView<T> sv3 = sv.subView(0, 30);
    TEST_ENSURE(sv3 == CStr::l_30);

    BasicStringView<T> sv4 = sv.subView(sv.begin() + 10, sv.end());
    BasicString<T> s4(CStr::l + 10);
    TEST_ENSURE(sv4 == s4);

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemove()
{
    BasicStringView<T> sv = CStr::l;
    BasicString<T> s = CStr::l;

    sv.popFront();
    s.removeRange(0, 1);
    TEST_ENSURE(sv == s);

    sv.popFront(5);
    s.removeRange(0, 5);
    TEST_ENSURE(sv == s);

    sv.popBack();
    s.popBack();
    TEST_ENSURE(sv == s);

    sv.popBack(5);
    s.popBack(5);
    TEST_ENSURE(sv == s);

    return 0;
}

template <typename T, typename CStr>
constexpr int testOperator()
{
    BasicStringView<T> view;

    view = CStr::e;
    TEST_ENSURE(view == static_cast<T const*>(nullptr));
    TEST_ENSURE((view == CStr::e && CStr::e == view));

    view = CStr::s_10;
    TEST_ENSURE(view[9] == view.last());

    return 0;
}

int main()
{
    TEST_STRING(testCtor, char);
    TEST_STRING(testCtor, wchar);
    TEST_STRING(testCtor, char8);
    TEST_STRING(testCtor, char16);
    TEST_STRING(testCtor, char32);

    TEST_STRING(TestCopy, char);
    TEST_STRING(TestCopy, wchar);
    TEST_STRING(TestCopy, char8);
    TEST_STRING(TestCopy, char16);
    TEST_STRING(TestCopy, char32);

    TEST_STRING(TestSubString, char);
    TEST_STRING(TestSubString, wchar);
    TEST_STRING(TestSubString, char8);
    TEST_STRING(TestSubString, char16);
    TEST_STRING(TestSubString, char32);

    TEST_STRING(testRemove, char);
    TEST_STRING(testRemove, wchar);
    TEST_STRING(testRemove, char8);
    TEST_STRING(testRemove, char16);
    TEST_STRING(testRemove, char32);

    TEST_STRING(testOperator, char);
    TEST_STRING(testOperator, wchar);
    TEST_STRING(testOperator, char8);
    TEST_STRING(testOperator, char16);
    TEST_STRING(testOperator, char32);

    return 0;
}