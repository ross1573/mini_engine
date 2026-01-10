export module mini.core:module_system;

import :type;
import :utility_operation;
import :array;
import :string;
import :string_view;
import :deleter;
import :shared_ptr;
import :module_platform;

namespace mini {

export class CORE_API ModuleInterface {
private:
    friend class ModuleLoader;

public:
    ModuleInterface() noexcept = default;
    virtual ~ModuleInterface() noexcept = default;

protected:
    virtual bool Initialize() { return true; }

    ModuleInterface(ModuleInterface const&) = delete;
    ModuleInterface(ModuleInterface&&) = delete;

    ModuleInterface& operator=(ModuleInterface const&) = delete;
    ModuleInterface& operator=(ModuleInterface&&) = delete;
};

class CORE_API ModuleHandle {
public:
    typedef NativeModuleHandle NativeModule;
    typedef void (*CallbackFunc)();

private:
    Array<CallbackFunc> m_ExitCallback;
    String m_LibraryName;

public:
    ModuleHandle(StringView) noexcept;
    virtual ~ModuleHandle() noexcept;

    virtual bool IsValid() const noexcept = 0;

    bool AtExit(CallbackFunc) noexcept;
    bool RemoveAtExit(CallbackFunc) noexcept;

    String LibraryName() const noexcept;
    virtual NativeModule NativeHandle() noexcept = 0;
    virtual ModuleInterface* GetInterface() const noexcept = 0;

    static SharedPtr<ModuleHandle> Load(StringView);

protected:
    ModuleHandle() noexcept = default;
};

template <typename T>
concept ModuleInterfaceT = ValueT<T> &&
                           ConvertibleToT<T const volatile*, ModuleInterface const volatile*>;

template <typename From, typename To>
concept RelatedInterfaceToT = ModuleInterfaceT<From> && ModuleInterfaceT<To> &&
                              ExplicitlyConvertibleToT<From const volatile*, To const volatile*>;

export template <ModuleInterfaceT T>
class Module {
private:
    typedef ModuleHandle Handle;

    template <ModuleInterfaceT U>
    friend class Module;

public:
    typedef typename Handle::NativeModule NativeModule;
    typedef typename Handle::CallbackFunc CallbackFunc;
    typedef T Interface;
    typedef T& InterfaceRef;
    typedef T* InterfacePtr;

private:
    SharedPtr<Handle> m_Handle;
    Interface* m_Interface;

public:
    Module() noexcept;
    ~Module() noexcept;
    Module(Module const&) noexcept = default;
    Module(Module&&) noexcept = default;

    explicit Module(StringView);
    template <UnbindedAllocatorT AllocT>
    Module(StringView, AllocT const&);
    template <RelatedInterfaceToT<T> U>
    Module(Module<U> const&) noexcept;
    template <RelatedInterfaceToT<T> U>
    Module(Module<U>&&) noexcept;

    bool IsValid() const noexcept;

    template <UnbindedAllocatorT AllocT>
    void Load(StringView, AllocT const&);
    void Load(StringView);
    void Release() noexcept;

    bool AtExit(CallbackFunc) noexcept;
    bool RemoveAtExit(CallbackFunc) noexcept;

    String LibraryName() const noexcept;
    InterfacePtr GetInterface() const noexcept;
    NativeModule NativeHandle() const noexcept;

    template <RelatedInterfaceToT<T> U>
    bool Equals(Module<U> const&) const noexcept;
    template <RelatedInterfaceToT<T> U>
    bool HandleEquals(Module<U> const&) const noexcept;

    InterfaceRef operator*() const noexcept;
    InterfacePtr operator->() const noexcept;

    Module& operator=(Module const&) noexcept = default;
    Module& operator=(Module&&) noexcept = default;

    template <RelatedInterfaceToT<T> U>
    Module& operator=(Module<U> const&) noexcept;
    template <RelatedInterfaceToT<T> U>
    Module& operator=(Module<U>&&) noexcept;

private:
    Interface* QueryInterface() const noexcept;
};

template <ModuleInterfaceT T>
inline Module<T>::Module() noexcept
    : m_Handle()
    , m_Interface(nullptr)
{
}

template <ModuleInterfaceT T>
inline Module<T>::Module(StringView name)
    : m_Handle(Handle::Load(name))
    , m_Interface(QueryInterface())
{
    if (m_Interface == nullptr) [[unlikely]] {
        m_Handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline Module<T>::~Module() noexcept
{
    m_Interface = nullptr;
}

template <ModuleInterfaceT T>
template <UnbindedAllocatorT AllocT>
inline Module<T>::Module(StringView name, AllocT const& alloc)
    : m_Handle(Handle::Load(name), alloc)
    , m_Interface(QueryInterface())
{
    if (m_Interface == nullptr) [[unlikely]] {
        m_Handle.Reset();
    }
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>::Module(Module<U> const& other) noexcept
    : m_Handle(other.m_Handle)
    , m_Interface(static_cast<U>(other.m_Interface))
{
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>::Module(Module<U>&& other) noexcept
    : m_Handle(MoveArg(other.m_Handle))
{
    m_Interface = static_cast<U>(other.m_Interface);
    other.m_Interface = nullptr;
}

template <ModuleInterfaceT T>
inline bool Module<T>::IsValid() const noexcept
{
    return m_Handle != nullptr;
}

template <ModuleInterfaceT T>
template <UnbindedAllocatorT AllocT>
inline void Module<T>::Load(StringView name, AllocT const& alloc)
{
    m_Handle.Reset(Handle::Load(name), DefaultDeleter<Handle>{}, alloc);
    m_Interface = QueryInterface();

    if (m_Interface == nullptr) [[unlikely]] {
        m_Handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline void Module<T>::Load(StringView name)
{
    m_Handle = Handle::Load(name);
    m_Interface = QueryInterface();

    if (m_Interface == nullptr) [[unlikely]] {
        m_Handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline void Module<T>::Release() noexcept
{
    m_Handle.Reset();
    m_Interface = nullptr;
}

template <ModuleInterfaceT T>
inline bool Module<T>::AtExit(CallbackFunc func) noexcept
{
    return m_Handle != nullptr ? m_Handle->AtExit(func) : false;
}

template <ModuleInterfaceT T>
inline bool Module<T>::RemoveAtExit(CallbackFunc func) noexcept
{
    return m_Handle != nullptr ? m_Handle->RemoveAtExit(func) : false;
}

template <ModuleInterfaceT T>
inline String Module<T>::LibraryName() const noexcept
{
    return m_Handle != nullptr ? m_Handle->LibraryName() : String();
}

template <ModuleInterfaceT T>
inline Module<T>::InterfacePtr Module<T>::GetInterface() const noexcept
{
    return m_Interface;
}

template <ModuleInterfaceT T>
inline Module<T>::NativeModule Module<T>::NativeHandle() const noexcept
{
    return m_Handle != nullptr ? m_Handle->NativeHandle() : nullptr;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
bool Module<T>::Equals(Module<U> const& other) const noexcept
{
    return m_Handle.OwnerEquals(other) && m_Interface == other.m_Interface;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
bool Module<T>::HandleEquals(Module<U> const& other) const noexcept
{
    return m_Handle.OwnerEquals(other);
}

template <ModuleInterfaceT T>
inline Module<T>::InterfaceRef Module<T>::operator*() const noexcept
{
    return *m_Interface;
}

template <ModuleInterfaceT T>
inline Module<T>::InterfacePtr Module<T>::operator->() const noexcept
{
    return m_Interface;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>& Module<T>::operator=(Module<U> const& other) noexcept
{
    m_Handle = other.m_Handle;
    m_Interface = static_cast<T>(other.m_Interface);
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>& Module<T>::operator=(Module<U>&& other) noexcept
{
    m_Handle = MoveArg(other.m_Handle);
    m_Interface = static_cast<T>(other.m_Interface);
    other.m_Interface = nullptr;
}

template <ModuleInterfaceT T>
inline Module<T>::Interface* Module<T>::QueryInterface() const noexcept
{
    if (m_Handle.IsValid() == false) [[unlikely]] {
        return nullptr;
    }

    return dynamic_cast<Interface*>(m_Handle->GetInterface());
}

template <ModuleInterfaceT T, ModuleInterfaceT U>
inline bool operator==(Module<T> const& l, Module<U> const& r) noexcept
{
    return l.Equals(r);
}

} // namespace mini