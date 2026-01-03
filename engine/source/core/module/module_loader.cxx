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

class CORE_API ModuleLoader {
private:
    typedef typename Array<ModuleRef>::Iterator RefIterator;
    typedef typename Array<ModuleRef>::ConstIterator RefConstIterator;
    typedef typename Array<ModuleWeakRef>::Iterator WeakRefIterator;

    // TODO: use hash map instead
    Array<ModuleRef> m_Uninitialized;
    Array<ModuleWeakRef> m_Modules;

public:
    bool RegisterUninitialized(StringView, SharedPtr<ModuleHandle>);
    SharedPtr<ModuleHandle> Load(StringView);

    SizeT Count() const noexcept;

private:
    SharedPtr<ModuleHandle> LoadHandle(StringView);
};

CORE_API ModuleLoader g_ModuleLoader = ModuleLoader();

} // namespace mini