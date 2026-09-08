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
    typedef typename Array<LoaderRef>::Iterator LoaderIterator;
    typedef typename Array<ModuleRef>::Iterator ModuleIterator;
    typedef typename Array<StringView>::Iterator PendingIterator;

    // TODO: use hash map instead
    Array<LoaderRef> m_registered;
    Array<ModuleRef> m_modules;
    Array<StringView> m_pending;

public:
    LoaderIterator FindRegistered(StringView);
    ModuleIterator FindLoaded(StringView);

    bool Register(StringView, LoaderRef::Loader);
    SharedPtr<ModuleHandle> Load(StringView);

    size_t Count() const noexcept;

private:
    SharedPtr<ModuleHandle> LoadHandle(StringView);

    bool FindCircularDependency(StringView);
};

CORE_API ModuleLoader g_moduleLoader = ModuleLoader();

} // namespace mini