module;

#include <windows.h>

export module mini.modules:dynamic_module;

import mini.string;
import mini.memory;
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