export module mini.core:module_initializer;

import :type;
import :string_view;
import :module_system;
import :module_loader;

namespace mini {

template <typename T>
concept InterfaceFactoryT = BaseOfT<ModuleInterface, typename T::Interface> && //
                            DefaultConstructibleT<typename T::Interface> &&    //
                            NoThrowDefaultConstructibleT<T> &&                 //
                            requires                                           //
{
    { T{}.operator()() } -> SameAsT<ModuleInterface*>;
};

export class StaticModuleInitializer {
public:
    template <InterfaceFactoryT T>
    static void registerUninitialized(StringView name)
    {
        moduleLoader.registerUninitialized(name, &T::operator());
    }

    static void registerUninitialized(StringView name, LoaderRef::Loader loader)
    {
        moduleLoader.registerUninitialized(name, loader);
    }
};

} // namespace mini