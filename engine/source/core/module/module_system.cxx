export module mini.core:module_system;

import :type;
import :array;
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
    typedef ModuleInterface const ConstInterface;
    typedef void (*CallbackFunc)();

private:
    Array<CallbackFunc> m_ExitCallback;
    String m_Name;

public:
    ModuleHandle(StringView) noexcept;
    virtual ~ModuleHandle() noexcept;

    virtual bool IsValid() const noexcept = 0;

    bool AtExit(CallbackFunc) noexcept;
    bool RemoveAtExit(CallbackFunc) noexcept;

    String GetName() const noexcept { return m_Name; }
    virtual Interface* GetInterface() noexcept = 0;
    virtual ConstInterface* GetInterface() const noexcept = 0;
    virtual NativeHandle GetNativeHandle() noexcept = 0;

protected:
    ModuleHandle() = default;
};

export class CORE_API Module {
public:
    typedef ModuleHandle Handle;
    typedef typename Handle::NativeHandle NativeHandle;
    typedef typename Handle::Interface Interface;
    typedef typename Handle::ConstInterface ConstInterface;
    typedef typename Handle::CallbackFunc CallbackFunc;

private:
    SharedPtr<Handle> m_Handle;
    Interface* m_Interface;

public:
    Module() noexcept;
    Module(StringView);

    bool IsValid() const noexcept;

    void Load(StringView);
    void Unload() noexcept;

    bool AtExit(CallbackFunc) noexcept;
    bool RemoveAtExit(CallbackFunc) noexcept;

    String GetName() const noexcept;
    Interface* GetInterface() noexcept;
    ConstInterface* GetInterface() const noexcept;
    NativeHandle GetNativeHandle() const noexcept;

    template <DerivedFromT<Interface> T>
    T* GetInterface() noexcept;

    template <DerivedFromT<Interface> T>
    const T* GetInterface() const noexcept;
};

inline bool Module::IsValid() const noexcept
{
    return m_Handle != nullptr ? m_Handle->IsValid() : false;
}

inline bool Module::AtExit(CallbackFunc func) noexcept
{
    return m_Handle != nullptr ? m_Handle->AtExit(func) : false;
}

inline bool Module::RemoveAtExit(CallbackFunc func) noexcept
{
    return m_Handle != nullptr ? m_Handle->RemoveAtExit(func) : false;
}

inline Module::NativeHandle Module::GetNativeHandle() const noexcept
{
    return m_Handle != nullptr ? m_Handle->GetNativeHandle() : nullptr;
}

inline String Module::GetName() const noexcept
{
    return m_Handle != nullptr ? m_Handle->GetName() : String();
}

inline Module::Interface* Module::GetInterface() noexcept
{
    return m_Interface;
}

inline Module::ConstInterface* Module::GetInterface() const noexcept
{
    return m_Interface;
}

template <DerivedFromT<ModuleInterface> T>
inline T* Module::GetInterface() noexcept
{
    if constexpr (SameAsT<T, ModuleInterface>) {
        return m_Interface;
    }

    return dynamic_cast<T*>(m_Interface);
}

template <DerivedFromT<ModuleInterface> T>
inline T const* Module::GetInterface() const noexcept
{
    if constexpr (SameAsT<T, ModuleInterface>) {
        return m_Interface;
    }

    return dynamic_cast<T*>(m_Interface);
}

} // namespace mini