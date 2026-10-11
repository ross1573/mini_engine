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
            debug::logEnsure("ModuleHandle::~ModuleHandle()",
                             "exception occured while invoking AtExit callback {} on module {}",
                             reinterpret_cast<void*>(callback),
                             m_libraryName);
        }
    }

    m_interface.reset();
    m_libraryName.clear();

    if (m_policy->deleter != nullptr) {
        m_policy->deleter(m_nativeModule);
    }

    m_nativeModule = nullptr;
}

bool ModuleHandle::valid() const noexcept
{
    return m_policy->validator == nullptr || m_policy->validator(m_nativeModule);
}

bool ModuleHandle::atExit(CallbackFunc func) noexcept
{
    auto iter = find(m_exitCallback.begin(), m_exitCallback.end(), func);
    if (iter.valid()) {
        return false;
    }

    m_exitCallback.pushBack(func);
    return true;
}

bool ModuleHandle::removeAtExit(CallbackFunc func) noexcept
{
    auto iter = find(m_exitCallback.begin(), m_exitCallback.end(), func);
    if (!iter.valid()) {
        return false;
    }

    m_exitCallback.remove(iter);
    return true;
}

SharedPtr<ModuleHandle> ModuleHandle::load(StringView libName)
{
    if (libName.empty()) [[unlikely]] {
        return {};
    }

    return moduleLoader.load(libName);
}

DynamicModuleHandle::DynamicModuleHandle(StringView name) noexcept
    : ModuleHandle(&policy, nullptr, nullptr, name)
{
    m_nativeModule = loadModule(name);
    ENSURE(m_nativeModule, "failed to load module {}", name) {
        return;
    }

    StartFunc startFunc = function<ModuleInterface*>("__start_module");
    ENSURE(startFunc, "failed to locate start function of module {}", name) {
        return;
    }

    ModuleInterface* interface = nullptr;
    try {
        interface = startFunc();
    } catch (...) {
        debug::logEnsure("__start_module()",
                         "exception occured while invoking __start_module on module {}",
                         m_libraryName);
    }

    ENSURE(interface, "failed to get interface object of module {}", name) {
        unloadModule(m_nativeModule);
        m_nativeModule = nullptr;
        return;
    }

    m_interface = UniquePtr(interface);
}

ModuleLoader::LoaderIterator ModuleLoader::findRegistered(StringView name)
{
    return findIf(m_registered.begin(), m_registered.end(), [&name](LoaderRef const& ref) noexcept {
        return ref.name == name;
    });
}

ModuleLoader::ModuleIterator ModuleLoader::findLoaded(StringView name)
{
    return findIf(m_modules.begin(), m_modules.end(), [&name](ModuleRef const& ref) noexcept {
        return ref.name == name;
    });
}

bool ModuleLoader::registerUninitialized(StringView name, LoaderRef::Loader loader)
{
    LoaderIterator loaderIter = findRegistered(name);
    if (loaderIter.valid()) {
        return false;
    }

    m_registered.pushBack(LoaderRef{.loader = loader, .name = name});
    return true;
}

PendingGuard::PendingGuard(Array<StringView>& pending, StringView target) noexcept
    : m_pending(pending)
    , m_target(target)
{
    m_pending.pushBack(target);
}

PendingGuard::~PendingGuard() noexcept
{
    Array<StringView>::Iterator pendingIter = find(m_pending.begin(), m_pending.end(), m_target);
    if (pendingIter.valid()) {
        m_pending.remove(pendingIter);
    }
}

SharedPtr<ModuleHandle> ModuleLoader::load(StringView name)
{
    ModuleIterator modulefIter = findLoaded(name);
    if (modulefIter.valid()) {
        if (modulefIter->handle.valid()) {
            return staticCast<ModuleHandle>(modulefIter->handle.lock());
        }

        m_modules.remove(modulefIter);
    }

    if (findCircularDependency(name)) {
        return nullptr;
    }

    PendingGuard guard(m_pending, name);
    SharedPtr<ModuleHandle> handle = loadHandle(name);
    if (!handle.valid()) {
        return nullptr;
    }

    ModuleInterface* interface = handle->interface();
    if (interface == nullptr) {
        return nullptr;
    }

    m_modules.pushBack(ModuleRef{.handle = handle, .name = name});
    return handle;
}

SharedPtr<ModuleHandle> ModuleLoader::loadHandle(StringView name)
{
    LoaderIterator loaderIter = findRegistered(name);
    if (loaderIter.valid()) {
        ModuleInterface* interface = loaderIter->loader();
        SharedPtr<StaticModuleHandle> staticHandle = makeShared<StaticModuleHandle>(name, interface);
        return staticCast<ModuleHandle>(moveCast(staticHandle));
    }

    SharedPtr<DynamicModuleHandle> dynHandle = makeShared<DynamicModuleHandle>(name);
    if (dynHandle->valid()) {
        return staticCast<ModuleHandle>(moveCast(dynHandle));
    }

    return nullptr;
}

bool ModuleLoader::findCircularDependency(StringView name)
{
    PendingIterator pendingIter = find(m_pending.begin(), m_pending.end(), name);

    ENSURE(pendingIter.valid() == false,
           "circular dependency detected while loading module {}. load order: {}",
           name.data(),
           m_pending) {
        return true;
    }

    return false;
}

size_t ModuleLoader::count() const noexcept
{
    size_t count = 0;
    for (auto const& mod : m_modules) {
        if (mod.handle.valid()) {
            ++count;
        }
    }

    return count;
}

} // namespace mini