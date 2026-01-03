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

ModuleHandle::ModuleHandle(StringView name) noexcept
    : m_Name(name)
{
}

ModuleHandle::~ModuleHandle() noexcept
{
    for (auto func : m_ExitCallback) {
        try {
            func();
        }
        catch (...) {
            String msg = Format("exception occured while invoking "
                                "AtExit callback {} on module {}",
                                (void*)func, m_Name.Data());
            ENSURE(false, msg.Data()) {}
        }
    }
}

bool ModuleHandle::AtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_ExitCallback.Begin(), m_ExitCallback.End(), func);
    if (iter != m_ExitCallback.End()) {
        return false;
    }

    m_ExitCallback.Push(func);
    return true;
}

bool ModuleHandle::RemoveAtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_ExitCallback.Begin(), m_ExitCallback.End(), func);
    if (iter == m_ExitCallback.End()) {
        return false;
    }

    m_ExitCallback.RemoveAt(iter);
    return true;
}

Module::Module() noexcept
    : m_Handle()
    , m_Interface(nullptr)
{
}

Module::Module(StringView name)
    : m_Handle(g_ModuleLoader.Load(name))
    , m_Interface(m_Handle != nullptr ? m_Handle->GetInterface() : nullptr)
{
}

void Module::Load(StringView name)
{
    m_Handle = g_ModuleLoader.Load(name);
    m_Interface = m_Handle != nullptr ? m_Handle->GetInterface() : nullptr;
}

void Module::Unload() noexcept
{
    m_Handle.Reset();
    m_Interface = nullptr;
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

SharedPtr<ModuleHandle> ModuleLoader::Load(StringView name)
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

    SharedPtr<ModuleHandle> handle = LoadHandle(name);
    if (handle.IsValid() == false) {
        return nullptr;
    }

    ModuleInterface* interface = handle->GetInterface();
    if (interface != nullptr && interface->Initialize() == false) {
        return nullptr;
    }

    m_Modules.Push(ModuleWeakRef{ .handle = handle, .name = name });
    return handle;
}

SharedPtr<ModuleHandle> ModuleLoader::LoadHandle(StringView name)
{
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

SizeT ModuleLoader::Count() const noexcept
{
    SizeT count = 0;
    for (auto const& mod : m_Modules) {
        if (mod.handle.IsValid()) ++count;
    }

    return count;
}

} // namespace mini