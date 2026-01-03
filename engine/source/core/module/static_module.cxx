export module mini.core:static_module;

import :type;
import :unique_ptr;
import :string_view;
import :module_system;
import :module_loader;

namespace mini {

class CORE_API StaticModuleHandle : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeHandle NativeHandle;
    typedef typename ModuleHandle::Interface Interface;
    typedef typename ModuleHandle::ConstInterface ConstInterface;

private:
    UniquePtr<Interface> m_Interface;

public:
    StaticModuleHandle(StringView name, Interface* interface) noexcept
        : ModuleHandle(name)
        , m_Interface(interface)
    {
    }

    virtual ~StaticModuleHandle() noexcept = default;

    bool IsValid() const noexcept final { return true; }

    NativeHandle GetNativeHandle() noexcept final { return nullptr; }
    Interface* GetInterface() noexcept final { return m_Interface.Get(); }
    ConstInterface* GetInterface() const noexcept final { return m_Interface.Get(); }
};

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