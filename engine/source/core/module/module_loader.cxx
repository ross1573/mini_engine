export module mini.core:module_loader;

import :array;
import :string;
import :string_view;
import :shared_ptr;
import :module_system;

namespace mini {

class CORE_API ModuleLoader {
private:
    Array<Module> m_Uninitialized;
    Array<Module> m_Modules;

    using Iterator = Array<Module>::Iterator;

public:
    Module Register(Module&&);
    Module Load(StringView);

    static bool ConstructModule(ModuleInterface* interface) { return interface->Initialize(); }
    static void DestructModule(ModuleInterface* interface) { return interface->Shutdown(); }

private:
    Iterator Find(StringView);
};

CORE_API ModuleLoader g_ModuleLoader = ModuleLoader();

} // namespace mini