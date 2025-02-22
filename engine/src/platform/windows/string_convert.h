#pragma once

#include "core/string.h"
#include "memory/allocator.h"

namespace mini::windows
{

template <
    typename From,
    typename To,
    AllocatorT<To> AllocT = mini::Allocator<To>
>
class StringConverter
{
    using CharT = To;
    using StringView = std::basic_string_view<From>;

    CharT* m_Data;
    SizeT m_Size;
    [[no_unique_address]] AllocT m_Alloc;

public:
    StringConverter(From* ptr)
        : m_Data(nullptr)
        , m_Size(0)
        , m_Alloc{}
    {
        if (ptr == nullptr)
        {
            m_Data = reinterpret_cast<CharT*>(&m_Size);
            return;
        }

        Initialize(ptr);
    }

    StringConverter(NullptrT)
        : m_Data(reinterpret_cast<CharT*>(&m_Size))
        , m_Size(0)
        , m_Alloc{}
    {
    }

    StringConverter(StringView str)
        : m_Data(nullptr)
        , m_Size(0)
        , m_Alloc{}
    {
        Initialize(str);
    }

    StringConverter(StringConverter const&) = delete;

    StringConverter(StringConverter&& other)
        : m_Data(other.m_Data)
        , m_Size(other.m_Size)
        , m_Alloc(MoveArg(other.m_Alloc))
    {
    }

    ~StringConverter()
    {
        if (m_Size != 0)
        {
            m_Alloc.Deallocate(m_Data, m_Size);
        }
    }

    [[force_inline]] SizeT Size() const noexcept { return m_Size; }
    [[force_inline]] CharT const* Data() const noexcept { return m_Data; }

    StringConverter& operator=(StringConverter const&) = delete;

    StringConverter& operator=(StringConverter&& other)
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
    void Initialize(StringView const& str)
    {
        if (str.size() == 0)
        {
            m_Data = reinterpret_cast<CharT*>(&m_Size);
            return;
        }

        m_Size = memory::ConvertLength(str.data(), (int)str.size());
        m_Data = m_Alloc.Allocate(m_Size).pointer;

        ASSERT(m_Data && m_Size != 0, "failed to allocate convert buffer");
        memory::Convert(str.data(), (int)str.size(), m_Data, m_Size);
    }
};

} // namespace mini::windows