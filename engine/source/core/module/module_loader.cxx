export module mini.core:module_loader;

import :array;
import :string;
import :string_view;
import :shared_ptr;
import :weak_ptr;
import :module_system;

namespace mini {

struct LoaderRef {
public:
    typedef ModuleInterface* (*Loader)();

    Loader loader;
    String name;
};

struct ModuleRef {
public:
    WeakPtr<ModuleHandle> handle;
    String name;
};

struct CORE_API PendingGuard {
private:
    Array<StringView>& m_pending;
    StringView m_target;

public:
    PendingGuard(Array<StringView>& pending, StringView target) noexcept;
    ~PendingGuard() noexcept;
};

class CORE_API ModuleLoader {
private:
    typedef Array<LoaderRef>::Iterator LoaderIterator;
    typedef Array<ModuleRef>::Iterator ModuleIterator;
    typedef Array<StringView>::Iterator PendingIterator;

    // TODO: use hash map instead
    Array<LoaderRef> m_registered;
    Array<ModuleRef> m_modules;
    Array<StringView> m_pending;

public:
    LoaderIterator findRegistered(StringView name);
    ModuleIterator findLoaded(StringView name);

    bool registerUninitialized(StringView name, LoaderRef::Loader loader);
    SharedPtr<ModuleHandle> load(StringView name);

    [[nodiscard]] size_t count() const noexcept;

private:
    SharedPtr<ModuleHandle> loadHandle(StringView name);

    bool findCircularDependency(StringView name);
};

CORE_API ModuleLoader moduleLoader = ModuleLoader();

} // namespace mini