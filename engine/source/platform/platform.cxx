export module mini.platform;

import mini.core;

export import :window;
export import :handle;

namespace mini::platform {

export class PLATFORM_API Interface final : public Module::Interface {
private:
    Module m_NativeModule;
    UniquePtr<Handle> m_Handle;
    UniquePtr<Window> m_Window;

public:
    Interface() noexcept;
    ~Interface() noexcept final;

    void PollEvents();

    Handle* GetHandle() const noexcept { return m_Handle.Get(); }
    Window* GetWindow() const noexcept { return m_Window.Get(); }

private:
    bool Initialize() final;
};

PLATFORM_API Interface* interface = nullptr;

} // namespace mini::platform

namespace mini {

export class PLATFORM_API Platform : public Module::Interface {
public:
    typedef platform::Handle Handle;
    typedef platform::Window Window;

private:
    friend class platform::Interface;

public:
    virtual ~Platform() override = default;

    virtual Handle* CreateHandle() = 0;
    virtual Window* CreateWindow() = 0;

    static void AlertError(StringView const&);

protected:
    Platform() noexcept = default;

    platform::Interface* Get() const noexcept;
};

} // namespace mini