export module mini.core:dynamic_module;

import :string_view;
import :unique_ptr;
import :module_system;
import :module_platform;

namespace mini {

class DynamicModuleHandle final : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeHandle NativeHandle;
    typedef typename ModuleHandle::Interface Interface;
    typedef typename ModuleHandle::ConstInterface ConstInterface;

private:
    NativeHandle m_NativeHandle;
    UniquePtr<Interface> m_Interface;

public:
    DynamicModuleHandle(StringView);
    DynamicModuleHandle(DynamicModuleHandle&&) noexcept;
    ~DynamicModuleHandle() final;

    bool IsValid() const noexcept final;

    NativeHandle GetNativeHandle() noexcept final;
    Interface* GetInterface() noexcept final;
    ConstInterface* GetInterface() const noexcept final;

    template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
    FuncT GetFunction(NativeHandle, StringView);
};

template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
FuncT DynamicModuleHandle::GetFunction(NativeHandle handle, StringView name)
{
    void* funcPtr = LoadFunction(handle, name);
    return reinterpret_cast<FuncT>(funcPtr);
}

} // namespace mini