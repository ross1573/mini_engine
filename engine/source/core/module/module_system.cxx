export module mini.core:module_system;

import :type;
import :string;
import :string_view;
import :shared_ptr;
import :module_platform;

namespace mini {

class CORE_API ModuleInterface {
private:
    friend class ModuleLoader;

public:
    virtual ~ModuleInterface() noexcept = default;

protected:
    virtual bool Initialize() { return true; }
};

class CORE_API ModuleHandle {
public:
    typedef NativeModuleHandle NativeHandle;
    typedef ModuleInterface Interface;

public:
    virtual ~ModuleHandle() noexcept = default;

    virtual bool IsValid() const noexcept = 0;

    virtual NativeHandle GetNativeHandle() noexcept = 0;
    virtual Interface* GetInterface() noexcept = 0;
    virtual const Interface* GetInterface() const noexcept = 0;

protected:
    ModuleHandle() = default;
};

export class CORE_API Module {
public:
    typedef ModuleHandle Handle;
    typedef ModuleInterface Interface;

private:
    template <CallableWithReturnT<Interface*> FactoryT>
    friend class StaticModuleInitializer;
    friend class ModuleLoader;

    SharedPtr<Handle> m_Handle;
    Interface* m_Interface;
    String m_Name;

public:
    Module() = default;
    Module(StringView);

    bool IsValid() const noexcept { return m_Handle.IsValid(); }
    String GetName() const noexcept { return m_Name; }

    Interface* GetInterface() noexcept { return m_Interface; }
    Interface const* GetInterface() const noexcept { return m_Interface; }

    template <DerivedFromT<Interface> T>
    T* GetInterface() noexcept;

    template <DerivedFromT<Interface> T>
    T const* GetInterface() const noexcept;

private:
    Module(StringView, SharedPtr<ModuleHandle>&&);
};

template <DerivedFromT<ModuleInterface> T>
T* Module::GetInterface() noexcept
{
    return dynamic_cast<T*>(m_Interface);
}

template <DerivedFromT<ModuleInterface> T>
T const* Module::GetInterface() const noexcept
{
    return dynamic_cast<T*>(m_Interface);
}

} // namespace mini