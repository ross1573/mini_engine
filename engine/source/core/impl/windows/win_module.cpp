module;

#include <windows.h>

module mini.core;

import :string_view;
import :string;
import :unique_ptr;
import :module_system;
import :module_dynamic;

namespace mini {

DynamicModuleHandle::DynamicModuleHandle(StringView name)
{
    StringView prefix = LIB_PREFIX ".";
    StringView postfix = ".dll";

    String modulePath(prefix.Size() + name.Size() + postfix.Size());
    modulePath.Append(prefix);
    modulePath.Append(name);
    modulePath.Append(postfix);

    m_Handle = LoadLibraryA(modulePath.Data());
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

    FreeLibrary(m_Handle);
}

bool DynamicModuleHandle::IsValid() const noexcept
{
    return m_Handle != nullptr;
}

void* DynamicModuleHandle::GetNativeHandle() noexcept
{
    return static_cast<void*>(m_Handle);
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

    void* func = GetProcAddress(m_Handle, name.Data());
    return func;
}

} // namespace mini