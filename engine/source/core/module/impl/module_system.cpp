module mini.core;

import :array;
import :string_view;
import :shared_ptr;
import :module_system;
import :module_loader;
import :dynamic_module;

namespace mini {

Module::Module(StringView name)
    : Module(g_ModuleLoader.Load(name))
{
}

Module::Module(StringView name, SharedPtr<ModuleHandle>&& handle)
    : m_Handle(MoveArg(handle))
    , m_Name(name)
{
    if (m_Handle->IsValid()) [[likely]] {
        m_Interface = m_Handle->GetInterface();
    }
    else {
        m_Interface = nullptr;
    }
}

Module ModuleLoader::Register(Module&& in)
{
    String name = in.GetName();
    Iterator iter = Find(name);
    if (iter != m_Modules.End()) {
        return *iter;
    }

    for (Module const& uninitialized : m_Uninitialized) {
        if (uninitialized.GetName() == name) {
            return uninitialized;
        }
    }

    m_Uninitialized.Push(in);
    return MoveArg(in);
}

Module ModuleLoader::Load(StringView name)
{
    Iterator iter = Find(name);
    if (iter != m_Modules.End()) {
        return *iter;
    }

    for (Module& uninitialized : m_Uninitialized) {
        if (uninitialized.GetName() == name) {
            ModuleInterface* interface = uninitialized.GetInterface();
            if (interface != nullptr && interface->Initialize() == false) {
                return Module();
            }

            m_Modules.Push(uninitialized);
            return uninitialized;
        }
    }

    SharedPtr<DynamicModuleHandle> dynHandle = MakeShared<DynamicModuleHandle>(name);
    if (dynHandle->IsValid() == false) {
        return Module();
    }

    Module mod(name, MoveArg(dynHandle));
    m_Modules.Push(mod);
    return mod;
}

ModuleLoader::Iterator ModuleLoader::Find(StringView name)
{
    if (name == StringView::Empty()) [[unlikely]] {
        return m_Modules.End();
    }

    Iterator iter = m_Modules.Begin();
    for (; iter != m_Modules.End(); ++iter) {
        if (iter->GetName() == name) {
            break;
        }
    }

    return iter;
}

} // namespace mini