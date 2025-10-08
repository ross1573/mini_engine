export module mini.core:module_system;

import :array;
import :string_view;
import :string;
import :shared_ptr;

namespace mini {

export class CORE_API ModuleInterface {
public:
    virtual ~ModuleInterface() noexcept = default;

    virtual void Startup() {}
    virtual void Shutdown() {}

protected:
    ModuleInterface() = default;
};

class ModuleHandle {
public:
    virtual ~ModuleHandle() noexcept = default;

    virtual bool IsValid() const noexcept = 0;
    virtual void* GetNativeHandle() noexcept = 0;
    virtual ModuleInterface* GetInterface() noexcept = 0;

protected:
    ModuleHandle() = default;
};

export class CORE_API Module {
private:
    SharedPtr<ModuleHandle> m_Handle;
    ModuleInterface* m_Interface;
    String m_Name;

    template <DerivedFromT<ModuleInterface> T, CallableWithReturnT<T*> FactoryT,
              DeleterT<ModuleInterface>>
    friend class StaticModuleInitializer;
    friend class ModuleLoader;

public:
    Module() = default;
    Module(StringView);

    bool IsValid() const noexcept { return m_Handle.IsValid(); }
    String GetName() const { return m_Name; }
    ModuleInterface* GetInterface() const noexcept { return m_Interface; }

    void* GetNativeHandle() const noexcept
    {
        return m_Handle.IsValid() ? m_Handle->GetNativeHandle() : nullptr;
    }

    template <DerivedFromT<ModuleInterface> T>
    T* GetInterface() noexcept
    {
        return dynamic_cast<T*>(GetInterface());
    }

private:
    Module(StringView, SharedPtr<ModuleHandle>&&);
};

class ModuleLoader {
private:
    Array<Module> m_Uninitialized;
    Array<Module> m_Modules;

    using Iterator = Array<Module>::Iterator;

public:
    Module Register(Module&&);
    Module Load(StringView);

private:
    Iterator Find(StringView);
};

ModuleLoader g_ModuleLoader = ModuleLoader();

} // namespace mini