#include <string_view>

#include "string_define.h"

import mini.test;
import mini.string;

using namespace mini;
using namespace mini::test;

[[maybe_unused]] static constexpr void StringViewConstraints()
{
    static_assert(sizeof(BasicStringView<char>) == 16);
    static_assert(sizeof(BasicStringView<wchar>) == 16);
    static_assert(sizeof(BasicStringView<char8>) == 16);
    static_assert(sizeof(BasicStringView<char16>) == 16);
    static_assert(sizeof(BasicStringView<char32>) == 16);
}

template <typename T, typename CStr>
static constexpr int TestCtor()
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
static constexpr int TestCopy()
{
    BasicStringView<T> svs = CStr::s;
    BasicString<T> ss{};
    ss.Resize(svs.Size());

    svs.Copy(ss.Data(), 0, svs.Size());
    TEST_ENSURE(svs == ss);

    svs.Copy(ss.Data(), svs.Begin(), svs.End());
    TEST_ENSURE(svs == ss);

    svs.Copy(ss.Data(), 5, svs.Size() - 5);
    ss.Resize(svs.Size() - 5);
    TEST_ENSURE(svs.SubString(5, svs.Size() - 5) == ss);

    svs.Copy(ss.Data(), svs.Begin() + 5, svs.End());
    TEST_ENSURE(svs.SubLast(svs.Size() - 5) == ss);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestSubString()
{
    BasicStringView<T> sv = CStr::l;

    BasicStringView<T> sv1 = sv.SubFirst(30);
    TEST_ENSURE(sv1 == CStr::l_30);

    BasicStringView<T> sv2 = sv.SubLast(10);
    BasicString<T> s2(CStr::l + sv.Size() - 10);
    TEST_ENSURE(sv2 == s2);

    BasicStringView<T> sv3 = sv.SubString(0, 30);
    TEST_ENSURE(sv3 == CStr::l_30);

    BasicStringView<T> sv4 = sv.SubString(sv.Begin() + 10, sv.End());
    BasicString<T> s4(CStr::l + 10);
    TEST_ENSURE(sv4 == s4);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestRemove()
{
    BasicStringView<T> sv = CStr::l;
    BasicString<T> s = CStr::l;

    sv.RemoveFirst();
    s.RemoveRange(0, 1);
    TEST_ENSURE(sv == s);

    sv.RemoveFirst(5);
    s.RemoveRange(0, 5);
    TEST_ENSURE(sv == s);

    sv.RemoveLast();
    s.RemoveLast();
    TEST_ENSURE(sv == s);

    sv.RemoveLast(5);
    s.RemoveLast(5);
    TEST_ENSURE(sv == s);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestOperator()
{
    BasicStringView<T> view;

    view = CStr::e;
    TEST_ENSURE(view == static_cast<T const*>(nullptr));
    TEST_ENSURE((view == CStr::e && CStr::e == view));

    view = CStr::s_10;
    TEST_ENSURE(view[9] == view.Last());

    view = CStr::l;
    auto stdstr = ToStdStringView(view);
    TEST_ENSURE(view == stdstr.data());
    TEST_ENSURE(view == BasicStringView<T>(stdstr));

    return 0;
}

int main()
{
    TEST_STRING(TestCtor, char);
    TEST_STRING(TestCtor, wchar);
    TEST_STRING(TestCtor, char8);
    TEST_STRING(TestCtor, char16);
    TEST_STRING(TestCtor, char32);

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

    TEST_STRING(TestRemove, char);
    TEST_STRING(TestRemove, wchar);
    TEST_STRING(TestRemove, char8);
    TEST_STRING(TestRemove, char16);
    TEST_STRING(TestRemove, char32);

    TEST_STRING(TestOperator, char);
    TEST_STRING(TestOperator, wchar);
    TEST_STRING(TestOperator, char8);
    TEST_STRING(TestOperator, char16);
    TEST_STRING(TestOperator, char32);

    return 0;
}