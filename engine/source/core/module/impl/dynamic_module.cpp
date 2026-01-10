module mini.core;

import :utility_operation;
import :string_view;
import :unique_ptr;
import :module_system;
import :module_platform;
import :dynamic_module;

namespace mini {

DynamicModuleHandle::DynamicModuleHandle(StringView name)
    : ModuleHandle(name)
{
    String path = BuildModulePath(name);
    m_NativeHandle = LoadModule(path);
    if (m_NativeHandle == nullptr) {
        return;
    }

    auto startFunc = GetFunction<ModuleInterface*>(m_NativeHandle, "__start_module");
    if (startFunc == nullptr) {
        return;
    }

    ModuleInterface* interface = startFunc();
    if (interface == nullptr) {
        return;
    }

    m_Interface = UniquePtr<ModuleInterface>(interface);
}

DynamicModuleHandle::DynamicModuleHandle(DynamicModuleHandle&& other) noexcept
    : m_NativeHandle(Exchange(other.m_NativeHandle, nullptr))
    , m_Interface(Exchange(other.m_Interface, nullptr))
{
}

DynamicModuleHandle::~DynamicModuleHandle()
{
    if (m_NativeHandle == nullptr) {
        return;
    }

    m_Interface.Reset();
    UnloadModule(m_NativeHandle);
}

bool DynamicModuleHandle::IsValid() const noexcept
{
    return m_NativeHandle != nullptr;
}

DynamicModuleHandle::NativeModule DynamicModuleHandle::NativeHandle() noexcept
{
    return m_NativeHandle;
}

ModuleInterface* DynamicModuleHandle::GetInterface() const noexcept
{
    return m_Interface.Get();
}

} // namespace mini