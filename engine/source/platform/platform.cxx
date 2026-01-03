export module mini.platform;

import mini.core;

export import :window;
export import :handle;

namespace mini::platform {

export class PLATFORM_API Interface final : public Module::Interface {
public:
    virtual ~Interface() final;

private:
    virtual bool Initialize() override;
};

} // namespace mini::platform

namespace mini {

PLATFORM_API UniquePtr<platform::Handle> g_Handle;
PLATFORM_API UniquePtr<platform::Window> g_Window;

export class PLATFORM_API Platform : public Module::Interface {
public:
    typedef platform::Handle Handle;
    typedef platform::Window Window;

private:
    friend class platform::Interface;

    Module m_NativeModule;

public:
    virtual ~Platform() override = default;

    static Handle* GetHandle() noexcept { return g_Handle.Get(); }
    static Window* GetWindow() noexcept { return g_Window.Get(); }

protected:
    Platform() noexcept = default;

    virtual Handle* CreateHandle() = 0;
    virtual Window* CreateWindow() = 0;
};

} // namespace mini