module mini.core;

import :utility_operation;
import :string_view;
import :unique_ptr;
import :module_system;
import :module_platform;
import :dynamic_module;

namespace mini {

DynamicModuleHandle::DynamicModuleHandle(StringView name)
{
    String path = BuildModulePath(name);
    m_NativeHandle = LoadModule(path);
    if (m_NativeHandle == nullptr) {
        return;
    }

    auto startFunc = GetFunction<Interface*>(m_NativeHandle, "__start_module");
    if (startFunc == nullptr) {
        return;
    }

    Interface* interface = startFunc();
    if (interface == nullptr) {
        return;
    }

    m_Interface = UniquePtr<Interface>(interface);
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

    UnloadModule(m_NativeHandle);
}

bool DynamicModuleHandle::IsValid() const noexcept
{
    return m_NativeHandle != nullptr;
}

DynamicModuleHandle::NativeHandle DynamicModuleHandle::GetNativeHandle() noexcept
{
    return m_NativeHandle;
}

DynamicModuleHandle::Interface* DynamicModuleHandle::GetInterface() noexcept
{
    return m_Interface.Get();
}

const DynamicModuleHandle::Interface* DynamicModuleHandle::GetInterface() const noexcept
{
    return m_Interface.Get();
}

} // namespace mini