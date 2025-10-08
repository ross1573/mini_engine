module;

#include <dlfcn.h>

export module mini.core:module_dynamic;

import :deleter;
import :string_view;
import :string;
import :unique_ptr;
import :module_system;

namespace mini {

class DynamicModuleHandle final : public ModuleHandle {
private:
    void* m_Handle;
    UniquePtr<ModuleInterface> m_Interface;

public:
    DynamicModuleHandle(StringView);
    DynamicModuleHandle(DynamicModuleHandle&&) noexcept;
    ~DynamicModuleHandle() final;

    bool IsValid() const noexcept final;
    void* GetNativeHandle() noexcept final;
    ModuleInterface* GetInterface() noexcept final;

private:
    void* LoadFunction(StringView);
};

DynamicModuleHandle::DynamicModuleHandle(StringView name)
{
    StringView prefix = "lib" LIB_PREFIX ".";
    StringView postfix = ".dylib";

    String fullPath(prefix.Size() + name.Size() + postfix.Size());
    fullPath.Append(prefix);
    fullPath.Append(name);
    fullPath.Append(postfix);

    m_Handle = dlopen(fullPath.Data(), RTLD_NOW | RTLD_LOCAL);
    if (m_Handle == nullptr) {
        return;
    }

    using StartFuncT = ModuleInterface* (*)();
    StartFuncT startFunc = (StartFuncT)LoadFunction("__start_module");
    if (startFunc != nullptr) {
        m_Interface = UniquePtr<ModuleInterface>(startFunc());
    }
}

DynamicModuleHandle::DynamicModuleHandle(DynamicModuleHandle&& other) noexcept
    : m_Handle(Exchange(other.m_Handle, nullptr))
    , m_Interface(Exchange(other.m_Interface, nullptr))
{
}

DynamicModuleHandle::~DynamicModuleHandle()
{
    if (m_Handle == nullptr) [[unlikely]] {
        return;
    }

    if (m_Interface != nullptr) [[likely]] {
        m_Interface->Shutdown();
        m_Interface.Reset();
    }

    dlclose(m_Handle);
}

bool DynamicModuleHandle::IsValid() const noexcept
{
    return m_Handle != nullptr;
}

void* DynamicModuleHandle::GetNativeHandle() noexcept
{
    return m_Handle;
}

ModuleInterface* DynamicModuleHandle::GetInterface() noexcept
{
    return m_Interface.Get();
}

void* DynamicModuleHandle::LoadFunction(StringView name)
{
    ENSURE(m_Handle, "module not loaded") {
        return nullptr;
    }

    void* func = dlsym(m_Handle, name.Data());
    return func;
}

} // namespace mini