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

private:
    UniquePtr<Interface> m_Interface;

public:
    StaticModuleHandle(StringView) noexcept {}
    StaticModuleHandle(StringView, Interface* interface) noexcept
        : m_Interface(interface)
    {
    }

    virtual ~StaticModuleHandle() noexcept = default;

    bool IsValid() const noexcept final { return true; }

    NativeHandle GetNativeHandle() noexcept final { return nullptr; }
    Interface* GetInterface() noexcept final { return m_Interface.Get(); }
    const Interface* GetInterface() const noexcept final { return m_Interface.Get(); }
};

export template <CallableWithReturnT<ModuleInterface*> FactoryT>
class StaticModuleInitializer {
private:
    static_assert(NoThrowDefaultConstructibleT<FactoryT>,
                  "factory must be nothrow default constructible");

public:
    static void Register(StringView name)
    {
        g_ModuleLoader.RegisterUninitialized(name, FactoryT{}());
    }
};

} // namespace mini