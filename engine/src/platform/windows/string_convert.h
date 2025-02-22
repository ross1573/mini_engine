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
    StringConverter(StringView str)
        : m_Data(nullptr)
        , m_Size(0)
        , m_Alloc{}
    {
        m_Size = memory::ConvertLength(str.data(), (int)str.size());
        m_Data = m_Alloc.Allocate(m_Size).pointer;

        ASSERT(m_Data, "failed to allocate convert buffer");
        memory::Convert(str.data(), (int)str.size(), m_Data, m_Size);
    }

    ~StringConverter()
    {
        m_Alloc.Deallocate(m_Data, m_Size);
    }

    [[force_inline]] SizeT Size() const noexcept { return m_Size; }
    [[force_inline]] CharT const* Data() const noexcept { return m_Data; }
};

} // namespace mini::windows