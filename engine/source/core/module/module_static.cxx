export module mini.core:module_static;

import :module_system;

namespace mini {

export class StaticModuleHandle : public ModuleHandle {
private:
    ModuleInterface* m_Interface;

public:
    StaticModuleHandle(StringView) noexcept {}
    StaticModuleHandle(StringView, ModuleInterface* interface) noexcept
        : m_Interface(interface)
    {
    }

    bool IsValid() const noexcept final { return true; }
    void* GetNativeHandle() noexcept final { return nullptr; }
    ModuleInterface* GetInterface() noexcept final { return m_Interface; }
};

export template <DerivedFromT<ModuleInterface> T>
class StaticModuleInitializer {
public:
    StaticModuleInitializer(StringView name, T* unit)
    {
        g_ModuleLoader.Register(GetModule(name, unit));
    }

private:
    Module GetModule(StringView name, T* unit)
    {
        SharedPtr<StaticModuleHandle> handle = MakeShared<StaticModuleHandle>(name, unit);
        return Module(name, StaticCast<ModuleHandle>(MoveArg(handle)));
    }
};

} // namespace mini