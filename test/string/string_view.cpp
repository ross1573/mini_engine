#include <string_view>

#include "string_define.h"

import mini.test;

using mini::BasicString;
using mini::BasicStringView;

template <typename T>
constexpr bool sameAs(BasicStringView<T> view, std::basic_string_view<T> ref)
{
    if (view.size() != ref.size() || view.empty() != ref.empty()) {
        return false;
    }
    for (mini::size_t i = 0; i < ref.size(); ++i) {
        if (view[i] != ref[i] || view.at(i) != ref[i] || view.data()[i] != ref[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, typename CStr>
constexpr int testCtorDefault()
{
    TEST_ENSURE(BasicStringView<T>{} == CStr::e);
    TEST_ENSURE(BasicStringView<T>{}.empty());
    TEST_ENSURE(BasicStringView<T>{}.data() != nullptr);
    TEST_ENSURE(BasicStringView<T>{}.data()[0] == T('\0'));

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorNull()
{
    TEST_ENSURE(BasicStringView<T>(static_cast<T const*>(nullptr)).empty());
    TEST_ENSURE(BasicStringView<T>(static_cast<T const*>(nullptr), 10).empty());
    TEST_ENSURE(BasicStringView<T>(static_cast<T const*>(nullptr)).data() != nullptr);

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorPointer()
{
    TEST_ENSURE(BasicStringView<T>(CStr::e).empty());
    TEST_ENSURE(BasicStringView<T>(CStr::s, 0).empty());

    TEST_ENSURE(BasicStringView<T>(CStr::s, 10) == CStr::s_10);
    TEST_ENSURE(BasicStringView<T>(CStr::l, 30) == CStr::l_30);

    TEST_ENSURE(BasicStringView<T>(CStr::s).data() == CStr::s);
    TEST_ENSURE(sameAs(BasicStringView<T>(CStr::s), std::basic_string_view<T>(CStr::s)));
    TEST_ENSURE(sameAs(BasicStringView<T>(CStr::l), std::basic_string_view<T>(CStr::l)));
    TEST_ENSURE(sameAs(BasicStringView<T>(CStr::l, 5), std::basic_string_view<T>(CStr::l, 5)));

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssign()
{
    BasicStringView<T> view;

    view = CStr::e;
    TEST_ENSURE(view.empty());
    TEST_ENSURE(view == static_cast<T const*>(nullptr));
    TEST_ENSURE(view == CStr::e);

    view = CStr::s_10;
    TEST_ENSURE(view.size() == 10);
    TEST_ENSURE(view == CStr::s_10);

    view = static_cast<T const*>(nullptr);
    TEST_ENSURE(view.empty());

    view = CStr::l;
    TEST_ENSURE(view.data() == CStr::l);
    TEST_ENSURE(view == CStr::l);

    return 0;
}

template <typename T, typename CStr>
constexpr int testFirstLast()
{
    BasicStringView<T> view = CStr::s;
    std::basic_string_view<T> ref = CStr::s;

    TEST_ENSURE(view.first() == ref.front());
    TEST_ENSURE(view.last() == ref.back());
    TEST_ENSURE(BasicStringView<T>(CStr::s_10).last() == T('l'));

    return 0;
}

template <typename T, typename CStr>
constexpr int testIndex()
{
    BasicStringView<T> view = CStr::s;
    std::basic_string_view<T> ref = CStr::s;

    TEST_ENSURE(view[0] == T('H'));
    TEST_ENSURE(view.at(4) == T('o'));
    TEST_ENSURE(view[view.size() - 1] == T('!'));
    TEST_ENSURE(sameAs(view, ref));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidIndex()
{
    BasicStringView<T> view = CStr::s;

    TEST_ENSURE(view.validIndex(0));
    TEST_ENSURE(view.validIndex(view.size() - 1));
    TEST_ENSURE(!view.validIndex(view.size()));
    TEST_ENSURE(!BasicStringView<T>().validIndex(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidIterator()
{
    BasicStringView<T> view = CStr::s;

    TEST_ENSURE(view.validIterator(view.begin()));
    TEST_ENSURE(view.validIterator(view.end() - 1));
    TEST_ENSURE(!view.validIterator(view.end()));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidRange()
{
    BasicStringView<T> view = CStr::s;

    TEST_ENSURE(view.validRange(view.begin(), view.end()));
    TEST_ENSURE(view.validRange(view.begin() + 1, view.begin() + 2));
    TEST_ENSURE(view.validRange(view.end() - 1, view.end()));
    TEST_ENSURE(!view.validRange(view.end(), view.end()));

    return 0;
}

template <typename T, typename CStr>
constexpr int testIterator()
{
    BasicStringView<T> view = CStr::s;
    std::basic_string_view<T> ref = CStr::s;

    TEST_ENSURE(static_cast<mini::size_t>(view.end() - view.begin()) == view.size());

    mini::size_t count = 0;
    for (T ch : view) {
        TEST_ENSURE(ch == ref[count]);
        ++count;
    }
    TEST_ENSURE(count == ref.size());

    count = 0;
    for ([[maybe_unused]] T ch : BasicStringView<T>()) {
        ++count;
    }
    TEST_ENSURE(count == 0);

    return 0;
}

template <typename T, typename CStr>
constexpr int testCopy()
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

    T buffer[4] = {T('x'), T('x'), T('x'), T('x')};
    svs.copy(buffer, svs.size() - 2, 100);
    TEST_ENSURE(buffer[0] == T('d'));
    TEST_ENSURE(buffer[1] == T('!'));
    TEST_ENSURE(buffer[2] == T('x'));

    svs.copy(buffer, 0, 0);
    TEST_ENSURE(buffer[0] == T('d'));

    svs.copy(static_cast<T*>(nullptr), 0, svs.size());
    svs.copy(static_cast<T*>(nullptr), svs.begin(), svs.end());

    return 0;
}

template <typename T, typename CStr>
constexpr int testSubFront()
{
    BasicStringView<T> sv = CStr::l;

    BasicStringView<T> front = sv.subFront(30);
    TEST_ENSURE(front == CStr::l_30);
    TEST_ENSURE(front.data() == sv.data());

    TEST_ENSURE(sv.subFront(0).empty());
    TEST_ENSURE(sv.subFront(1000) == sv);
    TEST_ENSURE(BasicStringView<T>().subFront(5).empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testSubBack()
{
    BasicStringView<T> sv = CStr::l;
    std::basic_string_view<T> ref = CStr::l;

    BasicStringView<T> back = sv.subBack(10);
    TEST_ENSURE(back == BasicString<T>(CStr::l + sv.size() - 10));
    TEST_ENSURE(sameAs(back, ref.substr(ref.size() - 10)));

    TEST_ENSURE(sv.subBack(0).empty());
    TEST_ENSURE(sv.subBack(1000) == sv);
    TEST_ENSURE(BasicStringView<T>().subBack(5).empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testSubView()
{
    BasicStringView<T> sv = CStr::l;
    std::basic_string_view<T> ref = CStr::l;

    TEST_ENSURE(sv.subView(0, 30) == CStr::l_30);
    TEST_ENSURE(sv.subView(sv.begin() + 10, sv.end()) == BasicString<T>(CStr::l + 10));

    TEST_ENSURE(sameAs(sv.subView(6, 5), ref.substr(6, 5)));
    TEST_ENSURE(sameAs(sv.subView(6, 1000), ref.substr(6, 1000)));
    TEST_ENSURE(sameAs(sv.subView(sv.size() - 1, 1), ref.substr(ref.size() - 1, 1)));
    TEST_ENSURE(sameAs(sv.subView(sv.begin() + 3, sv.begin() + 8), ref.substr(3, 5)));

    TEST_ENSURE(sv.subView(sv.begin() + 3, sv.begin() + 3).empty());
    TEST_ENSURE(sv.subView(0, 0).empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testPopFront()
{
    BasicStringView<T> sv = CStr::l;
    BasicString<T> s = CStr::l;

    sv.popFront();
    s.removeRange(0, 1);
    TEST_ENSURE(sv == s);

    sv.popFront(5);
    s.removeRange(0, 5);
    TEST_ENSURE(sv == s);

    sv.popFront(0);
    TEST_ENSURE(sv == s);

    BasicStringView<T> overflow = CStr::s;
    overflow.popFront(1000);
    TEST_ENSURE(overflow.empty());
    overflow.popFront();
    TEST_ENSURE(overflow.empty());

    BasicStringView<T> one = CStr::s;
    for (mini::size_t i = 0; i < 12; ++i) {
        one.popFront();
    }
    TEST_ENSURE(one.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testPopBack()
{
    BasicStringView<T> sv = CStr::l;
    BasicString<T> s = CStr::l;

    sv.popBack();
    s.popBack();
    TEST_ENSURE(sv == s);

    sv.popBack(5);
    s.popBack(5);
    TEST_ENSURE(sv == s);

    sv.popBack(0);
    TEST_ENSURE(sv == s);

    BasicStringView<T> overflow = CStr::s;
    overflow.popBack(1000);
    TEST_ENSURE(overflow.empty());
    TEST_ENSURE(overflow.data() == CStr::s);
    overflow.popBack();
    TEST_ENSURE(overflow.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testEqual()
{
    BasicStringView<T> view = CStr::s;

    TEST_ENSURE(BasicStringView<T>(CStr::e) == static_cast<T const*>(nullptr));
    TEST_ENSURE(BasicStringView<T>(CStr::e) == CStr::e);

    TEST_ENSURE(view == CStr::s);
    TEST_ENSURE(view != CStr::l);
    TEST_ENSURE(view != CStr::s_10);
    TEST_ENSURE(view != CStr::e);
    TEST_ENSURE(view == BasicString<T>(CStr::s));
    TEST_ENSURE(view == BasicStringView<T>(CStr::l, view.size()));
    TEST_ENSURE(view != BasicStringView<T>(CStr::l, view.size() - 1));

    T const embedded[] = {CStr::ch, T(0), CStr::ch};
    T const sameEmbedded[] = {CStr::ch, T(0), CStr::ch};
    T const differentTail[] = {CStr::ch, T(0), T('b')};
    T const shorter[] = {CStr::ch, T(0)};

    BasicStringView<T> embeddedView(embedded, 3);
    TEST_ENSURE(embeddedView == BasicStringView<T>(sameEmbedded, 3));
    TEST_ENSURE(embeddedView != BasicStringView<T>(differentTail, 3));
    TEST_ENSURE(embeddedView != BasicStringView<T>(shorter, 2));
    TEST_ENSURE(embeddedView != BasicStringView<T>(embedded));
    TEST_ENSURE(embeddedView.subFront(2) == BasicStringView<T>(differentTail, 2));

    BasicString<T> embeddedString(embeddedView);
    TEST_ENSURE(embeddedView == embeddedString);
    TEST_ENSURE(BasicStringView<T>(differentTail, 3) != embeddedString);
    TEST_ENSURE(BasicStringView<T>(shorter, 2) != embeddedString);

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidRangeOrder()
{
    BasicStringView<T> view = CStr::s;

    TEST_ENSURE(!view.validRange(view.begin() + 5, view.begin() + 2));
    TEST_ENSURE(!view.validRange(view.end() - 1, view.begin()));
    TEST_ENSURE(view.validRange(view.begin() + 2, view.begin() + 2));

    return 0;
}

template <typename T, typename CStr>
constexpr int testPopAdvancesData()
{
    BasicStringView<T> view = CStr::l;

    view.popFront(6);
    TEST_ENSURE(view.data() == CStr::l + 6);
    TEST_ENSURE(view.first() == CStr::l[6]);
    TEST_ENSURE(view.size() == BasicStringView<T>(CStr::l).size() - 6);

    view.popBack(3);
    TEST_ENSURE(view.data() == CStr::l + 6);
    TEST_ENSURE(view.last() == CStr::l[BasicStringView<T>(CStr::l).size() - 4]);

    view.popFront();
    TEST_ENSURE(view.data() == CStr::l + 7);
    TEST_ENSURE(view[0] == CStr::l[7]);

    return 0;
}

template <typename T, typename CStr>
constexpr int testNestedSubView()
{
    BasicStringView<T> view = CStr::l;
    std::basic_string_view<T> ref = CStr::l;

    BasicStringView<T> inner = view.subView(6, 20).subView(2, 5);
    TEST_ENSURE(inner.data() == CStr::l + 8);
    TEST_ENSURE(sameAs(inner, ref.substr(6, 20).substr(2, 5)));

    BasicStringView<T> fromIter = inner.subView(inner.begin() + 1, inner.end());
    TEST_ENSURE(fromIter.data() == CStr::l + 9);
    TEST_ENSURE(sameAs(fromIter, ref.substr(9, 4)));

    TEST_ENSURE(sameAs(view.subBack(10).subFront(4), ref.substr(ref.size() - 10, 4)));
    TEST_ENSURE(sameAs(view.subFront(10).subBack(4), ref.substr(6, 4)));

    T buffer[4] = {T('x'), T('x'), T('x'), T('x')};
    inner.copy(buffer, inner.begin() + 1, inner.begin() + 3);
    TEST_ENSURE(buffer[0] == CStr::l[9]);
    TEST_ENSURE(buffer[1] == CStr::l[10]);
    TEST_ENSURE(buffer[2] == T('x'));

    return 0;
}

constexpr int testCrossType()
{
    TEST_ENSURE(BasicStringView<char>("abc") == BasicStringView<mini::char8>(u8"abc"));
    TEST_ENSURE(BasicStringView<char>("abc") != BasicStringView<mini::char8>(u8"abd"));
    TEST_ENSURE(BasicStringView<char>("abc") != BasicStringView<mini::char8>(u8"ab"));
    TEST_ENSURE(BasicStringView<mini::char8>(u8"") == BasicStringView<char>(""));

    return 0;
}

constexpr int testCrossTypeStringLike()
{
    TEST_ENSURE(BasicStringView<char>("abc") == u8"abc");
    TEST_ENSURE(BasicStringView<char>("abc") != u8"abd");
    TEST_ENSURE(BasicStringView<char>("abc") == BasicString<mini::char8>(u8"abc"));
    TEST_ENSURE(BasicStringView<mini::char32>(U"abc") != BasicString<char>("ab"));

    return 0;
}

constexpr int testCrossTypeWide()
{
    TEST_ENSURE(BasicStringView<mini::wchar>(L"abc") == BasicStringView<mini::char16>(u"abc"));
    TEST_ENSURE(BasicStringView<mini::wchar>(L"abc") != BasicStringView<mini::char16>(u"abcd"));

    return 0;
}

int main()
{
    TEST_STRING(testCtorDefault, char);
    TEST_STRING(testCtorDefault, mini::wchar);
    TEST_STRING(testCtorDefault, mini::char8);
    TEST_STRING(testCtorDefault, mini::char16);
    TEST_STRING(testCtorDefault, mini::char32);

    TEST_STRING(testCtorNull, char);
    TEST_STRING(testCtorNull, mini::wchar);
    TEST_STRING(testCtorNull, mini::char8);
    TEST_STRING(testCtorNull, mini::char16);
    TEST_STRING(testCtorNull, mini::char32);

    TEST_STRING(testCtorPointer, char);
    TEST_STRING(testCtorPointer, mini::wchar);
    TEST_STRING(testCtorPointer, mini::char8);
    TEST_STRING(testCtorPointer, mini::char16);
    TEST_STRING(testCtorPointer, mini::char32);

    TEST_STRING(testAssign, char);
    TEST_STRING(testAssign, mini::wchar);
    TEST_STRING(testAssign, mini::char8);
    TEST_STRING(testAssign, mini::char16);
    TEST_STRING(testAssign, mini::char32);

    TEST_STRING(testFirstLast, char);
    TEST_STRING(testFirstLast, mini::wchar);
    TEST_STRING(testFirstLast, mini::char8);
    TEST_STRING(testFirstLast, mini::char16);
    TEST_STRING(testFirstLast, mini::char32);

    TEST_STRING(testIndex, char);
    TEST_STRING(testIndex, mini::wchar);
    TEST_STRING(testIndex, mini::char8);
    TEST_STRING(testIndex, mini::char16);
    TEST_STRING(testIndex, mini::char32);

    TEST_STRING(testValidIndex, char);
    TEST_STRING(testValidIndex, mini::wchar);
    TEST_STRING(testValidIndex, mini::char8);
    TEST_STRING(testValidIndex, mini::char16);
    TEST_STRING(testValidIndex, mini::char32);

    TEST_STRING(testValidIterator, char);
    TEST_STRING(testValidIterator, mini::wchar);
    TEST_STRING(testValidIterator, mini::char8);
    TEST_STRING(testValidIterator, mini::char16);
    TEST_STRING(testValidIterator, mini::char32);

    TEST_STRING(testValidRange, char);
    TEST_STRING(testValidRange, mini::wchar);
    TEST_STRING(testValidRange, mini::char8);
    TEST_STRING(testValidRange, mini::char16);
    TEST_STRING(testValidRange, mini::char32);

    TEST_STRING(testIterator, char);
    TEST_STRING(testIterator, mini::wchar);
    TEST_STRING(testIterator, mini::char8);
    TEST_STRING(testIterator, mini::char16);
    TEST_STRING(testIterator, mini::char32);

    TEST_STRING(testCopy, char);
    TEST_STRING(testCopy, mini::wchar);
    TEST_STRING(testCopy, mini::char8);
    TEST_STRING(testCopy, mini::char16);
    TEST_STRING(testCopy, mini::char32);

    TEST_STRING(testSubFront, char);
    TEST_STRING(testSubFront, mini::wchar);
    TEST_STRING(testSubFront, mini::char8);
    TEST_STRING(testSubFront, mini::char16);
    TEST_STRING(testSubFront, mini::char32);

    TEST_STRING(testSubBack, char);
    TEST_STRING(testSubBack, mini::wchar);
    TEST_STRING(testSubBack, mini::char8);
    TEST_STRING(testSubBack, mini::char16);
    TEST_STRING(testSubBack, mini::char32);

    TEST_STRING(testSubView, char);
    TEST_STRING(testSubView, mini::wchar);
    TEST_STRING(testSubView, mini::char8);
    TEST_STRING(testSubView, mini::char16);
    TEST_STRING(testSubView, mini::char32);

    TEST_STRING(testPopFront, char);
    TEST_STRING(testPopFront, mini::wchar);
    TEST_STRING(testPopFront, mini::char8);
    TEST_STRING(testPopFront, mini::char16);
    TEST_STRING(testPopFront, mini::char32);

    TEST_STRING(testPopBack, char);
    TEST_STRING(testPopBack, mini::wchar);
    TEST_STRING(testPopBack, mini::char8);
    TEST_STRING(testPopBack, mini::char16);
    TEST_STRING(testPopBack, mini::char32);

    TEST_STRING(testEqual, char);
    TEST_STRING(testEqual, mini::wchar);
    TEST_STRING(testEqual, mini::char8);
    TEST_STRING(testEqual, mini::char16);
    TEST_STRING(testEqual, mini::char32);

    TEST_STRING(testValidRangeOrder, char);
    TEST_STRING(testValidRangeOrder, mini::wchar);
    TEST_STRING(testValidRangeOrder, mini::char8);
    TEST_STRING(testValidRangeOrder, mini::char16);
    TEST_STRING(testValidRangeOrder, mini::char32);

    TEST_STRING(testPopAdvancesData, char);
    TEST_STRING(testPopAdvancesData, mini::wchar);
    TEST_STRING(testPopAdvancesData, mini::char8);
    TEST_STRING(testPopAdvancesData, mini::char16);
    TEST_STRING(testPopAdvancesData, mini::char32);

    TEST_STRING(testNestedSubView, char);
    TEST_STRING(testNestedSubView, mini::wchar);
    TEST_STRING(testNestedSubView, mini::char8);
    TEST_STRING(testNestedSubView, mini::char16);
    TEST_STRING(testNestedSubView, mini::char32);

    TEST_CONSTEXPR(testCrossType);
    TEST_CONSTEXPR(testCrossTypeStringLike);
    TEST_CONSTEXPR(testCrossTypeWide);

    return 0;
}
