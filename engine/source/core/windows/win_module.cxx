module;

#include <windows.h>

export module mini.core:module_dynamic;

import :string_view;
import :unique_ptr;
import :module_system;

namespace mini {

class DynamicModuleHandle final : public ModuleHandle {
private:
    HMODULE m_Handle;
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