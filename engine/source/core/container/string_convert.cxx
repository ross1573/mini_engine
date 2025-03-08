module;

#include <string_view>

export module mini.core:string_convert;

import convert_utf;
import :type;
import :string;
import :allocator;

export namespace mini
{

template <
    CharT T,
    AllocatorT<T> AllocT = mini::Allocator<T>
>
class StringConvert
{
public:
    typedef T ValueT;
    typedef T* PtrT;
    typedef T& RefT;
    typedef T const* ConstPtrT;
    
private:
    PtrT m_Data;
    SizeT m_Size;
    [[no_unique_address]] AllocT m_Alloc;

public:
    inline StringConvert(NullptrT)
        : m_Data(reinterpret_cast<PtrT>(&m_Size))
        , m_Size(0)
        , m_Alloc{}
    {
    }

    template <CharT U> requires (not SameAsT<T, U>)
    inline StringConvert(U* ptr)
        : m_Data(nullptr)
        , m_Size(0)
        , m_Alloc{}
    {
        if (ptr == nullptr)
        {
            m_Data = reinterpret_cast<PtrT>(&m_Size);
            return;
        }

        Initialize<U>(ptr);
    }

    template <CharT U> requires (not SameAsT<T, U>)
    inline StringConvert(std::basic_string_view<U> str)
        : m_Data(nullptr)
        , m_Size(0)
        , m_Alloc{}
    {
        Initialize<U>(str);
    }

    inline StringConvert(StringConvert&& other)
        : m_Data(other.m_Data)
        , m_Size(other.m_Size)
        , m_Alloc(MoveArg(other.m_Alloc))
    {
    }

    inline ~StringConvert()
    {
        if (m_Size != 0)
        {
            m_Alloc.Deallocate(m_Data, m_Size);
        }
    }

    inline SizeT Size() const noexcept { return m_Size; }
    inline ConstPtrT Data() const noexcept { return m_Data; }

    inline StringConvert& operator=(StringConvert&& other)
    {
        if (m_Size != 0)
        {
            m_Alloc.Deallocate(m_Data, m_Size);
        }

        m_Data = Exchange(other.m_Data, nullptr);
        m_Size = Exchange(other.m_Size, 0);
        m_Alloc = MoveArg(other.m_Alloc);
    }

private:
    StringConvert(StringConvert const&) = delete;
    StringConvert& operator=(StringConvert const&) = delete;

    template <CharT U> requires (not SameAsT<T, U>)
    inline void Initialize(std::basic_string_view<U> const& str)
    {
        if (str.size() == 0)
        {
            m_Data = reinterpret_cast<PtrT>(&m_Size);
            return;
        }

        U const* begin = str.data();
        U const* end = str.data() + str.length();
        byte isNullTerminated = str.back() == '\0' ? 0 : 1;

        m_Size = (SizeT)ConvertLength(begin, end, ValueT(0));
        m_Data = m_Alloc.Allocate(m_Size + isNullTerminated).pointer;

        ASSERT(m_Data && m_Size != 0, "failed to allocate convert buffer");
        Convert(begin, end, m_Data, m_Data + m_Size);

        if (isNullTerminated)
        {
            m_Data[m_Size] = ValueT(0);
            ++m_Size;
        }
    }
};

} // namespace mini