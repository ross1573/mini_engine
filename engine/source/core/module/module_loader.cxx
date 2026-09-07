export module mini.core:module_loader;

import :array;
import :string;
import :string_view;
import :shared_ptr;
import :weak_ptr;
import :module_system;

namespace mini {

struct ModuleRef {
public:
    SharedPtr<ModuleHandle> handle;
    String name;
};

struct ModuleWeakRef {
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
    typedef typename Array<ModuleRef>::Iterator RefIterator;
    typedef typename Array<ModuleRef>::ConstIterator RefConstIterator;
    typedef typename Array<ModuleWeakRef>::Iterator WeakRefIterator;
    typedef typename Array<StringView>::Iterator PendingIterator;

    // TODO: use hash map instead
    Array<ModuleRef> m_uninitialized;
    Array<ModuleWeakRef> m_modules;
    Array<StringView> m_pending;

public:
    RefIterator FindRegistered(StringView);
    WeakRefIterator FindLoaded(StringView);

    bool RegisterUninitialized(StringView, SharedPtr<ModuleHandle>);
    SharedPtr<ModuleHandle> Load(StringView);

    size_t Count() const noexcept;

private:
    SharedPtr<ModuleHandle> LoadHandle(StringView);

    bool FindCircularDependency(StringView);
};

CORE_API ModuleLoader g_moduleLoader = ModuleLoader();

} // namespace mini