module;

#include <string_view>

#include "assertion.h"

export module mini.core:string_convert;

import convert_utf;
import :type;
import :string;
import :allocator;

export namespace mini {

template <CharT T, AllocatorT<T> AllocT = mini::Allocator<T>>
class StringConvert {
public:
    typedef T ValueT;
    typedef T* PtrT;
    typedef T& RefT;
    typedef T const* ConstPtrT;

private:
    PtrT m_Data;
    SizeT m_Size;
    [[no_unique_address]] AllocT m_Alloc;

    static constexpr T m_EmptyStr[1] = { '\0' };

public:
    constexpr StringConvert(NullptrT) noexcept;
    constexpr StringConvert(StringConvert&& other) noexcept;
    constexpr ~StringConvert() noexcept;

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr StringConvert(U* ptr);

    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr StringConvert(std::basic_string_view<U> str);

    constexpr SizeT Size() const noexcept;
    constexpr ConstPtrT Data() const noexcept;

    constexpr StringConvert& operator=(StringConvert&& other);
    StringConvert(StringConvert const&) = delete;
    StringConvert& operator=(StringConvert const&) = delete;

private:
    template <CharT U>
        requires(not SameAsT<T, U>)
    constexpr void Initialize(std::basic_string_view<U> const& str);
};

template <CharT T, AllocatorT<T> AllocT>
inline constexpr StringConvert<T, AllocT>::StringConvert(NullptrT) noexcept
    : m_Data(const_cast<PtrT>(m_EmptyStr))
    , m_Size(0)
    , m_Alloc{}
{
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr StringConvert<T, AllocT>::StringConvert(StringConvert&& other) noexcept
    : m_Data(other.m_Data)
    , m_Size(other.m_Size)
    , m_Alloc(MoveArg(other.m_Alloc))
{
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr StringConvert<T, AllocT>::~StringConvert() noexcept
{
    if (m_Size != 0) {
        m_Alloc.Deallocate(m_Data, m_Size);
    }
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr StringConvert<T, AllocT>::StringConvert(U* ptr)
    : m_Data(const_cast<PtrT>(m_EmptyStr))
    , m_Size(0)
    , m_Alloc{}
{
    if (ptr == nullptr) {
        return;
    }

    Initialize<U>(ptr);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr StringConvert<T, AllocT>::StringConvert(std::basic_string_view<U> str)
    : m_Data(const_cast<PtrT>(m_EmptyStr))
    , m_Size(0)
    , m_Alloc{}
{
    Initialize<U>(str);
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr SizeT StringConvert<T, AllocT>::Size() const noexcept
{
    return m_Size;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr StringConvert<T, AllocT>::ConstPtrT StringConvert<T, AllocT>::Data() const noexcept
{
    return m_Data;
}

template <CharT T, AllocatorT<T> AllocT>
inline constexpr StringConvert<T, AllocT>&
StringConvert<T, AllocT>::operator=(StringConvert&& other)
{
    if (m_Size != 0) {
        m_Alloc.Deallocate(m_Data, m_Size);
    }

    m_Data = Exchange(other.m_Data, nullptr);
    m_Size = Exchange(other.m_Size, 0);
    m_Alloc = MoveArg(other.m_Alloc);
}

template <CharT T, AllocatorT<T> AllocT>
template <CharT U>
    requires(not SameAsT<T, U>)
inline constexpr void StringConvert<T, AllocT>::Initialize(std::basic_string_view<U> const& str)
{
    if (str.size() == 0) {
        m_Data = const_cast<PtrT>(m_EmptyStr);
        return;
    }

    U const* begin = str.data();
    U const* end = str.data() + str.length();
    byte isNullTerminated = str.back() == '\0' ? 0 : 1;

    m_Size = (SizeT)ConvertLength(begin, end, ValueT(0));
    m_Data = m_Alloc.Allocate(m_Size + isNullTerminated).pointer;

    ASSERT(m_Data && m_Size != 0, "failed to allocate convert buffer");
    Convert(begin, end, m_Data, m_Data + m_Size);

    if (isNullTerminated) {
        m_Data[m_Size] = ValueT(0);
        ++m_Size;
    }
}

} // namespace mini