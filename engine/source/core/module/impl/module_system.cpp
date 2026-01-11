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
    : m_LibraryName(name)
{
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

void ModuleHandle::InvokeExitCallbacks() noexcept
{
    for (auto func : m_ExitCallback) {
        try {
            func();
        }
        catch (...) {
            String msg = Format("exception occured while invoking AtExit callback {} on module {}",
                                (void*)func, m_LibraryName);

            ENSURE(false, msg.Data()) {}
        }
    }
}

String ModuleHandle::LibraryName() const noexcept
{
    return m_LibraryName;
}

SharedPtr<ModuleHandle> ModuleHandle::Load(StringView libName)
{
    if (libName == StringView::Empty()) [[unlikely]] {
        return SharedPtr<ModuleHandle>();
    }

    return g_ModuleLoader.Load(libName);
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