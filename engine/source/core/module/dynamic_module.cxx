export module mini.core:dynamic_module;

import :string_view;
import :unique_ptr;
import :module_system;
import :module_platform;

namespace mini {

class DynamicModuleHandle final : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeModule NativeModule;

private:
    NativeModule m_NativeHandle;
    UniquePtr<ModuleInterface> m_Interface;

public:
    DynamicModuleHandle(StringView);
    DynamicModuleHandle(DynamicModuleHandle&&) noexcept;
    ~DynamicModuleHandle();

    bool IsValid() const noexcept final;

    NativeModule NativeHandle() noexcept final;
    ModuleInterface* GetInterface() const noexcept final;

    template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
    FuncT GetFunction(NativeModule, StringView);
};

template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
FuncT DynamicModuleHandle::GetFunction(NativeModule handle, StringView name)
{
    void* funcPtr = LoadFunction(handle, name);
    return reinterpret_cast<FuncT>(funcPtr);
}

} // namespace mini