export module mini.platform:modules;

import mini.core;

namespace mini::platform {

export class PLATFORM_API Module {
public:
    virtual ~Module() = default;

    template <typename Ret, typename... Args, typename FuncT = Ret (*)(Args...)>
    FuncT GetFunction(StringView);

    virtual StringView GetName() const = 0;

protected:
    Module() = default;

    virtual void* LoadFunc(StringView) = 0;
};

template <typename RetT, typename... Args, typename FuncT = RetT (*)(Args...)>
inline FuncT Module::GetFunction(StringView name)
{
    void* func = LoadFunc(name);
    ENSURE(func, Format("failed to load function {0}", name).Data()) {
        return nullptr;
    }

    return reinterpret_cast<FuncT>(func);
}

} // namespace mini::platform