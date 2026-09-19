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
            debug::LogEnsure("ModuleHandle::~ModuleHandle()",
                             "exception occured while invoking AtExit callback {} on module {}",
                             reinterpret_cast<void*>(callback),
                             m_libraryName);
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
    return m_policy->validator == nullptr || m_policy->validator(m_nativeModule);
}

bool ModuleHandle::AtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_exitCallback.Begin(), m_exitCallback.End(), func);
    if (iter.Valid()) {
        return false;
    }

    m_exitCallback.PushBack(func);
    return true;
}

bool ModuleHandle::RemoveAtExit(CallbackFunc func) noexcept
{
    auto iter = Find(m_exitCallback.Begin(), m_exitCallback.End(), func);
    if (!iter.Valid()) {
        return false;
    }

    m_exitCallback.Remove(iter);
    return true;
}

SharedPtr<ModuleHandle> ModuleHandle::Load(StringView libName)
{
    if (libName.Empty()) [[unlikely]] {
        return { };
    }

    return g_moduleLoader.Load(libName);
}

DynamicModuleHandle::DynamicModuleHandle(StringView name) noexcept
    : ModuleHandle(&policy, nullptr, nullptr, name)
{
    m_nativeModule = LoadModule(name);
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
        debug::LogEnsure("__start_module()",
                         "exception occured while invoking __start_module on module {}",
                         m_libraryName);
    }

    ENSURE(interface, "failed to get interface object of module {}", name) {
        UnloadModule(m_nativeModule);
        m_nativeModule = nullptr;
        return;
    }

    m_interface = UniquePtr(interface);
}

ModuleLoader::LoaderIterator ModuleLoader::FindRegistered(StringView name)
{
    return FindIf(m_registered.Begin(), m_registered.End(), [&name](LoaderRef const& ref) noexcept {
        return ref.name == name;
    });
}

ModuleLoader::ModuleIterator ModuleLoader::FindLoaded(StringView name)
{
    return FindIf(m_modules.Begin(), m_modules.End(), [&name](ModuleRef const& ref) noexcept {
        return ref.name == name;
    });
}

bool ModuleLoader::Register(StringView name, LoaderRef::Loader loader)
{
    LoaderIterator loaderIter = FindRegistered(name);
    if (loaderIter.Valid()) {
        return false;
    }

    m_registered.PushBack(LoaderRef{ .loader = loader, .name = name });
    return true;
}

PendingGuard::PendingGuard(Array<StringView>& pending, StringView target) noexcept
    : m_pending(pending)
    , m_target(target)
{
    m_pending.PushBack(target);
}

PendingGuard::~PendingGuard() noexcept
{
    Array<StringView>::Iterator pendingIter = Find(m_pending.Begin(), m_pending.End(), m_target);
    if (pendingIter.Valid()) {
        m_pending.Remove(pendingIter);
    }
}

SharedPtr<ModuleHandle> ModuleLoader::Load(StringView name)
{
    ModuleIterator modulefIter = FindLoaded(name);
    if (modulefIter.Valid()) {
        if (modulefIter->handle.Valid()) {
            return StaticCast<ModuleHandle>(modulefIter->handle.Lock());
        }

        m_modules.Remove(modulefIter);
    }

    if (FindCircularDependency(name)) {
        return nullptr;
    }

    PendingGuard guard(m_pending, name);
    SharedPtr<ModuleHandle> handle = LoadHandle(name);
    if (!handle.Valid()) {
        return nullptr;
    }

    ModuleInterface* interface = handle->GetInterface();
    if (interface == nullptr) {
        return nullptr;
    }

    m_modules.PushBack(ModuleRef{ .handle = handle, .name = name });
    return handle;
}

SharedPtr<ModuleHandle> ModuleLoader::LoadHandle(StringView name)
{
    LoaderIterator loaderIter = FindRegistered(name);
    if (loaderIter.Valid()) {
        ModuleInterface* interface = loaderIter->loader();
        SharedPtr<StaticModuleHandle> staticHandle = MakeShared<StaticModuleHandle>(name, interface);
        return StaticCast<ModuleHandle>(MoveArg(staticHandle));
    }

    SharedPtr<DynamicModuleHandle> dynHandle = MakeShared<DynamicModuleHandle>(name);
    if (dynHandle->Valid()) {
        return StaticCast<ModuleHandle>(MoveArg(dynHandle));
    }

    return nullptr;
}

bool ModuleLoader::FindCircularDependency(StringView name)
{
    PendingIterator pendingIter = Find(m_pending.Begin(), m_pending.End(), name);

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
        if (mod.handle.Valid()) {
            ++count;
        }
    }

    return count;
}

} // namespace mini