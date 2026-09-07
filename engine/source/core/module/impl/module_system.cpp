module mini.core;

import :weak_ptr;
import :algorithm;
import :module_system;
import :module_loader;

namespace mini {

ModuleHandle::ModuleHandle(ModulePoilcy const* policy,
                           NativeModule nativeModule,
                           ModuleInterface* interface,
                           StringView libraryName) noexcept
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
            ENSURE(false,
                   "exception occured while invoking AtExit callback {} on module {}",
                   (void*)callback,
                   m_libraryName) { }
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
    if (iter.Valid()) {
        return false;
    }

    m_exitCallback.Push(func);
    return true;
}

bool ModuleHandle::RemoveAtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_exitCallback.Begin(), m_exitCallback.End(), func);
    if (iter.Valid() == false) {
        return false;
    }

    m_exitCallback.RemoveAt(iter);
    return true;
}

SharedPtr<ModuleHandle> ModuleHandle::Load(StringView libName)
{
    if (libName.Empty()) [[unlikely]] {
        return SharedPtr<ModuleHandle>();
    }

    return g_moduleLoader.Load(libName);
}

DynamicModuleHandle::DynamicModuleHandle(StringView name) noexcept
    : ModuleHandle(&policy, nullptr, nullptr, name)
{
    String path = BuildModulePath(name);
    m_nativeModule = LoadModule(path);
    ENSURE(m_nativeModule, "failed to load module {}", name) {
        return;
    }

    StartFunc startFunc = GetFunction<ModuleInterface*>("__start_module");
    ENSURE(startFunc, "failed to locate start function of module {}", name) {
        return;
    }

    ModuleInterface* interface = nullptr;
    try {
        interface = startFunc();
    } catch (...) {
    }

    ENSURE(interface, "failed to get interface object of module {}", name) {
        UnloadModule(m_nativeModule);
        m_nativeModule = nullptr;
        return;
    }

    m_interface = UniquePtr(interface);
}

ModuleLoader::RefIterator ModuleLoader::FindRegistered(StringView name)
{
    return FindIf(m_uninitialized.Begin(), m_uninitialized.End(), [&name](ModuleRef const& ref) noexcept {
        return ref.name == name;
    });
}

ModuleLoader::WeakRefIterator ModuleLoader::FindLoaded(StringView name)
{
    return FindIf(m_modules.Begin(), m_modules.End(), [&name](ModuleWeakRef const& ref) noexcept {
        return ref.name == name;
    });
}

bool ModuleLoader::RegisterUninitialized(StringView name, SharedPtr<ModuleHandle> handle)
{
    RefIterator refIterator = FindRegistered(name);
    if (refIterator.Valid()) {
        return false;
    }

    m_uninitialized.Push(ModuleRef{ .handle = MoveArg(handle), .name = name });
    return true;
}

PendingGuard::PendingGuard(Array<StringView>& pending, StringView target) noexcept
    : m_pending(pending)
    , m_target(target)
{
    m_pending.Push(target);
}

PendingGuard::~PendingGuard() noexcept
{
    Array<StringView>::Iterator pendingIter = Find(m_pending.Begin(), m_pending.End(), m_target);
    if (pendingIter.Valid()) {
        m_pending.RemoveAt(pendingIter);
    }
}

SharedPtr<ModuleHandle> ModuleLoader::Load(StringView name)
{
    WeakRefIterator weakRefIter = FindLoaded(name);
    if (weakRefIter.Valid()) {
        if (weakRefIter->handle.Valid()) {
            return StaticCast<ModuleHandle>(weakRefIter->handle.Lock());
        }

        m_modules.RemoveAt(weakRefIter);
    }

    if (FindCircularDependency(name)) {
        return nullptr;
    }

    PendingGuard guard(m_pending, name);
    SharedPtr<ModuleHandle> handle = LoadHandle(name);
    if (handle.Valid() == false) {
        return nullptr;
    }

    ModuleInterface* interface = handle->GetInterface();
    if (interface == nullptr) {
        return nullptr;
    }

    m_modules.Push(ModuleWeakRef{ .handle = handle, .name = name });
    return handle;
}

SharedPtr<ModuleHandle> ModuleLoader::LoadHandle(StringView name)
{
    RefIterator refIter = FindRegistered(name);
    if (refIter != m_uninitialized.End()) {
        return StaticCast<ModuleHandle>(MoveArg(refIter->handle));
    }

    SharedPtr<DynamicModuleHandle> dynHandle = MakeShared<DynamicModuleHandle>(name);
    if (dynHandle->Valid()) {
        return StaticCast<ModuleHandle>(MoveArg(dynHandle));
    }

    return nullptr;
}

bool ModuleLoader::FindCircularDependency(StringView name)
{
    Array<StringView>::Iterator pendingIter = Find(m_pending.Begin(), m_pending.End(), name);

    ENSURE(pendingIter.Valid() == false,
           "circular dependency detected while loading module {}. load order: {}",
           name,
           m_pending) {
        return true;
    }

    return false;
}

size_t ModuleLoader::Count() const noexcept
{
    size_t count = 0;
    for (auto const& mod : m_modules) {
        if (mod.handle.Valid()) ++count;
    }

    return count;
}

} // namespace mini