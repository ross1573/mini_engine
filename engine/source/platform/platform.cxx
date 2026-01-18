export module mini.platform;

import mini.core;

export import :interface;
export import :window;
export import :handle;

namespace mini {

export class PLATFORM_API Platform final : public ModuleInterface {
public:
    typedef platform::Interface Interface;
    typedef platform::Handle Handle;
    typedef platform::Window Window;

private:
    Module<Interface> m_nativeModule;
    UniquePtr<Handle> m_handle;
    UniquePtr<Window> m_window;

public:
    Platform() noexcept;
    ~Platform() noexcept;

    void PollEvents();

    Handle* GetHandle() const noexcept { return m_handle.Get(); }
    Window* GetWindow() const noexcept { return m_window.Get(); }

    static void AlertError(StringView const&);

protected:
    bool Initialize() final;
};

} // namespace mini