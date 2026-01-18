export module mini.core:module_system;

import :type;
import :utility_operation;
import :memory_operation;
import :array;
import :string;
import :string_view;
import :deleter;
import :unique_ptr;
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

struct CORE_API ModulePoilcy {
public:
    typedef void (*CallbackFunc)();
    typedef bool (*ValidFunc)(NativeModuleHandle);
    typedef void (*DeleteFunc)(NativeModuleHandle);

    ValidFunc validator;
    DeleteFunc deleter;
};

class CORE_API ModuleHandle {
public:
    typedef NativeModuleHandle NativeModule;
    typedef typename ModulePoilcy::CallbackFunc CallbackFunc;
    typedef typename ModulePoilcy::ValidFunc ValidFunc;
    typedef typename ModulePoilcy::DeleteFunc DeleteFunc;

protected:
    ModulePoilcy const* m_policy;
    NativeModule m_nativeModule;
    UniquePtr<ModuleInterface> m_interface;
    String m_libraryName;
    Array<CallbackFunc> m_exitCallback;

public:
    ModuleHandle(ModulePoilcy const*, NativeModule, ModuleInterface*, StringView) noexcept;
    ModuleHandle(ModuleHandle&&) noexcept = default;
    ~ModuleHandle() noexcept;

    bool Valid() const noexcept;

    bool AtExit(CallbackFunc) noexcept;
    bool RemoveAtExit(CallbackFunc) noexcept;

    String LibraryName() const noexcept;
    NativeModule NativeHandle() noexcept;
    ModuleInterface* GetInterface() const noexcept;

    ModuleHandle& operator=(ModuleHandle&&) noexcept = default;

    static SharedPtr<ModuleHandle> Load(StringView);
};

class DynamicModuleHandle : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeModule NativeModule;
    typedef ModuleInterface* (*StartFunc)();

    inline static constexpr ModulePoilcy policy = ModulePoilcy{
        .validator = [](NativeModule nativeModule) { return nativeModule != nullptr; },
        .deleter = UnloadModule
    };

public:
    DynamicModuleHandle(StringView name) noexcept
        : ModuleHandle(&policy, nullptr, nullptr, name)
    {
        String path = BuildModulePath(name);
        m_nativeModule = LoadModule(path);
        if (m_nativeModule == nullptr) {
            return;
        }

        StartFunc startFunc = GetFunction<ModuleInterface*>("__start_module");
        if (startFunc == nullptr) {
            return;
        }

        ModuleInterface* interface = nullptr;
        try {
            interface = startFunc();
        } catch (...) {
        }

        if (interface == nullptr) {
            UnloadModule(m_nativeModule);
            m_nativeModule = nullptr;
            return;
        }

        m_interface = UniquePtr(interface);
    }

    template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
    FuncT GetFunction(StringView name)
    {
        void* funcPtr = LoadFunction(m_nativeModule, name);
        return reinterpret_cast<FuncT>(funcPtr);
    }
};

class StaticModuleHandle : public ModuleHandle {
public:
    typedef typename ModuleHandle::NativeModule NativeModule;

    inline static NativeModule programHandle = LoadMainProgram();
    inline static constexpr ModulePoilcy policy =
        ModulePoilcy{ .validator = nullptr, .deleter = nullptr };

public:
    StaticModuleHandle(StringView name, ModuleInterface* interface) noexcept
        : ModuleHandle(&policy, programHandle, interface, name)
    {
    }
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
    SharedPtr<Handle> m_handle;
    Interface* m_interface;

public:
    Module() noexcept;
    ~Module() noexcept;
    Module(Module const&) noexcept = default;
    Module(Module&&) noexcept = default;

    explicit Module(StringView);
    template <UnboundAllocatorT AllocT>
    Module(StringView, AllocT const&);
    template <RelatedInterfaceToT<T> U>
    Module(Module<U> const&) noexcept;
    template <RelatedInterfaceToT<T> U>
    Module(Module<U>&&) noexcept;

    bool Valid() const noexcept;

    template <UnboundAllocatorT AllocT>
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
    : m_handle()
    , m_interface(nullptr)
{
}

template <ModuleInterfaceT T>
inline Module<T>::Module(StringView name)
    : m_handle(Handle::Load(name))
    , m_interface(QueryInterface())
{
    if (m_interface == nullptr) [[unlikely]] {
        m_handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline Module<T>::~Module() noexcept
{
    m_interface = nullptr;
}

template <ModuleInterfaceT T>
template <UnboundAllocatorT AllocT>
inline Module<T>::Module(StringView name, AllocT const& alloc)
    : m_handle(Handle::Load(name), alloc)
    , m_interface(QueryInterface())
{
    if (m_interface == nullptr) [[unlikely]] {
        m_handle.Reset();
    }
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>::Module(Module<U> const& other) noexcept
    : m_handle(other.m_handle)
    , m_interface(static_cast<U>(other.m_interface))
{
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>::Module(Module<U>&& other) noexcept
    : m_handle(MoveArg(other.m_handle))
{
    m_interface = static_cast<U>(other.m_interface);
    other.m_interface = nullptr;
}

template <ModuleInterfaceT T>
inline bool Module<T>::Valid() const noexcept
{
    return m_handle != nullptr;
}

template <ModuleInterfaceT T>
template <UnboundAllocatorT AllocT>
inline void Module<T>::Load(StringView name, AllocT const& alloc)
{
    m_handle.Reset(Handle::Load(name), DefaultDeleter<Handle>{}, alloc);
    m_interface = QueryInterface();

    if (m_interface == nullptr) [[unlikely]] {
        m_handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline void Module<T>::Load(StringView name)
{
    m_handle = Handle::Load(name);
    m_interface = QueryInterface();

    if (m_interface == nullptr) [[unlikely]] {
        m_handle.Reset();
    }
}

template <ModuleInterfaceT T>
inline void Module<T>::Release() noexcept
{
    m_handle.Reset();
    m_interface = nullptr;
}

template <ModuleInterfaceT T>
inline bool Module<T>::AtExit(CallbackFunc func) noexcept
{
    return m_handle != nullptr ? m_handle->AtExit(func) : false;
}

template <ModuleInterfaceT T>
inline bool Module<T>::RemoveAtExit(CallbackFunc func) noexcept
{
    return m_handle != nullptr ? m_handle->RemoveAtExit(func) : false;
}

template <ModuleInterfaceT T>
inline String Module<T>::LibraryName() const noexcept
{
    return m_handle != nullptr ? m_handle->LibraryName() : String();
}

template <ModuleInterfaceT T>
inline Module<T>::InterfacePtr Module<T>::GetInterface() const noexcept
{
    return m_interface;
}

template <ModuleInterfaceT T>
inline Module<T>::NativeModule Module<T>::NativeHandle() const noexcept
{
    return m_handle != nullptr ? m_handle->NativeHandle() : nullptr;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
bool Module<T>::Equals(Module<U> const& other) const noexcept
{
    return m_handle.OwnerEquals(other) && m_interface == other.m_interface;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
bool Module<T>::HandleEquals(Module<U> const& other) const noexcept
{
    return m_handle.OwnerEquals(other);
}

template <ModuleInterfaceT T>
inline Module<T>::InterfaceRef Module<T>::operator*() const noexcept
{
    return *m_interface;
}

template <ModuleInterfaceT T>
inline Module<T>::InterfacePtr Module<T>::operator->() const noexcept
{
    return m_interface;
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>& Module<T>::operator=(Module<U> const& other) noexcept
{
    m_handle = other.m_handle;
    m_interface = static_cast<T>(other.m_interface);
}

template <ModuleInterfaceT T>
template <RelatedInterfaceToT<T> U>
inline Module<T>& Module<T>::operator=(Module<U>&& other) noexcept
{
    m_handle = MoveArg(other.m_handle);
    m_interface = static_cast<T>(other.m_interface);
    other.m_interface = nullptr;
}

template <ModuleInterfaceT T>
inline Module<T>::Interface* Module<T>::QueryInterface() const noexcept
{
    if (m_handle.Valid() == false) [[unlikely]] {
        return nullptr;
    }

    return dynamic_cast<Interface*>(m_handle->GetInterface());
}

template <ModuleInterfaceT T, ModuleInterfaceT U>
inline bool operator==(Module<T> const& l, Module<U> const& r) noexcept
{
    return l.Equals(r);
}

} // namespace mini