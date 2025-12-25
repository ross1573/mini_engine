#include <string>

#include "string_define.h"

import mini.test;

using namespace mini;
using namespace mini::test;

[[maybe_unused]] static constexpr void StringConstraints()
{
    static_assert(sizeof(BasicString<char>) == 24);
    static_assert(sizeof(BasicString<wchar>) == 24);
    static_assert(sizeof(BasicString<char8>) == 24);
    static_assert(sizeof(BasicString<char16>) == 24);
    static_assert(sizeof(BasicString<char32>) == 24);
}

template <typename T>
static constexpr int TestSSO()
{
    using CStr = CStr_inline<T>;
    constexpr auto SSOSize = (sizeof(BasicString<T>) / sizeof(T)) - 2;

    TEST_ENSURE(BasicString<T>(CStr::ch, 4).Capacity() == SSOSize);
    TEST_ENSURE(BasicString<T>(CStr::l, 30).Capacity() == 30);
    TEST_ENSURE(BasicString<T>(CStr::ch, SSOSize - 1).Capacity() == SSOSize);

    BasicString<T> str(CStr::l);
    str.Resize(4);
    str.Shrink();
    TEST_ENSURE(str.Capacity() == SSOSize);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestCtor()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>{} == CStr::e);
    TEST_ENSURE(BasicString<T>(alloc) == CStr::e);

    TEST_ENSURE(BasicString<T>(CStr::ch, 2).Size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 2, alloc).Size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32).Size() == 32);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32, alloc).Size() == 32);

    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s)) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l)) == CStr::l);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l), alloc) == CStr::l);
    TEST_ENSURE(BasicString<T>(MakeLRef(BasicString<T>(CStr::s))) == CStr::s);
    TEST_ENSURE(BasicString<T>(MakeLRef(BasicString<T>(CStr::s)), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(MakeLRef(BasicString<T>(CStr::l))) == CStr::l);
    TEST_ENSURE(BasicString<T>(MakeLRef(BasicString<T>(CStr::l)), alloc) == CStr::l);

    TEST_ENSURE(BasicString<T>(CStr::s) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::s, alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::l) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::l, alloc) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::s, 10) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30) == CStr::l_30);
    TEST_ENSURE(BasicString<T>(CStr::s, 10, alloc) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30, alloc) == CStr::l_30);

    BasicString<T> str(CStr::s);
    BasicString<T> str2(str.Begin(), str.End());
    BasicString<T> str3(str2.Begin(), str2.End(), alloc);
    BasicString<T> str4(CStr::l);
    BasicString<T> str5(str4.Begin(), str4.End());
    BasicString<T> str6(str5.Begin(), str5.End(), alloc);

    TEST_ENSURE(str == CStr::s);
    TEST_ENSURE(str2 == str);
    TEST_ENSURE(str3 == str);
    TEST_ENSURE(str4 == CStr::l);
    TEST_ENSURE(str5 == str4);
    TEST_ENSURE(str6 == str4);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestAssign()
{
    BasicString<T> str;
    BasicString<T> str2;

    str.Assign(CStr::s);
    str2.Assign(str);
    str2.Assign(MoveArg(str));
    TEST_ENSURE(str2 == CStr::s);

    str.Assign(CStr::l);
    str2.Assign(str);
    str2.Assign(MoveArg(str));
    TEST_ENSURE(str2 == CStr::l);

    str.Assign(CStr::s, 10);
    str2.Assign(str);
    str2.Assign(MoveArg(str));
    TEST_ENSURE(str2 == CStr::s_10);

    str.Assign(CStr::l, 30);
    str2.Assign(str);
    str2.Assign(MoveArg(str));
    TEST_ENSURE(str2 == CStr::l_30);

    str = str2;
    str2.Assign(str.Begin(), str.End());
    TEST_ENSURE(str == str2);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestAppend()
{
    BasicString<T> str;
    BasicString<T> str2;
    std::basic_string<T> stdstr;

    str.Append(CStr::s);
    str2.Append(CStr::s);
    stdstr.append(CStr::s);
    TEST_ENSURE(str == CStr::s);
    TEST_ENSURE(str2 == stdstr);

    str.Clear();
    str.Append(CStr::l);
    str2.Append(CStr::l);
    stdstr.append(CStr::l);
    TEST_ENSURE(str == CStr::l);
    TEST_ENSURE(str2 == stdstr);

    str.Clear();
    str.Append(CStr::s, 10);
    str2.Append(CStr::s, 10);
    stdstr.append(CStr::s, 10);
    TEST_ENSURE(str == CStr::s_10);
    TEST_ENSURE(str2 == stdstr);

    str.Clear();
    str.Append(CStr::l, 30);
    str2.Append(CStr::l, 30);
    stdstr.append(CStr::l, 30);
    TEST_ENSURE(str == CStr::l_30);
    TEST_ENSURE(str2 == stdstr);

    str2 = CStr::s;
    str.Clear();
    str.Append(str2);
    TEST_ENSURE(str == CStr::s);

    str2 = CStr::l;
    str.Clear();
    str.Append(str2.Begin(), str2.End());
    TEST_ENSURE(str == CStr::l);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestPush()
{
    BasicString<T> str;
    std::basic_string<T> stdstr;

    str.Push(CStr::ch);
    stdstr.push_back(CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Push(CStr::ch, 5);
    stdstr.append(5, CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Push(CStr::ch, 16);
    stdstr.append(16, CStr::ch);
    TEST_ENSURE(str == stdstr);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestInsert()
{
    BasicString<T> str;
    std::basic_string<T> stdstr;

    str.Insert(0, CStr::ch);
    stdstr.insert(0, 1, CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Insert(1, CStr::ch, 2);
    stdstr.insert(1, 2, CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Insert(2, CStr::s);
    stdstr.insert(2, CStr::s);
    TEST_ENSURE(str == stdstr);

    str.Insert(3, CStr::s, 10);
    stdstr.insert(3, CStr::s, 0, 10);
    TEST_ENSURE(str == stdstr);

    str.Insert(4, str);
    stdstr.insert(4, stdstr);
    TEST_ENSURE(str == stdstr);

    str.Insert(str.Begin(), CStr::ch);
    stdstr.insert(stdstr.begin(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Insert(str.End(), CStr::ch);
    stdstr.insert(stdstr.end(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr);

    str.Insert(str.Begin() + 1, CStr::s);
    stdstr.insert(1, CStr::s);
    TEST_ENSURE(str == stdstr);

    str.Insert(str.Begin() + 2, CStr::s, 10);
    stdstr.insert(2, CStr::s, 10);
    TEST_ENSURE(str == stdstr);

    str.Insert(str.Begin() + 3, str);
    stdstr.insert(3, stdstr);
    TEST_ENSURE(str == stdstr);

    str.InsertRange(4, str.Begin() + 1, str.Begin() + 8);
    stdstr.insert(stdstr.begin() + 4, stdstr.begin() + 1, stdstr.begin() + 8);
    TEST_ENSURE(str == stdstr);

    str.InsertRange(str.Begin(), str.Begin(), str.Begin());
    stdstr.insert(stdstr.begin(), stdstr.begin(), stdstr.begin());
    TEST_ENSURE(str == stdstr);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestRemove()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.RemoveLast();
    stdstr.pop_back();
    TEST_ENSURE(str == stdstr);

    str.RemoveLast(5);
    stdstr.erase(stdstr.size() - 5, 5);
    TEST_ENSURE(str == stdstr);

    str.RemoveAt(1);
    stdstr.erase(stdstr.begin() + 1);
    TEST_ENSURE(str == stdstr);

    str.RemoveAt(str.Begin());
    stdstr.erase(stdstr.begin());
    TEST_ENSURE(str == stdstr);

    str.RemoveRange(2, 4);
    stdstr.erase(stdstr.begin() + 2, stdstr.begin() + 6);
    TEST_ENSURE(str == stdstr);

    str.RemoveRange(str.End(), 5);
    stdstr.erase(stdstr.end(), stdstr.end()); // MSVC refuses to handle it
    TEST_ENSURE(str == stdstr);

    str.RemoveRange(str.Begin(), 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr);

    str.RemoveRange(str.Begin(), str.Begin() + 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestModify()
{
    BasicString<char> str;
    BasicString<char> str2;

    str.Resize(42, 'a');
    TEST_ENSURE(str.Size() == 42 && str.Capacity() == 42);

    str.Reserve(62);
    TEST_ENSURE(str.Capacity() == 62);

    str.Shrink();
    TEST_ENSURE(str.Capacity() == 42);

    str.Clear();
    TEST_ENSURE(str.Capacity() == 42 && str.Size() == 0);

    str2.Swap(str);
    TEST_ENSURE(str2.Size() == 0 && str2.Capacity() == 42);

    return 0;
}

template <typename T, typename CStr>
static constexpr int TestOperator()
{
    BasicString<T> str;

    str = CStr::e;
    TEST_ENSURE(str == static_cast<T const*>(nullptr));
    TEST_ENSURE((str == CStr::e && CStr::e == str));

    str = CStr::s;
    TEST_ENSURE(str[10] == str.At(10));
    TEST_ENSURE((str == CStr::s && CStr::s == str));

    str = CStr::l;
    TEST_ENSURE(str[10] == str.At(10));
    TEST_ENSURE((str == CStr::l && CStr::l == str));

    str.Clear();
    str += CStr::s_10;
    TEST_ENSURE(str == CStr::s_10);

    str += CStr::s[10];
    str += CStr::s[11];
    str += CStr::s[12];
    TEST_ENSURE(str == CStr::s);

    auto stdstr = ToStdString(str);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str == BasicString<T>(stdstr));

    return 0;
}

int main()
{
    // small-buffer-optimization is only applied on runtime
    TEST_ENSURE((TestSSO<char>() == 0));
    TEST_ENSURE((TestSSO<wchar>() == 0));
    TEST_ENSURE((TestSSO<char8>() == 0));
    TEST_ENSURE((TestSSO<char16>() == 0));
    TEST_ENSURE((TestSSO<char32>() == 0));

    TEST_STRING(TestCtor, char);
    TEST_STRING(TestCtor, wchar);
    TEST_STRING(TestCtor, char8);
    TEST_STRING(TestCtor, char16);
    TEST_STRING(TestCtor, char32);

    TEST_STRING(TestAssign, char);
    TEST_STRING(TestAssign, wchar);
    TEST_STRING(TestAssign, char8);
    TEST_STRING(TestAssign, char16);
    TEST_STRING(TestAssign, char32);

    TEST_STRING(TestAppend, char);
    TEST_STRING(TestAppend, wchar);
    TEST_STRING(TestAppend, char8);
    TEST_STRING(TestAppend, char16);
    TEST_STRING(TestAppend, char32);

    TEST_STRING(TestPush, char);
    TEST_STRING(TestPush, wchar);
    TEST_STRING(TestPush, char8);
    TEST_STRING(TestPush, char16);
    TEST_STRING(TestPush, char32);

    TEST_STRING(TestInsert, char);
    TEST_STRING(TestInsert, wchar);
    TEST_STRING(TestInsert, char8);
    TEST_STRING(TestInsert, char16);
    TEST_STRING(TestInsert, char32);

    TEST_STRING(TestRemove, char);
    TEST_STRING(TestRemove, wchar);
    TEST_STRING(TestRemove, char8);
    TEST_STRING(TestRemove, char16);
    TEST_STRING(TestRemove, char32);

    TEST_STRING(TestModify, char);
    TEST_STRING(TestModify, wchar);
    TEST_STRING(TestModify, char8);
    TEST_STRING(TestModify, char16);
    TEST_STRING(TestModify, char32);

    TEST_STRING(TestOperator, char);
    TEST_STRING(TestOperator, wchar);
    TEST_STRING(TestOperator, char8);
    TEST_STRING(TestOperator, char16);
    TEST_STRING(TestOperator, char32);

    return 0;
}