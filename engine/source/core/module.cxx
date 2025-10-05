export module mini.core:module_base;

import :string_view;
import :string;
import :shared_ptr;
import :format;

namespace mini {

template <typename T>
struct ModuleFuncResult {
public:
    typedef T Value;
    typedef T* Ptr;
    typedef T& Ref;
    typedef T const ConstValue;
    typedef T const* ConstPtr;
    typedef T const& ConstRef;

private:
    union {
        byte m_Empty;
        Value m_Value;
    };
    bool m_HasValue;

public:
    constexpr ModuleFuncResult() noexcept;
    constexpr ModuleFuncResult(T const&);
    constexpr ModuleFuncResult(T&&);

    constexpr bool HasValue() const noexcept;
    constexpr Ptr GetValue() noexcept;
    constexpr ConstPtr GetValue() const noexcept;

    constexpr ModuleFuncResult& operator=(T const&);
    constexpr ModuleFuncResult& operator=(T&&);
};

template <typename T>
inline constexpr ModuleFuncResult<T>::ModuleFuncResult() noexcept
    : m_HasValue(false)
{
    memory::BeginLifetime(&m_Empty);
}

template <typename T>
inline constexpr ModuleFuncResult<T>::ModuleFuncResult(T const& value)
    : m_HasValue(true)
{
    memory::ConstructAt(&m_Value, value);
}

template <typename T>
inline constexpr ModuleFuncResult<T>::ModuleFuncResult(T&& value)
    : m_HasValue(true)
{
    memory::ConstructAt(&m_Value, MoveArg(value));
}

template <typename T>
inline constexpr bool ModuleFuncResult<T>::HasValue() const noexcept
{
    return m_HasValue;
}

template <typename T>
inline constexpr ModuleFuncResult<T>::Ptr ModuleFuncResult<T>::GetValue() noexcept
{
    return m_HasValue ? &m_Value : Ptr(nullptr);
}

template <typename T>
inline constexpr ModuleFuncResult<T>::ConstPtr ModuleFuncResult<T>::GetValue() const noexcept
{
    return m_HasValue ? &m_Value : ConstPtr(nullptr);
}

template <typename T>
inline constexpr ModuleFuncResult<T>& ModuleFuncResult<T>::operator=(T const& value)
{
    if (m_HasValue) {
        m_Value = value;
        return;
    }

    memory::EndLifetime(&m_Empty);
    memory::ConstructAt(&m_Value, value);
}

template <typename T>
inline constexpr ModuleFuncResult<T>& ModuleFuncResult<T>::operator=(T&& value)
{
    if (m_HasValue) {
        m_Value = MoveArg(value);
        return;
    }

    memory::EndLifetime(&m_Empty);
    memory::ConstructAt(&m_Value, MoveArg(value));
}

template <typename T>
concept ModuleHandleT = ConstructibleFromT<StringView> && requires(T handle) {
    { handle.IsValid() } -> ConvertibleToT<bool>;
    { handle.GetName() } -> ConvertibleToT<String>;
    { handle.LoadFunc(StringView::Empty) } -> SameAsT<void*>;
};

template <ModuleHandleT T>
class BasicModule {
public:
    static SharedPtr<BasicModule> LoadModule(StringView name);

private:
    T m_Handle;

public:
    BasicModule(T&&);
    virtual ~BasicModule() = default;

    virtual bool Initialize() { return true; };
    virtual void Shutdown() {};

    T const& GetHandle() const;
    String GetName() const;

    template <typename Ret, typename... Args, typename FuncT = Ret (*)(Args...)>
    FuncT GetFunction(StringView);

    template <typename RetT, typename... Args>
    ModuleFuncResult<RetT> CallFunction(StringView, Args&&...);

private:
    BasicModule(BasicModule const&) = delete;
    BasicModule(BasicModule&&) = delete;

    BasicModule& operator=(BasicModule const&) = delete;
    BasicModule& operator=(BasicModule&&) = delete;
};

template <ModuleHandleT T>
inline SharedPtr<BasicModule<T>> BasicModule<T>::LoadModule(StringView name)
{
    SharedPtr<BasicModule> mod = MakeShared<BasicModule>(name);
    if (mod->GetHandle().IsValid() == false) {
        mod.Reset();
    }

    return mod;
}

template <ModuleHandleT T>
inline BasicModule<T>::BasicModule(T&& handle)
    : m_Handle(MoveArg(handle))
{
}

template <ModuleHandleT T>
inline T const& BasicModule<T>::GetHandle() const
{
    return m_Handle;
}

template <ModuleHandleT T>
inline String BasicModule<T>::GetName() const
{
    return m_Handle.GetName();
}

template <ModuleHandleT T>
template <typename RetT, typename... Args, typename FuncT>
inline FuncT BasicModule<T>::GetFunction(StringView name)
{
    void* func = m_Handle.LoadFunc(name);
    if (func == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<FuncT>(func);
}

template <ModuleHandleT T>
template <typename RetT, typename... Args>
inline ModuleFuncResult<RetT> BasicModule<T>::CallFunction(StringView name, Args&&... args)
{
    using FuncT = RetT (*)(Args...);

    FuncT func = GetFunction<RetT, Args...>(name);
    if (func == nullptr) {
        return ModuleFuncResult<RetT>();
    }

    return ModuleFuncResult(func(ForwardArg<Args>(args)...));
}

} // namespace mini