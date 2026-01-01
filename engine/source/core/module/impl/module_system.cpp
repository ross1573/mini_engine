module mini.core;

import :array;
import :string_view;
import :shared_ptr;
import :weak_ptr;
import :algorithm;
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

bool ModuleLoader::RegisterUninitialized(StringView name, SharedPtr<ModuleHandle> handle)
{
    RefIterator refIter =
        FindIf(m_Uninitialized.Begin(), m_Uninitialized.End(),
               [&name](ModuleRef const& ref) noexcept { return ref.name == name; });

    if (refIter != m_Uninitialized.End()) {
        return false;
    }

    m_Uninitialized.Push(ModuleRef{ .handle = MoveArg(handle), .name = name });
    return true;
}

Module ModuleLoader::Load(StringView name)
{
    SharedPtr<ModuleHandle> handle = LoadHandle(name);
    if (handle.IsValid() == false) {
        return Module();
    }

    ModuleInterface* interface = handle->GetInterface();
    if (interface != nullptr && interface->Initialize() == false) {
        return Module();
    }

    m_Modules.Push(ModuleWeakRef{ .handle = handle, .name = name });
    return Module(name, MoveArg(handle));
}

SharedPtr<ModuleHandle> ModuleLoader::LoadHandle(StringView name)
{
    WeakRefIterator weakRefIter =
        FindIf(m_Modules.Begin(), m_Modules.End(),
               [&name](ModuleWeakRef const& ref) noexcept { return ref.name == name; });

    if (weakRefIter != m_Modules.End()) {
        if (weakRefIter->handle.IsValid()) {
            return StaticCast<ModuleHandle>(weakRefIter->handle.Lock());
        }

        m_Modules.RemoveAt(weakRefIter);
    }

    RefIterator refIter =
        FindIf(m_Uninitialized.Begin(), m_Uninitialized.End(),
               [&name](ModuleRef const& ref) noexcept { return ref.name == name; });

    if (refIter != m_Uninitialized.End()) {
        return StaticCast<ModuleHandle>(MoveArg(refIter->handle));
    }

    SharedPtr<DynamicModuleHandle> dynHandle = MakeShared<DynamicModuleHandle>(name);
    if (dynHandle->IsValid()) {
        return StaticCast<ModuleHandle>(MoveArg(dynHandle));
    }

    return nullptr;
}

} // namespace mini