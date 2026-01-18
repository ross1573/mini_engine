module mini.core;

import :array;
import :string_view;
import :shared_ptr;
import :weak_ptr;
import :algorithm;
import :module_system;
import :module_loader;

namespace mini {

ModuleHandle::ModuleHandle(ModulePoilcy const* policy, NativeModule nativeModule,
                           ModuleInterface* interface, StringView libraryName) noexcept
    : m_policy(policy)
    , m_nativeModule(nativeModule)
    , m_interface(interface)
    , m_libraryName(libraryName)
{
}

ModuleHandle::~ModuleHandle() noexcept
{
    for (auto callback : m_exitCallback) {
        try {
            callback();
        } catch (...) {
            String msg = Format("exception occured while invoking AtExit callback {} on module {}",
                                (void*)callback, m_libraryName);

            ENSURE(false, msg.Data()) {}
        }
    }

    m_interface.Reset();
    m_libraryName.Clear();

    if (m_policy->deleter != nullptr) {
        m_policy->deleter(m_nativeModule);
    }

    m_nativeModule = nullptr;
}

bool ModuleHandle::Valid() const noexcept
{
    return m_policy->validator != nullptr && m_policy->validator(m_nativeModule);
}

bool ModuleHandle::AtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_exitCallback.Begin(), m_exitCallback.End(), func);
    if (iter != m_exitCallback.End()) {
        return false;
    }

    m_exitCallback.Push(func);
    return true;
}

bool ModuleHandle::RemoveAtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_exitCallback.Begin(), m_exitCallback.End(), func);
    if (iter == m_exitCallback.End()) {
        return false;
    }

    m_exitCallback.RemoveAt(iter);
    return true;
}

String ModuleHandle::LibraryName() const noexcept
{
    return m_libraryName;
}

ModuleHandle::NativeModule ModuleHandle::NativeHandle() noexcept
{
    return m_nativeModule;
}

ModuleInterface* ModuleHandle::GetInterface() const noexcept
{
    return m_interface.Get();
}

SharedPtr<ModuleHandle> ModuleHandle::Load(StringView libName)
{
    if (libName.Empty()) [[unlikely]] {
        return SharedPtr<ModuleHandle>();
    }

    return g_moduleLoader.Load(libName);
}

bool ModuleLoader::RegisterUninitialized(StringView name, SharedPtr<ModuleHandle> handle)
{
    RefIterator refIter =
        FindIf(m_uninitialized.Begin(), m_uninitialized.End(),
               [&name](ModuleRef const& ref) noexcept { return ref.name == name; });

    if (refIter != m_uninitialized.End()) {
        return false;
    }

    m_uninitialized.Push(ModuleRef{ .handle = MoveArg(handle), .name = name });
    return true;
}

SharedPtr<ModuleHandle> ModuleLoader::Load(StringView name)
{
    WeakRefIterator weakRefIter =
        FindIf(m_modules.Begin(), m_modules.End(),
               [&name](ModuleWeakRef const& ref) noexcept { return ref.name == name; });

    if (weakRefIter != m_modules.End()) {
        if (weakRefIter->handle.Valid()) {
            return StaticCast<ModuleHandle>(weakRefIter->handle.Lock());
        }

        m_modules.RemoveAt(weakRefIter);
    }

    SharedPtr<ModuleHandle> handle = LoadHandle(name);
    if (handle.Valid() == false) {
        return nullptr;
    }

    ModuleInterface* interface = handle->GetInterface();
    if (interface != nullptr && interface->Initialize() == false) {
        return nullptr;
    }

    m_modules.Push(ModuleWeakRef{ .handle = handle, .name = name });
    return handle;
}

SharedPtr<ModuleHandle> ModuleLoader::LoadHandle(StringView name)
{
    RefIterator refIter =
        FindIf(m_uninitialized.Begin(), m_uninitialized.End(),
               [&name](ModuleRef const& ref) noexcept { return ref.name == name; });

    if (refIter != m_uninitialized.End()) {
        return StaticCast<ModuleHandle>(MoveArg(refIter->handle));
    }

    SharedPtr<DynamicModuleHandle> dynHandle = MakeShared<DynamicModuleHandle>(name);
    if (dynHandle->Valid()) {
        return StaticCast<ModuleHandle>(MoveArg(dynHandle));
    }

    return nullptr;
}

SizeT ModuleLoader::Count() const noexcept
{
    SizeT count = 0;
    for (auto const& mod : m_modules) {
        if (mod.handle.Valid()) ++count;
    }

    return count;
}

} // namespace mini