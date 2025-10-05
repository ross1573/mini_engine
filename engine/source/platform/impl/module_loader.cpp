module mini.platform;

import mini.core;
import :module_loader;

namespace mini::platform {

SharedPtr<Module> ModuleLoader::LoadModule(StringView name)
{
    for (SharedPtr<Module>& mod : m_LoadedModules) {
        if (mod->GetName() == name) {
            return mod;
        }
    }

    SharedPtr<Module> mod = Module::LoadModule(name);
    if (mod.IsValid() == false) {
        return mod;
    }

    m_LoadedModules.Push(SharedPtr<Module>(mod));
    return mod;
}

bool ModuleLoader::ContainsModule(StringView name)
{
    for (SharedPtr<Module>& mod : m_LoadedModules) {
        if (mod->GetName() == name) {
            return true;
        }
    }

    return false;
}

} // namespace mini::platform