module;

#include <dlfcn.h>

export module mini.core:modules;

import :string_view;
import :string;
import :module_base;

namespace mini {

export class CORE_API DarwinModuleHandle {
private:
    void* m_Handle;
    String m_Name;

public:
    DarwinModuleHandle(StringView);
    DarwinModuleHandle(DarwinModuleHandle&&) noexcept;
    ~DarwinModuleHandle();

    bool IsValid() const noexcept;
    void* LoadFunc(StringView);
    void* GetNativeHandle() const;
    String GetName() const;
};

export using Module = BasicModule<DarwinModuleHandle>;

DarwinModuleHandle::DarwinModuleHandle(StringView name)
{
    StringView prefix = "lib" LIB_PREFIX ".";
    StringView postfix = ".dylib";

    String fullPath(prefix.Size() + name.Size() + postfix.Size());
    fullPath.Append(prefix);
    fullPath.Append(name);
    fullPath.Append(postfix);

    m_Handle = dlopen(fullPath.Data(), RTLD_NOW | RTLD_LOCAL);
    m_Name = m_Handle == nullptr ? StringView::empty : name;
}

DarwinModuleHandle::DarwinModuleHandle(DarwinModuleHandle&& other) noexcept
    : m_Handle(Exchange(other.m_Handle, nullptr))
    , m_Name(Exchange(other.m_Name, {}))
{
}

DarwinModuleHandle::~DarwinModuleHandle()
{
    if (m_Handle != nullptr) {
        dlclose(m_Handle);
    }
}

bool DarwinModuleHandle::IsValid() const noexcept
{
    return m_Handle != nullptr;
}

void* DarwinModuleHandle::LoadFunc(StringView name)
{
    ENSURE(m_Handle, "module not loaded") {
        return nullptr;
    }

    void* func = dlsym(m_Handle, name.Data());
    return func;
}

String DarwinModuleHandle::GetName() const
{
    return m_Name;
}

void* DarwinModuleHandle::GetNativeHandle() const
{
    return m_Handle;
}

} // namespace mini