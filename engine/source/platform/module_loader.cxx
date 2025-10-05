export module mini.platform:module_loader;

import mini.core;

namespace mini::platform {

export class PLATFORM_API ModuleLoader {
private:
    Array<SharedPtr<Module>> m_LoadedModules;

public:
    SharedPtr<Module> LoadModule(StringView);

    bool ContainsModule(StringView);
};

} // namespace mini::platform