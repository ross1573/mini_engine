export module mini.core:module_initializer;

import :type;
import :string_view;
import :module_system;
import :module_loader;

namespace mini {

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
        g_moduleLoader.RegisterUninitialized(name, StaticCast<ModuleHandle>(MoveArg(handle)));
    }
};

} // namespace mini