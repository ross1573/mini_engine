module;

#include <windows.h>

export module mini.core:modules;

import :string_view;
import :string;
import :module_base;

namespace mini {

export class CORE_API WinModuleHandle {
private:
    HMODULE m_Handle;
    String m_Name;

public:
    WinModuleHandle(StringView);
    WinModuleHandle(WinModuleHandle&&) noexcept;
    ~WinModuleHandle();

    bool IsValid() const noexcept;
    void* LoadFunc(StringView);
    String GetName() const;
    HMODULE GetNativeHandle() const;
};

export using Module = BasicModule<WinModuleHandle>;

WinModuleHandle::WinModuleHandle(StringView name)
{
    StringView prefix = LIB_PREFIX ".";
    StringView postfix = ".dll";

    String modulePath(prefix.Size() + name.Size() + postfix.Size());
    modulePath.Append(prefix);
    modulePath.Append(name);
    modulePath.Append(postfix);

    m_Handle = LoadLibraryA(modulePath.Data());
    m_Name = m_Handle == nullptr ? StringView::empty : name;
}

WinModuleHandle::WinModuleHandle(WinModuleHandle&& other) noexcept
    : m_Handle(Exchange(other.m_Handle, nullptr))
    , m_Name(Exchange(other.m_Name, {}))
{
}

WinModuleHandle::~WinModuleHandle()
{
    if (m_Handle != nullptr) {
        FreeLibrary(m_Handle);
    }
}

bool WinModuleHandle::IsValid() const noexcept
{
    return m_Handle != nullptr;
}

void* WinModuleHandle::LoadFunc(StringView name)
{
    ENSURE(m_Handle, "module not loaded") {
        return nullptr;
    }

    void* func = GetProcAddress(m_Handle, name.Data());
    return func;
}

String WinModuleHandle::GetName() const
{
    return m_Name;
}

HMODULE WinModuleHandle::GetNativeHandle() const
{
    return m_Handle;
}

} // namespace mini