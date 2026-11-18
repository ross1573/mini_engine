#include <string>

#include "string_define.h"

import mini.test;

using mini::Allocator;
using mini::BasicString;
using mini::BasicStringView;

inline mini::size_t stringLiveAllocations = 0;
inline mini::size_t stringLiveAllocatedBytes = 0;

inline bool noStringLiveAllocations() noexcept
{
    return stringLiveAllocations == 0 && stringLiveAllocatedBytes == 0;
}

template <typename T>
struct StringCountingAlloc {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    constexpr mini::AllocationResult<T> allocate(mini::size_t size) const noexcept
    {
        if !consteval {
            ++stringLiveAllocations;
            stringLiveAllocatedBytes += size * sizeof(T);
        }

        return Allocator<T>{}.allocate(size);
    }

    constexpr void deallocate(Pointer ptr, mini::size_t size) const noexcept
    {
        if !consteval {
            --stringLiveAllocations;
            stringLiveAllocatedBytes -= size * sizeof(T);
        }

        Allocator<T>{}.deallocate(ptr, size);
    }

    template <typename U>
    constexpr bool operator==(StringCountingAlloc<U> const&) const noexcept
    {
        return true;
    }
};

template <typename T>
struct TaggedAlloc {
    typedef T Value;
    typedef T* Pointer;
    typedef T const* ConstPointer;

    int tag = 0;

    constexpr mini::AllocationResult<T> allocate(mini::size_t size) const noexcept
    {
        return Allocator<T>{}.allocate(size);
    }

    constexpr void deallocate(Pointer ptr, mini::size_t size) const noexcept
    {
        Allocator<T>{}.deallocate(ptr, size);
    }

    template <typename U>
    constexpr bool operator==(TaggedAlloc<U> const& other) const noexcept
    {
        return tag == other.tag;
    }
};

template <typename T, typename CStr>
constexpr int testCtorDefault()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>{} == CStr::e);
    TEST_ENSURE(BasicString<T>(alloc) == CStr::e);

    BasicString<T> str;
    TEST_ENSURE(str.empty());
    TEST_ENSURE(str.data() != nullptr);
    TEST_ENSURE(str.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorCapacity()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>(static_cast<mini::size_t>(64), alloc).empty());
    TEST_ENSURE(BasicString<T>(static_cast<mini::size_t>(64), alloc).capacity() >= 64);

    BasicString<T> sized(static_cast<mini::size_t>(100));
    TEST_ENSURE(sized.empty());
    TEST_ENSURE(sized.capacity() >= 100);
    TEST_ENSURE(sized.data()[0] == T(0));

    BasicString<T> small(static_cast<mini::size_t>(1));
    TEST_ENSURE(small.empty());
    TEST_ENSURE(small.capacity() >= 1);
    TEST_ENSURE(small.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorFill()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>(CStr::ch, 2).size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 2, alloc).size() == 2);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32).size() == 32);
    TEST_ENSURE(BasicString<T>(CStr::ch, 32, alloc).size() == 32);
    TEST_ENSURE(BasicString<T>(CStr::ch, 0).empty());

    BasicString<T> str(CStr::ch, 32);
    for (T ch : str) {
        TEST_ENSURE(ch == CStr::ch);
    }
    TEST_ENSURE(str.data()[32] == T(0));

    BasicString<T> zeros(T(0), 3);
    TEST_ENSURE(zeros.size() == 3);
    TEST_ENSURE(!zeros.empty());
    TEST_ENSURE(zeros[0] == T(0) && zeros[1] == T(0) && zeros[2] == T(0));
    TEST_ENSURE(zeros.data()[3] == T(0));
    TEST_ENSURE(zeros != BasicString<T>());

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorCopy()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::s))) == CStr::s);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::s)), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::l))) == CStr::l);
    TEST_ENSURE(BasicString<T>(makeLvalueReference(BasicString<T>(CStr::l)), alloc) == CStr::l);

    BasicString<T> source(CStr::l);
    BasicString<T> copied(source);
    TEST_ENSURE(copied == source);
    TEST_ENSURE(copied.data() != source.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorMove()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s)) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::s), alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l)) == CStr::l);
    TEST_ENSURE(BasicString<T>(BasicString<T>(CStr::l), alloc) == CStr::l);

    BasicString<T> source(CStr::l);
    T const* data = source.data();
    BasicString<T> moved(mini::moveCast(source));
    TEST_ENSURE(moved == CStr::l);
    TEST_ENSURE(moved.data() == data);
    TEST_ENSURE(source.empty());
    TEST_ENSURE(source.data()[0] == T(0));

    BasicString<T> small(CStr::ch, 2);
    BasicString<T> movedSmall(mini::moveCast(small));
    TEST_ENSURE(movedSmall.size() == 2);
    TEST_ENSURE(small.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorPointer()
{
    constexpr auto alloc = Allocator<T>{};

    TEST_ENSURE(BasicString<T>(CStr::s) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::s, alloc) == CStr::s);
    TEST_ENSURE(BasicString<T>(CStr::l) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::l, alloc) == CStr::l);
    TEST_ENSURE(BasicString<T>(CStr::s, 10) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30) == CStr::l_30);
    TEST_ENSURE(BasicString<T>(CStr::s, 10, alloc) == CStr::s_10);
    TEST_ENSURE(BasicString<T>(CStr::l, 30, alloc) == CStr::l_30);
    TEST_ENSURE(BasicString<T>(CStr::e).empty());
    TEST_ENSURE(BasicString<T>(CStr::s, 0).empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorView()
{
    BasicStringView<T> longView = CStr::l;

    BasicString<T> fromView{longView};
    TEST_ENSURE(fromView == CStr::l);

    BasicString<T> fromViewSliced{longView, 5};
    TEST_ENSURE(fromViewSliced == BasicStringView<T>(CStr::l, 5));

    return 0;
}

template <typename T, typename CStr>
constexpr int testCtorIterator()
{
    constexpr auto alloc = Allocator<T>{};

    BasicString<T> str(CStr::s);
    BasicString<T> str2(str.begin(), str.end());
    BasicString<T> str3(str2.begin(), str2.end(), alloc);
    TEST_ENSURE(str2 == str);
    TEST_ENSURE(str3 == str);

    BasicString<T> str4(CStr::l);
    BasicString<T> str5(str4.begin(), str4.end());
    BasicString<T> str6(str5.begin(), str5.end(), alloc);
    TEST_ENSURE(str5 == str4);
    TEST_ENSURE(str6 == str4);

    BasicStringView<T> longView = CStr::l;
    BasicString<T> fromIterators(longView.begin() + 6, longView.begin() + 11);
    TEST_ENSURE(fromIterators == BasicStringView<T>(CStr::l + 6, 5));

    BasicString<T> fromEmptyRange(longView.begin(), longView.begin());
    TEST_ENSURE(fromEmptyRange.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssignOperator()
{
    BasicString<T> target(CStr::l);
    BasicString<T> source(CStr::s);
    target = source;
    TEST_ENSURE(target == CStr::s);
    TEST_ENSURE(source == CStr::s);

    target = BasicString<T>(CStr::l);
    TEST_ENSURE(target == CStr::l);

    target = BasicStringView<T>(CStr::s_10);
    TEST_ENSURE(target == CStr::s_10);

    target = CStr::l_30;
    TEST_ENSURE(target == CStr::l_30);

    target = CStr::e;
    TEST_ENSURE(target.empty());

    target = CStr::s;
    TEST_ENSURE((target == CStr::s && CStr::s == target));

    target = CStr::l;
    target = makeLvalueReference(target);
    TEST_ENSURE(target == CStr::l);

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssign()
{
    BasicString<T> str;
    BasicString<T> str2;

    str.assign(CStr::s);
    str2.assign(str);
    TEST_ENSURE(str2 == CStr::s);
    TEST_ENSURE(str == CStr::s);
    str2.assign(CStr::e);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::s);

    str.assign(CStr::l);
    str2.assign(str);
    TEST_ENSURE(str2 == CStr::l);
    TEST_ENSURE(str == CStr::l);
    str2.assign(CStr::e);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::l);

    str.assign(CStr::s, 10);
    str2.assign(str);
    TEST_ENSURE(str2 == CStr::s_10);
    TEST_ENSURE(str == CStr::s_10);
    str2.assign(CStr::e);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::s_10);

    str.assign(CStr::l, 30);
    str2.assign(str);
    TEST_ENSURE(str2 == CStr::l_30);
    TEST_ENSURE(str == CStr::l_30);
    str2.assign(CStr::e);
    str2.assign(moveCast(str));
    TEST_ENSURE(str2 == CStr::l_30);
    TEST_ENSURE(str2.data()[str2.size()] == T(0));

    str = str2;
    str2.assign(str.begin(), str.end());
    TEST_ENSURE(str == str2);

    str.assign(CStr::e);
    TEST_ENSURE(str.empty());

    str.assign(CStr::l, 0);
    TEST_ENSURE(str.empty());

    str.assign(CStr::s);
    str.assign(str);
    TEST_ENSURE(str == CStr::s);

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

    BasicString<T> self(CStr::s);
    std::basic_string<T> stdself(CStr::s);
    self.append(self);
    stdself.append(stdself);
    TEST_ENSURE(self == stdself.data());

    self.append(CStr::e);
    self.append(CStr::l, 0);
    TEST_ENSURE(self == stdself.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testPushBack()
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
    TEST_ENSURE(str.data()[str.size()] == T(0));

    BasicString<T> empty;
    empty.pushBack(T(0));
    TEST_ENSURE(empty.empty());

    empty.pushBack(T(0), 5);
    TEST_ENSURE(empty.empty());

    empty.pushBack(CStr::ch, 0);
    TEST_ENSURE(empty.empty());

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

    str.insert(str.size(), CStr::l);
    stdstr.insert(stdstr.size(), CStr::l);
    TEST_ENSURE(str == stdstr.data());

    BasicString<T> empty;
    empty.insert(0, CStr::ch, 0);
    TEST_ENSURE(empty.empty());

    empty.insert(0, CStr::e);
    TEST_ENSURE(empty.empty());

    empty.insert(0, CStr::s, 0);
    TEST_ENSURE(empty.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertMiddle()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    str.insert(3, CStr::ch);
    stdstr.insert(3, 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(5, CStr::ch, 3);
    stdstr.insert(5, 3, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(2, CStr::ch, 0);
    TEST_ENSURE(str == stdstr.data());

    str.insert(1, CStr::ch, 40);
    stdstr.insert(1, 40, CStr::ch);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertIterator()
{
    BasicString<T> str(CStr::s_10);
    std::basic_string<T> stdstr(CStr::s_10);

    str.insert(str.begin(), CStr::ch);
    stdstr.insert(stdstr.begin(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), CStr::ch);
    stdstr.insert(stdstr.end(), 1, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin() + 1, CStr::ch, 20);
    stdstr.insert(1, 20, CStr::ch);
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

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertIteratorEnd()
{
    BasicString<T> str(CStr::s_10);
    std::basic_string<T> stdstr(CStr::s_10);

    str.insert(str.end(), CStr::s);
    stdstr.append(CStr::s);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), CStr::s, 4);
    stdstr.append(CStr::s, 4);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), CStr::ch, 3);
    stdstr.append(3, CStr::ch);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), CStr::ch, 0);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertRange()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.insertRange(4, str.begin() + 1, str.begin() + 8);
    stdstr.insert(stdstr.begin() + 4, stdstr.begin() + 1, stdstr.begin() + 8);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(str.begin() + 2, str.begin() + 10, str.begin() + 20);
    stdstr.insert(stdstr.begin() + 2, stdstr.begin() + 10, stdstr.begin() + 20);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(str.begin(), str.begin(), str.begin());
    stdstr.insert(stdstr.begin(), stdstr.begin(), stdstr.begin());
    TEST_ENSURE(str == stdstr.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testPopBack()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.popBack();
    stdstr.pop_back();
    TEST_ENSURE(str == stdstr.data());

    str.popBack(5);
    stdstr.erase(stdstr.size() - 5, 5);
    TEST_ENSURE(str == stdstr.data());

    str.popBack(str.size());
    TEST_ENSURE(str.empty());
    TEST_ENSURE(str.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemove()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.remove(1);
    stdstr.erase(stdstr.begin() + 1);
    TEST_ENSURE(str == stdstr.data());

    str.remove(str.begin());
    stdstr.erase(stdstr.begin());
    TEST_ENSURE(str == stdstr.data());

    str.remove(str.end() - 1);
    stdstr.erase(stdstr.end() - 1);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemoveRange()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.removeRange(2, 4);
    stdstr.erase(stdstr.begin() + 2, stdstr.begin() + 6);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.end(), 5);
    stdstr.erase(stdstr.end(), stdstr.end());
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.begin(), 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.begin(), str.begin() + 5);
    stdstr.erase(stdstr.begin(), stdstr.begin() + 5);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.begin(), str.begin());
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(0, str.size());
    TEST_ENSURE(str.empty());
    TEST_ENSURE(str.data()[0] == T(0));

    str.assign(CStr::l);
    str.removeRange(str.begin(), str.end());
    TEST_ENSURE(str.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testResize()
{
    BasicString<T> str;

    str.resize(42, CStr::ch);
    TEST_ENSURE(str.size() == 42 && str.capacity() == 42);
    TEST_ENSURE(str[0] == CStr::ch && str[41] == CStr::ch);
    TEST_ENSURE(str.data()[42] == T(0));

    str.resize(42);
    TEST_ENSURE(str.size() == 42);

    str.resize(20);
    TEST_ENSURE(str.size() == 20);
    TEST_ENSURE(str.data()[20] == T(0));
    TEST_ENSURE(str == BasicString<T>(CStr::ch, 20));

    str.resize(25, T('b'));
    TEST_ENSURE(str.size() == 25);
    TEST_ENSURE(str[19] == CStr::ch && str[20] == T('b') && str[24] == T('b'));
    TEST_ENSURE(str.data()[25] == T(0));

    str.resize(0);
    TEST_ENSURE(str.empty());
    TEST_ENSURE(str.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testResizeGrow()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);
    str.shrink();

    str.resize(str.size() * 3, CStr::ch);
    stdstr.resize(stdstr.size() * 3, CStr::ch);
    TEST_ENSURE(str.size() == stdstr.size());
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testReserve()
{
    BasicString<T> str(CStr::ch, 42);
    str.shrink();

    str.reserve(62);
    TEST_ENSURE(str.capacity() == 62);
    TEST_ENSURE(str.size() == 42);
    TEST_ENSURE(str == BasicString<T>(CStr::ch, 42));

    str.reserve(10);
    TEST_ENSURE(str.capacity() == 62);
    TEST_ENSURE(str.size() == 42);

    return 0;
}

template <typename T, typename CStr>
constexpr int testShrink()
{
    BasicString<T> str(CStr::ch, 42);
    str.reserve(62);

    str.shrink();
    TEST_ENSURE(str.capacity() == 42);
    TEST_ENSURE(str == BasicString<T>(CStr::ch, 42));

    return 0;
}

template <typename T, typename CStr>
constexpr int testClear()
{
    BasicString<T> str(CStr::ch, 42);

    str.clear();
    TEST_ENSURE(str.capacity() == 42 && str.empty());
    TEST_ENSURE(str.data()[0] == T(0));

    str.clear();
    TEST_ENSURE(str.empty());

    return 0;
}

template <typename T, typename CStr>
constexpr int testSwap()
{
    BasicString<T> str;
    str.resize(42, CStr::ch);
    str.clear();

    BasicString<T> str2;
    str2.swap(str);
    TEST_ENSURE(str2.empty() && str2.capacity() == 42);

    BasicString<T> a(CStr::s);
    BasicString<T> b(CStr::l);
    a.swap(b);
    TEST_ENSURE(a == CStr::l);
    TEST_ENSURE(b == CStr::s);
    a.swap(b);
    TEST_ENSURE(a == CStr::s);
    TEST_ENSURE(b == CStr::l);

    swap(a, b);
    TEST_ENSURE(a == CStr::l);
    TEST_ENSURE(b == CStr::s);

    return 0;
}

template <typename T, typename CStr>
constexpr int testFirstLast()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    TEST_ENSURE(str.first() == stdstr.front());
    TEST_ENSURE(str.last() == stdstr.back());

    str.first() = T('J');
    str.last() = T('?');
    TEST_ENSURE(str[0] == T('J'));
    TEST_ENSURE(str[str.size() - 1] == T('?'));

    BasicString<T> const& cref = str;
    TEST_ENSURE(cref.first() == T('J'));
    TEST_ENSURE(cref.last() == T('?'));

    return 0;
}

template <typename T, typename CStr>
constexpr int testIndex()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    for (mini::size_t i = 0; i < stdstr.size(); ++i) {
        TEST_ENSURE(str[i] == stdstr[i]);
        TEST_ENSURE(str.at(i) == stdstr[i]);
    }

    BasicString<T> large(CStr::l);
    std::basic_string<T> stdlarge(CStr::l);
    for (mini::size_t i = 0; i < stdlarge.size(); ++i) {
        TEST_ENSURE(large.at(i) == stdlarge[i]);
    }

    str[1] = T('a');
    str.at(2) = T('L');
    stdstr[1] = T('a');
    stdstr[2] = T('L');
    TEST_ENSURE(str == stdstr.data());

    BasicString<T> const& cref = str;
    TEST_ENSURE(cref[1] == T('a'));
    TEST_ENSURE(cref.at(2) == T('L'));

    return 0;
}

template <typename T, typename CStr>
constexpr int testData()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(str.data()[str.size()] == T(0));
    TEST_ENSURE(BasicStringView<T>(str.data()) == CStr::s);

    str.data()[0] = T('J');
    TEST_ENSURE(str.first() == T('J'));

    BasicString<T> const& cref = str;
    TEST_ENSURE(cref.data() == str.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidIndex()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(str.validIndex(0));
    TEST_ENSURE(str.validIndex(str.size() - 1));
    TEST_ENSURE(!str.validIndex(str.size()));
    TEST_ENSURE(!BasicString<T>().validIndex(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidIterator()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(str.validIterator(str.begin()));
    TEST_ENSURE(str.validIterator(str.end() - 1));
    TEST_ENSURE(!str.validIterator(str.end()));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidRange()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(str.validRange(str.begin(), str.end()));
    TEST_ENSURE(str.validRange(str.begin() + 1, str.begin() + 2));
    TEST_ENSURE(str.validRange(str.end() - 1, str.end()));
    TEST_ENSURE(!str.validRange(str.end(), str.end()));

    return 0;
}

template <typename T, typename CStr>
constexpr int testConvertToView()
{
    BasicString<T> small(CStr::s);
    BasicStringView<T> smallView = small;
    TEST_ENSURE(smallView.data() == small.data());
    TEST_ENSURE(smallView.size() == small.size());
    TEST_ENSURE(smallView == CStr::s);

    BasicString<T> large(CStr::l);
    BasicStringView<T> largeView = large;
    TEST_ENSURE(largeView.data() == large.data());
    TEST_ENSURE(largeView.size() == large.size());
    TEST_ENSURE(largeView == CStr::l);

    BasicString<T> const empty;
    BasicStringView<T> emptyView = empty;
    TEST_ENSURE(emptyView.empty());
    TEST_ENSURE(emptyView.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testIterator()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    TEST_ENSURE(static_cast<mini::size_t>(str.end() - str.begin()) == str.size());

    mini::size_t count = 0;
    for (T ch : str) {
        TEST_ENSURE(ch == stdstr[count]);
        ++count;
    }
    TEST_ENSURE(count == stdstr.size());

    for (T& ch : str) {
        ch = CStr::ch;
    }
    TEST_ENSURE(str == BasicString<T>(CStr::ch, stdstr.size()));

    BasicString<T> const& cref = str;
    TEST_ENSURE(static_cast<mini::size_t>(cref.end() - cref.begin()) == cref.size());
    for (T ch : cref) {
        TEST_ENSURE(ch == CStr::ch);
    }

    return 0;
}

template <typename T, typename CStr>
constexpr int testAdd()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    BasicString<T> added = str + CStr::ch;
    TEST_ENSURE(added == (stdstr + CStr::ch).data());
    TEST_ENSURE(str == CStr::s);

    BasicString<T> addedView = str + CStr::l;
    TEST_ENSURE(addedView == (stdstr + CStr::l).data());

    BasicString<T> addedString = str + str;
    TEST_ENSURE(addedString == (stdstr + stdstr).data());

    BasicString<T> addedMoved = BasicString<T>(CStr::s) + CStr::l;
    TEST_ENSURE(addedMoved == (stdstr + CStr::l).data());

    BasicString<T> addedMovedChar = BasicString<T>(CStr::l) + CStr::ch;
    TEST_ENSURE(addedMovedChar == (std::basic_string<T>(CStr::l) + CStr::ch).data());

    BasicString<T> chained = BasicString<T>(CStr::e) + CStr::s_10 + CStr::ch + BasicStringView<T>(CStr::l_30);
    TEST_ENSURE(chained == (std::basic_string<T>(CStr::s_10) + CStr::ch + CStr::l_30).data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testAddAssign()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    str += BasicString<T>(CStr::e);
    TEST_ENSURE(str == CStr::s);

    str += str;
    stdstr += stdstr;
    TEST_ENSURE(str == stdstr.data());

    BasicString<T> built;
    built += CStr::s_10;
    TEST_ENSURE(built == CStr::s_10);

    built += CStr::s[10];
    built += CStr::s[11];
    built += CStr::s[12];
    TEST_ENSURE(built == CStr::s);

    BasicString<T> reused(CStr::l);
    reused.clear();
    reused += CStr::s_10;
    reused += CStr::s[10];
    reused += BasicString<T>(CStr::l);
    std::basic_string<T> stdreused(CStr::s_10);
    stdreused += CStr::s[10];
    stdreused += CStr::l;
    TEST_ENSURE(reused == stdreused.data());
    TEST_ENSURE(reused.data()[reused.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testEqual()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(BasicString<T>(CStr::e) == static_cast<T const*>(nullptr));
    TEST_ENSURE(BasicString<T>(CStr::e) == CStr::e);
    TEST_ENSURE(str == CStr::s);

    TEST_ENSURE(str == BasicString<T>(CStr::s));
    TEST_ENSURE(str != BasicString<T>(CStr::l));
    TEST_ENSURE(str != BasicString<T>(CStr::s_10));
    TEST_ENSURE(str != BasicString<T>(CStr::e));
    TEST_ENSURE(str == BasicStringView<T>(CStr::s));
    TEST_ENSURE(BasicStringView<T>(CStr::s) == str);
    TEST_ENSURE(str == BasicStringView<T>(CStr::l, str.size()));
    TEST_ENSURE(str != CStr::l);
    TEST_ENSURE(str == str);

    BasicString<T> large(CStr::l);
    TEST_ENSURE(large == CStr::l);
    TEST_ENSURE(large != CStr::s);

    large = CStr::e;
    TEST_ENSURE(large == static_cast<T const*>(nullptr));
    TEST_ENSURE(large == CStr::e);

    BasicString<T> embedded(CStr::ch, 3);
    BasicString<T> differentTail(CStr::ch, 3);
    embedded[1] = T(0);
    differentTail[1] = T(0);
    differentTail[2] = CStr::s[0];
    TEST_ENSURE(embedded != differentTail);

    BasicString<T> other(CStr::s);
    other.last() = CStr::ch;
    TEST_ENSURE(str != other);

    TEST_ENSURE(BasicString<T>() == BasicString<T>(CStr::e));

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertSelfInPlace()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.reserve(str.size() * 3);
    T const* data = str.data();

    str.insert(3, str);
    std::basic_string<T> stdcopy(stdstr);
    stdstr.insert(3, stdcopy);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data() == data);

    str.insert(str.begin() + 1, BasicStringView<T>(str).subFront(5));
    stdstr.insert(1, stdstr.substr(0, 5));
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data() == data);

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertSelfSSO()
{
    BasicString<T> str(CStr::s, 2);
    std::basic_string<T> stdstr(CStr::s, 2);
    T const* data = str.data();

    str.insert(1, str);
    std::basic_string<T> stdcopy(stdstr);
    stdstr.insert(1, stdcopy);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));
    if !consteval {
        TEST_ENSURE(str.data() == data);
    }

    BasicString<T> sub(CStr::s, 3);
    std::basic_string<T> stdsub(CStr::s, 3);
    data = sub.data();

    sub.insert(sub.begin() + 1, BasicStringView<T>(sub).subBack(1));
    stdsub.insert(1, stdsub.substr(2, 1));
    TEST_ENSURE(sub == stdsub.data());
    TEST_ENSURE(sub.data()[sub.size()] == T(0));
    if !consteval {
        TEST_ENSURE(sub.data() == data);
    }

    BasicString<T> front(CStr::s, 2);
    std::basic_string<T> stdfront(CStr::s, 2);
    front.insert(0, BasicStringView<T>(front).subFront(1));
    stdfront.insert(0, stdfront.substr(0, 1));
    TEST_ENSURE(front == stdfront.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testAppendRangeSelf()
{
    BasicString<T> source(CStr::l);

    BasicString<T> small(CStr::s, 2);
    std::basic_string<T> stdsmall(CStr::s, 2);
    small.append(source.begin(), source.end());
    stdsmall.append(CStr::l);
    TEST_ENSURE(small == stdsmall.data());
    TEST_ENSURE(small.data()[small.size()] == T(0));

    BasicString<T> selfSmall(CStr::s, 2);
    std::basic_string<T> stdselfSmall(CStr::s, 2);
    selfSmall.append(selfSmall.begin(), selfSmall.end());
    stdselfSmall.append(std::basic_string<T>(stdselfSmall));
    TEST_ENSURE(selfSmall == stdselfSmall.data());
    TEST_ENSURE(selfSmall.data()[selfSmall.size()] == T(0));

    BasicString<T> selfGrow(CStr::s_10);
    std::basic_string<T> stdselfGrow(CStr::s_10);
    selfGrow.append(selfGrow.begin(), selfGrow.end());
    stdselfGrow.append(std::basic_string<T>(stdselfGrow));
    TEST_ENSURE(selfGrow == stdselfGrow.data());
    TEST_ENSURE(selfGrow.data()[selfGrow.size()] == T(0));

    BasicString<T> selfLarge(CStr::l);
    std::basic_string<T> stdselfLarge(CStr::l);
    selfLarge.append(selfLarge.begin(), selfLarge.end());
    stdselfLarge.append(std::basic_string<T>(stdselfLarge));
    TEST_ENSURE(selfLarge == stdselfLarge.data());
    TEST_ENSURE(selfLarge.data()[selfLarge.size()] == T(0));

    BasicString<T> selfReserved(CStr::l);
    std::basic_string<T> stdselfReserved(CStr::l);
    selfReserved.reserve(selfReserved.size() * 3);
    T const* data = selfReserved.data();
    selfReserved.append(selfReserved.begin() + 1, selfReserved.end());
    stdselfReserved.append(stdselfReserved.substr(1));
    TEST_ENSURE(selfReserved == stdselfReserved.data());
    TEST_ENSURE(selfReserved.data()[selfReserved.size()] == T(0));
    TEST_ENSURE(selfReserved.data() == data);

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssignSelfView()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.assign(BasicStringView<T>(str).subBack(30));
    stdstr = stdstr.substr(stdstr.size() - 30);
    TEST_ENSURE(str.size() == stdstr.size());
    TEST_ENSURE(str == stdstr.data());

    str.assign(BasicStringView<T>(str).subFront(20));
    stdstr = stdstr.substr(0, 20);
    TEST_ENSURE(str == stdstr.data());

    BasicString<T> small(CStr::ch, 3);
    small[0] = CStr::s[0];
    std::basic_string<T> stdsmall(small.data());
    small.assign(BasicStringView<T>(small).subBack(2));
    stdsmall = stdsmall.substr(1);
    TEST_ENSURE(small == stdsmall.data());
    TEST_ENSURE(small.data()[small.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testMoveAssignSelf()
{
    BasicString<T> const smallExpected(CStr::s, 2);
    BasicString<T> small(CStr::s, 2);
    small = makeRvalueReference(small);
    TEST_ENSURE(small == smallExpected);
    TEST_ENSURE(small.data()[small.size()] == T(0));
    small.assign(makeRvalueReference(small));
    TEST_ENSURE(small == smallExpected);
    TEST_ENSURE(small.data()[small.size()] == T(0));
    small = CStr::s;
    TEST_ENSURE(small == CStr::s);

    BasicString<T> large(CStr::l);
    T const* data = large.data();
    large = makeRvalueReference(large);
    TEST_ENSURE(large == CStr::l);
    TEST_ENSURE(large.data() == data);
    TEST_ENSURE(large.data()[large.size()] == T(0));
    large.assign(makeRvalueReference(large));
    TEST_ENSURE(large == CStr::l);
    TEST_ENSURE(large.data() == data);
    TEST_ENSURE(large.data()[large.size()] == T(0));
    large.append(CStr::l);
    TEST_ENSURE(large.data()[large.size()] == T(0));
    large = CStr::l;
    TEST_ENSURE(large == CStr::l);

    BasicString<T> empty;
    empty = makeRvalueReference(empty);
    TEST_ENSURE(empty.empty());
    TEST_ENSURE(empty.data()[0] == T(0));
    empty.assign(makeRvalueReference(empty));
    TEST_ENSURE(empty.empty());
    empty = CStr::l;
    TEST_ENSURE(empty == CStr::l);

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertRangeEdge()
{
    BasicString<T> source(CStr::l);
    BasicString<T> str(CStr::s_10);
    std::basic_string<T> stdstr(CStr::s_10);

    str.insertRange(str.size(), source.begin(), source.begin() + 5);
    stdstr.append(CStr::l, 5);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(2, source.begin() + 3, source.begin() + 3);
    TEST_ENSURE(str == stdstr.data());

    str.insertRange(str.end(), source.begin(), source.begin() + 2);
    stdstr.append(CStr::l, 2);
    TEST_ENSURE(str == stdstr.data());

    BasicString<T> small(CStr::ch, 2);
    std::basic_string<T> stdsmall(2, CStr::ch);
    small.insertRange(1, source.begin(), source.end());
    stdsmall.insert(1, CStr::l);
    TEST_ENSURE(small == stdsmall.data());
    TEST_ENSURE(small.data()[small.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testAppendRangeGrowth()
{
    BasicString<T> source(CStr::l);
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);
    str.shrink();

    str.append(source.begin(), source.end());
    stdstr.append(CStr::l);
    TEST_ENSURE(str.size() == stdstr.size());
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    str.append(source.begin(), source.begin() + 4);
    stdstr.append(CStr::l, 4);
    TEST_ENSURE(str.size() == stdstr.size());
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testPopBackEdge()
{
    BasicString<T> empty;
    empty.popBack();
    TEST_ENSURE(empty.empty());
    empty.popBack(3);
    TEST_ENSURE(empty.empty());

    BasicString<T> str(CStr::s);
    str.popBack(str.size() + 10);
    TEST_ENSURE(str.empty());
    TEST_ENSURE(str.data()[0] == T(0));

    str.append(CStr::s_10);
    TEST_ENSURE(str == CStr::s_10);

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemoveEdge()
{
    BasicString<T> str(CStr::l);
    std::basic_string<T> stdstr(CStr::l);

    str.remove(str.size());
    TEST_ENSURE(str == stdstr.data());

    str.remove(str.end());
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.size(), 3);
    TEST_ENSURE(str == stdstr.data());

    str.removeRange(str.end() - 4, 100);
    stdstr.erase(stdstr.size() - 4);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testReserveSmall()
{
    BasicString<T> str(CStr::ch, 2);

    str.reserve(100);
    TEST_ENSURE(str.capacity() >= 100);
    TEST_ENSURE(str.size() == 2);
    TEST_ENSURE(str[0] == CStr::ch && str[1] == CStr::ch);
    TEST_ENSURE(str.data()[2] == T(0));

    BasicString<T> empty;
    empty.reserve(64);
    TEST_ENSURE(empty.capacity() >= 64);
    TEST_ENSURE(empty.empty());
    TEST_ENSURE(empty.data()[0] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testAssignExactCapacity()
{
    BasicString<T> longer(CStr::l);
    longer.pushBack(CStr::ch);

    BasicString<T> str(CStr::l);
    str.assign(longer);
    TEST_ENSURE(str == longer);
    TEST_ENSURE(str.size() <= str.capacity());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    BasicString<T> view(CStr::l);
    view.assign(BasicStringView<T>(longer));
    TEST_ENSURE(view == longer);
    TEST_ENSURE(view.size() <= view.capacity());
    TEST_ENSURE(view.data()[view.size()] == T(0));

    BasicString<T> op(CStr::l);
    op = longer;
    TEST_ENSURE(op == longer);
    TEST_ENSURE(op.size() <= op.capacity());

    return 0;
}

template <typename T, typename CStr>
constexpr int testSlicedLengthClamp()
{
    BasicStringView<T> view(CStr::l, 10);

    BasicString<T> assigned;
    assigned.assign(view, 20);
    TEST_ENSURE(assigned.size() == 10);
    TEST_ENSURE(assigned == CStr::s_10);

    BasicString<T> appended(CStr::s_10);
    appended.append(view, 20);
    TEST_ENSURE(appended.size() == 20);
    TEST_ENSURE(appended == (BasicString<T>(CStr::s_10) + CStr::s_10));

    BasicString<T> inserted(CStr::s);
    inserted.insert(1, view, 20);
    std::basic_string<T> stdinserted(CStr::s);
    stdinserted.insert(1, CStr::s_10);
    TEST_ENSURE(inserted == stdinserted.data());

    BasicString<T> insertedIter(CStr::s);
    insertedIter.insert(insertedIter.begin() + 1, view, 20);
    TEST_ENSURE(insertedIter == stdinserted.data());

    BasicString<T> constructed(view, 20);
    TEST_ENSURE(constructed.size() == 10);
    TEST_ENSURE(constructed == CStr::s_10);

    return 0;
}

template <typename T, typename CStr>
constexpr int testInsertWithLength()
{
    BasicString<T> str(CStr::s);
    std::basic_string<T> stdstr(CStr::s);

    str.insert(str.begin() + 3, CStr::l, 25);
    stdstr.insert(3, CStr::l, 25);
    TEST_ENSURE(str == stdstr.data());
    TEST_ENSURE(str.data()[str.size()] == T(0));

    str.insert(2, BasicString<T>(CStr::l), 7);
    stdstr.insert(2, CStr::l, 7);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.end(), BasicStringView<T>(CStr::l), 4);
    stdstr.append(CStr::l, 4);
    TEST_ENSURE(str == stdstr.data());

    str.insert(str.begin() + 1, CStr::l, 0);
    str.insert(1, BasicStringView<T>(CStr::l), 0);
    TEST_ENSURE(str == stdstr.data());

    return 0;
}

template <typename T, typename CStr>
constexpr int testRemoveRangeHugeCount()
{
    BasicString<T> str(CStr::l);
    str.reserve(100);
    str.removeRange(2, static_cast<mini::size_t>(-1));
    TEST_ENSURE(str.size() == 2);
    TEST_ENSURE(str == BasicStringView<T>(CStr::l, 2));
    TEST_ENSURE(str.data()[2] == T(0));

    BasicString<T> iter(CStr::l);
    iter.reserve(100);
    iter.removeRange(iter.begin() + 3, static_cast<mini::size_t>(-1));
    TEST_ENSURE(iter.size() == 3);
    TEST_ENSURE(iter == BasicStringView<T>(CStr::l, 3));
    TEST_ENSURE(iter.data()[3] == T(0));

    return 0;
}

template <typename T, typename CStr>
constexpr int testValidRangeOrder()
{
    BasicString<T> str(CStr::s);

    TEST_ENSURE(!str.validRange(str.begin() + 5, str.begin() + 2));
    TEST_ENSURE(!str.validRange(str.end() - 1, str.begin()));
    TEST_ENSURE(str.validRange(str.begin() + 2, str.begin() + 2));

    return 0;
}

template <typename T>
int testSSOBoundary()
{
    using CStr = CStr_inline<T>;
    constexpr mini::size_t ssoSize = (sizeof(BasicString<T>) / sizeof(T)) - 2;

    BasicString<T> exact(CStr::ch, ssoSize);
    TEST_ENSURE(exact.size() == ssoSize);
    TEST_ENSURE(exact.capacity() == ssoSize);
    TEST_ENSURE(exact.data()[ssoSize] == T(0));
    TEST_ENSURE(exact == std::basic_string<T>(ssoSize, CStr::ch).data());

    BasicString<T> over(CStr::ch, ssoSize + 1);
    TEST_ENSURE(over.size() == ssoSize + 1);
    TEST_ENSURE(over.capacity() == ssoSize + 1);
    TEST_ENSURE(over.data()[ssoSize + 1] == T(0));
    TEST_ENSURE(over == std::basic_string<T>(ssoSize + 1, CStr::ch).data());

    BasicString<T> filled(CStr::ch, ssoSize - 1);
    filled.pushBack(CStr::ch);
    TEST_ENSURE(filled.capacity() == ssoSize);
    TEST_ENSURE(filled == exact);

    std::basic_string<T> stdfull(ssoSize, CStr::ch);

    BasicString<T> pushed(exact);
    pushed.pushBack(CStr::s[0]);
    TEST_ENSURE(pushed.capacity() > ssoSize);
    TEST_ENSURE(pushed == (stdfull + CStr::s[0]).data());
    TEST_ENSURE(pushed.data()[pushed.size()] == T(0));

    BasicString<T> appended(exact);
    appended.append(CStr::s, 1);
    TEST_ENSURE(appended.capacity() > ssoSize);
    TEST_ENSURE(appended == (stdfull + CStr::s[0]).data());
    TEST_ENSURE(appended.data()[appended.size()] == T(0));

    BasicString<T> insertedFront(exact);
    insertedFront.insert(0, CStr::s, 1);
    TEST_ENSURE(insertedFront.capacity() > ssoSize);
    TEST_ENSURE(insertedFront == (CStr::s[0] + stdfull).data());
    TEST_ENSURE(insertedFront.data()[insertedFront.size()] == T(0));

    BasicString<T> insertedCount(exact);
    std::basic_string<T> stdcount(stdfull);
    insertedCount.insert(1, CStr::s[0], 3);
    stdcount.insert(1, 3, CStr::s[0]);
    TEST_ENSURE(insertedCount == stdcount.data());
    TEST_ENSURE(insertedCount.data()[insertedCount.size()] == T(0));

    BasicString<T> insertedSelf(exact);
    insertedSelf[0] = CStr::s[0];
    std::basic_string<T> stdself(insertedSelf.data());
    insertedSelf.insert(1, insertedSelf);
    stdself.insert(1, std::basic_string<T>(stdself));
    TEST_ENSURE(insertedSelf == stdself.data());
    TEST_ENSURE(insertedSelf.data()[insertedSelf.size()] == T(0));

    return 0;
}

template <typename T>
int testCountingAlloc()
{
    using CStr = CStr_inline<T>;
    using CountingString = BasicString<T, StringCountingAlloc<T>>;
    constexpr mini::size_t ssoSize = (sizeof(BasicString<T>) / sizeof(T)) - 2;

    TEST_ENSURE(noStringLiveAllocations());
    {
        CountingString small(CStr::ch, 2);
        TEST_ENSURE(stringLiveAllocations == 0);

        CountingString large(CStr::l);
        TEST_ENSURE(stringLiveAllocations == 1);

        large.resize(4);
        large.shrink();
        TEST_ENSURE(large.capacity() == ssoSize);
        TEST_ENSURE(large == CountingString(CStr::l, 4));
        TEST_ENSURE(noStringLiveAllocations());

        CountingString reserved(CStr::ch, 2);
        reserved.reserve(100);
        TEST_ENSURE(stringLiveAllocations == 1);
        reserved.shrink();
        TEST_ENSURE(reserved.capacity() == ssoSize);
        TEST_ENSURE(noStringLiveAllocations());

        reserved.reserve(100);
        reserved.append(CStr::l);
        reserved.shrink();
        TEST_ENSURE(reserved.capacity() == reserved.size());
        TEST_ENSURE(stringLiveAllocations == 1);

        CountingString grown;
        for (mini::size_t i = 0; i < 10; ++i) {
            grown.append(CStr::l);
        }
        grown.pushBack(CStr::ch, 100);
        grown.insert(5, CStr::l);
        grown.append(grown.begin(), grown.end());
        grown.insertRange(3, small.begin(), small.end());
        grown.resize(grown.size() * 2, CStr::ch);
        TEST_ENSURE(stringLiveAllocations == 2);

        CountingString copied(grown);
        CountingString moved(mini::moveCast(copied));
        copied = moved;
        moved = CStr::s;
        moved = grown;
        copied = mini::moveCast(moved);
        grown.clear();
        grown.shrink();
        TEST_ENSURE(grown.capacity() == ssoSize);
        copied.swap(grown);
        swap(copied, grown);

        moved = CStr::l;
        moved = makeRvalueReference(moved);
        moved.assign(makeRvalueReference(moved));
        TEST_ENSURE(moved.data()[moved.size()] == T(0));
    }
    TEST_ENSURE(noStringLiveAllocations());

    return 0;
}

template <typename T>
int testNoReallocWithinCapacity()
{
    using CStr = CStr_inline<T>;
    constexpr mini::size_t ssoSize = (sizeof(BasicString<T>) / sizeof(T)) - 2;

    BasicString<T> small(CStr::ch, 2);
    small.resize(ssoSize, CStr::ch);
    TEST_ENSURE(small.size() == ssoSize);
    TEST_ENSURE(small.capacity() == ssoSize);
    TEST_ENSURE(small == BasicString<T>(CStr::ch, ssoSize));

    BasicString<T> reserved(CStr::ch, 2);
    reserved.reserve(ssoSize);
    TEST_ENSURE(reserved.capacity() == ssoSize);
    TEST_ENSURE(reserved.size() == 2);

    BasicString<T> large(CStr::l);
    T const* data = large.data();
    mini::size_t capacity = large.capacity();
    large.reserve(capacity);
    TEST_ENSURE(large.data() == data);
    TEST_ENSURE(large.capacity() == capacity);

    large.resize(capacity, CStr::ch);
    TEST_ENSURE(large.data() == data);
    TEST_ENSURE(large.size() == capacity);
    TEST_ENSURE(large.data()[capacity] == T(0));

    return 0;
}

template <typename T>
int testUnequalAllocatorMove()
{
    using CStr = CStr_inline<T>;
    using TaggedString = BasicString<T, TaggedAlloc<T>>;

    TaggedString large(CStr::l, TaggedAlloc<T>{1});
    TaggedString movedWithAlloc(mini::moveCast(large), TaggedAlloc<T>{2});
    TEST_ENSURE(movedWithAlloc == CStr::l);
    TEST_ENSURE(movedWithAlloc.data()[movedWithAlloc.size()] == T(0));

    TaggedString target(CStr::s, TaggedAlloc<T>{4});
    TaggedString other(CStr::l, TaggedAlloc<T>{5});
    target = mini::moveCast(other);
    TEST_ENSURE(target == CStr::l);
    TEST_ENSURE(target.data()[target.size()] == T(0));

    TaggedString sameTarget(CStr::s, TaggedAlloc<T>{6});
    TaggedString sameSource(CStr::l, TaggedAlloc<T>{6});
    T const* data = sameSource.data();
    sameTarget = mini::moveCast(sameSource);
    TEST_ENSURE(sameTarget == CStr::l);
    TEST_ENSURE(sameTarget.data() == data);

    TaggedString small(CStr::ch, 2, TaggedAlloc<T>{7});
    TaggedString movedSmall(mini::moveCast(small), TaggedAlloc<T>{8});
    TEST_ENSURE(movedSmall == TaggedString(CStr::ch, 2));

    return 0;
}

constexpr int testCrossTypeEqual()
{
    TEST_ENSURE(BasicString<mini::char8>() == BasicString<char>());
    TEST_ENSURE(BasicString<char>("abc") == BasicString<mini::char8>(u8"abc"));
    TEST_ENSURE(BasicString<char>("abc") != BasicString<mini::char8>(u8"abd"));
    TEST_ENSURE(BasicString<char>("abc") != BasicString<mini::char8>(u8"ab"));

    return 0;
}

constexpr int testCrossTypeEqualView()
{
    TEST_ENSURE(BasicString<char>("abc") == BasicStringView<mini::char8>(u8"abc"));
    TEST_ENSURE(BasicString<char>("abc") != BasicStringView<mini::char8>(u8"abd"));
    TEST_ENSURE(BasicString<char>("abc") == u8"abc");
    TEST_ENSURE(BasicString<mini::char32>(U"abc") != "ab");

    return 0;
}

int main()
{
    TEST_STRING(testCtorDefault, char);
    TEST_STRING(testCtorDefault, mini::wchar);
    TEST_STRING(testCtorDefault, mini::char8);
    TEST_STRING(testCtorDefault, mini::char16);
    TEST_STRING(testCtorDefault, mini::char32);

    TEST_STRING(testCtorCapacity, char);
    TEST_STRING(testCtorCapacity, mini::wchar);
    TEST_STRING(testCtorCapacity, mini::char8);
    TEST_STRING(testCtorCapacity, mini::char16);
    TEST_STRING(testCtorCapacity, mini::char32);

    TEST_STRING(testCtorFill, char);
    TEST_STRING(testCtorFill, mini::wchar);
    TEST_STRING(testCtorFill, mini::char8);
    TEST_STRING(testCtorFill, mini::char16);
    TEST_STRING(testCtorFill, mini::char32);

    TEST_STRING(testCtorCopy, char);
    TEST_STRING(testCtorCopy, mini::wchar);
    TEST_STRING(testCtorCopy, mini::char8);
    TEST_STRING(testCtorCopy, mini::char16);
    TEST_STRING(testCtorCopy, mini::char32);

    TEST_STRING(testCtorMove, char);
    TEST_STRING(testCtorMove, mini::wchar);
    TEST_STRING(testCtorMove, mini::char8);
    TEST_STRING(testCtorMove, mini::char16);
    TEST_STRING(testCtorMove, mini::char32);

    TEST_STRING(testCtorPointer, char);
    TEST_STRING(testCtorPointer, mini::wchar);
    TEST_STRING(testCtorPointer, mini::char8);
    TEST_STRING(testCtorPointer, mini::char16);
    TEST_STRING(testCtorPointer, mini::char32);

    TEST_STRING(testCtorView, char);
    TEST_STRING(testCtorView, mini::wchar);
    TEST_STRING(testCtorView, mini::char8);
    TEST_STRING(testCtorView, mini::char16);
    TEST_STRING(testCtorView, mini::char32);

    TEST_STRING(testCtorIterator, char);
    TEST_STRING(testCtorIterator, mini::wchar);
    TEST_STRING(testCtorIterator, mini::char8);
    TEST_STRING(testCtorIterator, mini::char16);
    TEST_STRING(testCtorIterator, mini::char32);

    TEST_STRING(testAssignOperator, char);
    TEST_STRING(testAssignOperator, mini::wchar);
    TEST_STRING(testAssignOperator, mini::char8);
    TEST_STRING(testAssignOperator, mini::char16);
    TEST_STRING(testAssignOperator, mini::char32);

    TEST_STRING(testAssign, char);
    TEST_STRING(testAssign, mini::wchar);
    TEST_STRING(testAssign, mini::char8);
    TEST_STRING(testAssign, mini::char16);
    TEST_STRING(testAssign, mini::char32);

    TEST_STRING(testAppend, char);
    TEST_STRING(testAppend, mini::wchar);
    TEST_STRING(testAppend, mini::char8);
    TEST_STRING(testAppend, mini::char16);
    TEST_STRING(testAppend, mini::char32);

    TEST_STRING(testPushBack, char);
    TEST_STRING(testPushBack, mini::wchar);
    TEST_STRING(testPushBack, mini::char8);
    TEST_STRING(testPushBack, mini::char16);
    TEST_STRING(testPushBack, mini::char32);

    TEST_STRING(testInsert, char);
    TEST_STRING(testInsert, mini::wchar);
    TEST_STRING(testInsert, mini::char8);
    TEST_STRING(testInsert, mini::char16);
    TEST_STRING(testInsert, mini::char32);

    TEST_STRING(testInsertMiddle, char);
    TEST_STRING(testInsertMiddle, mini::wchar);
    TEST_STRING(testInsertMiddle, mini::char8);
    TEST_STRING(testInsertMiddle, mini::char16);
    TEST_STRING(testInsertMiddle, mini::char32);

    TEST_STRING(testInsertIterator, char);
    TEST_STRING(testInsertIterator, mini::wchar);
    TEST_STRING(testInsertIterator, mini::char8);
    TEST_STRING(testInsertIterator, mini::char16);
    TEST_STRING(testInsertIterator, mini::char32);

    TEST_STRING(testInsertIteratorEnd, char);
    TEST_STRING(testInsertIteratorEnd, mini::wchar);
    TEST_STRING(testInsertIteratorEnd, mini::char8);
    TEST_STRING(testInsertIteratorEnd, mini::char16);
    TEST_STRING(testInsertIteratorEnd, mini::char32);

    TEST_STRING(testInsertRange, char);
    TEST_STRING(testInsertRange, mini::wchar);
    TEST_STRING(testInsertRange, mini::char8);
    TEST_STRING(testInsertRange, mini::char16);
    TEST_STRING(testInsertRange, mini::char32);

    TEST_STRING(testPopBack, char);
    TEST_STRING(testPopBack, mini::wchar);
    TEST_STRING(testPopBack, mini::char8);
    TEST_STRING(testPopBack, mini::char16);
    TEST_STRING(testPopBack, mini::char32);

    TEST_STRING(testRemove, char);
    TEST_STRING(testRemove, mini::wchar);
    TEST_STRING(testRemove, mini::char8);
    TEST_STRING(testRemove, mini::char16);
    TEST_STRING(testRemove, mini::char32);

    TEST_STRING(testRemoveRange, char);
    TEST_STRING(testRemoveRange, mini::wchar);
    TEST_STRING(testRemoveRange, mini::char8);
    TEST_STRING(testRemoveRange, mini::char16);
    TEST_STRING(testRemoveRange, mini::char32);

    TEST_STRING(testResize, char);
    TEST_STRING(testResize, mini::wchar);
    TEST_STRING(testResize, mini::char8);
    TEST_STRING(testResize, mini::char16);
    TEST_STRING(testResize, mini::char32);

    TEST_STRING(testResizeGrow, char);
    TEST_STRING(testResizeGrow, mini::wchar);
    TEST_STRING(testResizeGrow, mini::char8);
    TEST_STRING(testResizeGrow, mini::char16);
    TEST_STRING(testResizeGrow, mini::char32);

    TEST_STRING(testReserve, char);
    TEST_STRING(testReserve, mini::wchar);
    TEST_STRING(testReserve, mini::char8);
    TEST_STRING(testReserve, mini::char16);
    TEST_STRING(testReserve, mini::char32);

    TEST_STRING(testShrink, char);
    TEST_STRING(testShrink, mini::wchar);
    TEST_STRING(testShrink, mini::char8);
    TEST_STRING(testShrink, mini::char16);
    TEST_STRING(testShrink, mini::char32);

    TEST_STRING(testClear, char);
    TEST_STRING(testClear, mini::wchar);
    TEST_STRING(testClear, mini::char8);
    TEST_STRING(testClear, mini::char16);
    TEST_STRING(testClear, mini::char32);

    TEST_STRING(testSwap, char);
    TEST_STRING(testSwap, mini::wchar);
    TEST_STRING(testSwap, mini::char8);
    TEST_STRING(testSwap, mini::char16);
    TEST_STRING(testSwap, mini::char32);

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

    TEST_STRING(testData, char);
    TEST_STRING(testData, mini::wchar);
    TEST_STRING(testData, mini::char8);
    TEST_STRING(testData, mini::char16);
    TEST_STRING(testData, mini::char32);

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

    TEST_STRING(testConvertToView, char);
    TEST_STRING(testConvertToView, mini::wchar);
    TEST_STRING(testConvertToView, mini::char8);
    TEST_STRING(testConvertToView, mini::char16);
    TEST_STRING(testConvertToView, mini::char32);

    TEST_STRING(testIterator, char);
    TEST_STRING(testIterator, mini::wchar);
    TEST_STRING(testIterator, mini::char8);
    TEST_STRING(testIterator, mini::char16);
    TEST_STRING(testIterator, mini::char32);

    TEST_STRING(testAdd, char);
    TEST_STRING(testAdd, mini::wchar);
    TEST_STRING(testAdd, mini::char8);
    TEST_STRING(testAdd, mini::char16);
    TEST_STRING(testAdd, mini::char32);

    TEST_STRING(testAddAssign, char);
    TEST_STRING(testAddAssign, mini::wchar);
    TEST_STRING(testAddAssign, mini::char8);
    TEST_STRING(testAddAssign, mini::char16);
    TEST_STRING(testAddAssign, mini::char32);

    TEST_STRING(testEqual, char);
    TEST_STRING(testEqual, mini::wchar);
    TEST_STRING(testEqual, mini::char8);
    TEST_STRING(testEqual, mini::char16);
    TEST_STRING(testEqual, mini::char32);

    TEST_STRING(testInsertSelfInPlace, char);
    TEST_STRING(testInsertSelfInPlace, mini::wchar);
    TEST_STRING(testInsertSelfInPlace, mini::char8);
    TEST_STRING(testInsertSelfInPlace, mini::char16);
    TEST_STRING(testInsertSelfInPlace, mini::char32);

    TEST_STRING(testInsertSelfSSO, char);
    TEST_STRING(testInsertSelfSSO, mini::wchar);
    TEST_STRING(testInsertSelfSSO, mini::char8);
    TEST_STRING(testInsertSelfSSO, mini::char16);
    TEST_STRING(testInsertSelfSSO, mini::char32);

    TEST_STRING(testAppendRangeSelf, char);
    TEST_STRING(testAppendRangeSelf, mini::wchar);
    TEST_STRING(testAppendRangeSelf, mini::char8);
    TEST_STRING(testAppendRangeSelf, mini::char16);
    TEST_STRING(testAppendRangeSelf, mini::char32);

    TEST_STRING(testAssignSelfView, char);
    TEST_STRING(testAssignSelfView, mini::wchar);
    TEST_STRING(testAssignSelfView, mini::char8);
    TEST_STRING(testAssignSelfView, mini::char16);
    TEST_STRING(testAssignSelfView, mini::char32);

    TEST_STRING(testMoveAssignSelf, char);
    TEST_STRING(testMoveAssignSelf, mini::wchar);
    TEST_STRING(testMoveAssignSelf, mini::char8);
    TEST_STRING(testMoveAssignSelf, mini::char16);
    TEST_STRING(testMoveAssignSelf, mini::char32);

    TEST_STRING(testInsertRangeEdge, char);
    TEST_STRING(testInsertRangeEdge, mini::wchar);
    TEST_STRING(testInsertRangeEdge, mini::char8);
    TEST_STRING(testInsertRangeEdge, mini::char16);
    TEST_STRING(testInsertRangeEdge, mini::char32);

    TEST_STRING(testAppendRangeGrowth, char);
    TEST_STRING(testAppendRangeGrowth, mini::wchar);
    TEST_STRING(testAppendRangeGrowth, mini::char8);
    TEST_STRING(testAppendRangeGrowth, mini::char16);
    TEST_STRING(testAppendRangeGrowth, mini::char32);

    TEST_STRING(testPopBackEdge, char);
    TEST_STRING(testPopBackEdge, mini::wchar);
    TEST_STRING(testPopBackEdge, mini::char8);
    TEST_STRING(testPopBackEdge, mini::char16);
    TEST_STRING(testPopBackEdge, mini::char32);

    TEST_STRING(testRemoveEdge, char);
    TEST_STRING(testRemoveEdge, mini::wchar);
    TEST_STRING(testRemoveEdge, mini::char8);
    TEST_STRING(testRemoveEdge, mini::char16);
    TEST_STRING(testRemoveEdge, mini::char32);

    TEST_STRING(testReserveSmall, char);
    TEST_STRING(testReserveSmall, mini::wchar);
    TEST_STRING(testReserveSmall, mini::char8);
    TEST_STRING(testReserveSmall, mini::char16);
    TEST_STRING(testReserveSmall, mini::char32);

    TEST_STRING(testAssignExactCapacity, char);
    TEST_STRING(testAssignExactCapacity, mini::wchar);
    TEST_STRING(testAssignExactCapacity, mini::char8);
    TEST_STRING(testAssignExactCapacity, mini::char16);
    TEST_STRING(testAssignExactCapacity, mini::char32);

    TEST_STRING(testSlicedLengthClamp, char);
    TEST_STRING(testSlicedLengthClamp, mini::wchar);
    TEST_STRING(testSlicedLengthClamp, mini::char8);
    TEST_STRING(testSlicedLengthClamp, mini::char16);
    TEST_STRING(testSlicedLengthClamp, mini::char32);

    TEST_STRING(testInsertWithLength, char);
    TEST_STRING(testInsertWithLength, mini::wchar);
    TEST_STRING(testInsertWithLength, mini::char8);
    TEST_STRING(testInsertWithLength, mini::char16);
    TEST_STRING(testInsertWithLength, mini::char32);

    TEST_STRING(testRemoveRangeHugeCount, char);
    TEST_STRING(testRemoveRangeHugeCount, mini::wchar);
    TEST_STRING(testRemoveRangeHugeCount, mini::char8);
    TEST_STRING(testRemoveRangeHugeCount, mini::char16);
    TEST_STRING(testRemoveRangeHugeCount, mini::char32);

    TEST_STRING(testValidRangeOrder, char);
    TEST_STRING(testValidRangeOrder, mini::wchar);
    TEST_STRING(testValidRangeOrder, mini::char8);
    TEST_STRING(testValidRangeOrder, mini::char16);
    TEST_STRING(testValidRangeOrder, mini::char32);

    TEST_FUNCTION_TEMPLATE(testSSOBoundary, char);
    TEST_FUNCTION_TEMPLATE(testSSOBoundary, mini::wchar);
    TEST_FUNCTION_TEMPLATE(testSSOBoundary, mini::char8);
    TEST_FUNCTION_TEMPLATE(testSSOBoundary, mini::char16);
    TEST_FUNCTION_TEMPLATE(testSSOBoundary, mini::char32);

    TEST_FUNCTION_TEMPLATE(testCountingAlloc, char);
    TEST_FUNCTION_TEMPLATE(testCountingAlloc, mini::wchar);
    TEST_FUNCTION_TEMPLATE(testCountingAlloc, mini::char8);
    TEST_FUNCTION_TEMPLATE(testCountingAlloc, mini::char16);
    TEST_FUNCTION_TEMPLATE(testCountingAlloc, mini::char32);

    TEST_FUNCTION_TEMPLATE(testNoReallocWithinCapacity, char);
    TEST_FUNCTION_TEMPLATE(testNoReallocWithinCapacity, mini::wchar);
    TEST_FUNCTION_TEMPLATE(testNoReallocWithinCapacity, mini::char8);
    TEST_FUNCTION_TEMPLATE(testNoReallocWithinCapacity, mini::char16);
    TEST_FUNCTION_TEMPLATE(testNoReallocWithinCapacity, mini::char32);

    TEST_FUNCTION_TEMPLATE(testUnequalAllocatorMove, char);
    TEST_FUNCTION_TEMPLATE(testUnequalAllocatorMove, mini::wchar);
    TEST_FUNCTION_TEMPLATE(testUnequalAllocatorMove, mini::char8);
    TEST_FUNCTION_TEMPLATE(testUnequalAllocatorMove, mini::char16);
    TEST_FUNCTION_TEMPLATE(testUnequalAllocatorMove, mini::char32);

    TEST_CONSTEXPR(testCrossTypeEqual);
    TEST_CONSTEXPR(testCrossTypeEqualView);

    return 0;
}
