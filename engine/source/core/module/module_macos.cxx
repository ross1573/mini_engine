export module mini.core:dynamic_module;

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

} // namespace mini