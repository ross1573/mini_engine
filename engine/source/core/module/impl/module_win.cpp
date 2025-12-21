module;

#include <windows.h>

module mini.modules;

import mini.string;
import mini.memory;
import :module_system;
import :dynamic_module;

namespace mini {

DynamicModuleHandle::DynamicModuleHandle(StringView name)
{
    StringView prefix = MODULE_OUTPUT_PREFIX ".";
    StringView suffix = MODULE_OUTPUT_SUFFIX;

    String modulePath(prefix.Size() + name.Size() + suffix.Size());
    modulePath.Append(prefix);
    modulePath.Append(name);
    modulePath.Append(suffix);

    m_Handle = LoadLibraryA(modulePath.Data());
    if (m_Handle == nullptr) {
        return;
    }

    using StartFuncT = ModuleInterface* (*)();
    StartFuncT startFunc = (StartFuncT)LoadFunction("__start_module");
    if (startFunc != nullptr) {
        m_Interface = UniquePtr<ModuleInterface>(startFunc());
    }

    if (m_Interface != nullptr) {
        bool result = ModuleLoader::ConstructModule(m_Interface.Get());
        if (result == false) {
            FreeLibrary(m_Handle);
            m_Handle = nullptr;
            m_Interface.Reset();
        }
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
        ModuleLoader::DestructModule(m_Interface.Get());
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