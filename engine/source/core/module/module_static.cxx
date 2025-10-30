export module mini.core:module_static;

import :type;
import :unique_ptr;
import :module_system;

namespace mini {

class CORE_API StaticModuleHandle : public ModuleHandle {
private:
    UniquePtr<ModuleInterface> m_Interface;

public:
    StaticModuleHandle(StringView) noexcept {}
    StaticModuleHandle(StringView, ModuleInterface* interface) noexcept
        : m_Interface(interface)
    {
    }

    virtual ~StaticModuleHandle() noexcept = default;

    bool IsValid() const noexcept final { return true; }
    void* GetNativeHandle() noexcept final { return nullptr; }
    ModuleInterface* GetInterface() noexcept final { return m_Interface.Get(); }
};

export template <CallableWithReturnT<ModuleInterface*> FactoryT>
class StaticModuleInitializer {
private:
    static_assert(NoThrowDefaultConstructibleT<FactoryT>,
                  "factory must be nothrow default constructible");

public:
    static void Register(StringView name) { g_ModuleLoader.Register(GetModule(name)); }

private:
    static Module GetModule(StringView name)
    {
        SharedPtr<StaticModuleHandle> targetHandle =
            MakeShared<StaticModuleHandle>(name, FactoryT{}());

        return Module(name, StaticCast<ModuleHandle>(MoveArg(targetHandle)));
    }
};

} // namespace mini