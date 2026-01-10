export module mini.core:static_module;

import :type;
import :unique_ptr;
import :string_view;
import :module_platform;
import :module_system;
import :module_loader;

namespace mini {

class CORE_API StaticModuleHandle : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeModule NativeModule;

private:
    UniquePtr<ModuleInterface> m_Interface;

    static NativeModule m_ProgramHandle;

public:
    StaticModuleHandle(StringView name, ModuleInterface* interface) noexcept
        : ModuleHandle(name)
        , m_Interface(interface)
    {
    }

    ~StaticModuleHandle() noexcept = default;

    bool IsValid() const noexcept final { return true; }

    NativeModule NativeHandle() noexcept final { return m_ProgramHandle; }
    ModuleInterface* GetInterface() const noexcept final { return m_Interface.Get(); }
};

StaticModuleHandle::NativeModule StaticModuleHandle::m_ProgramHandle = LoadMainProgram();

export template <CallableWithReturnT<ModuleInterface*> FactoryT>
class StaticModuleInitializer {
private:
    static_assert(NoThrowDefaultConstructibleT<FactoryT>,
                  "factory must be nothrow default constructible");

public:
    static void Register(StringView name)
    {
        ModuleInterface* interface = FactoryT{}();
        SharedPtr<StaticModuleHandle> handle = MakeShared<StaticModuleHandle>(name, interface);
        g_ModuleLoader.RegisterUninitialized(name, StaticCast<ModuleHandle>(MoveArg(handle)));
    }
};

} // namespace mini