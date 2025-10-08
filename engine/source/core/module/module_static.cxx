export module mini.core:module_static;

import :type;
import :module_system;

namespace mini {

class StaticModuleHandle : public ModuleHandle {
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

template <DerivedFromT<ModuleInterface> T,
          CallableWithReturnT<T*> FactoryT = decltype([]() { return new T(); })>
class StaticModuleInitializer {
public:
    static void Register(StringView name)
    {
        g_ModuleLoader.Register(Module(name, GetHandle(name)));
    }

private:
    static SharedPtr<ModuleHandle> GetHandle(StringView name)
    {
        SharedPtr<StaticModuleHandle> handle = MakeShared<StaticModuleHandle>(name, FactoryT{}());
        return StaticCast<ModuleHandle>(MoveArg(handle));
    }
};

} // namespace mini