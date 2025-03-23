export module mini.windows:handle;

import mini.platform;
import mini.graphics;
import :window;

namespace mini::windows {

export class WINDOWS_API Handle final : public platform::Handle {
private:
    HINSTANCE m_Instance;

public:
    Handle(HINSTANCE);
    ~Handle() final;

    bool Initialize() final;

    void PollEvents() final;

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;

    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    inline HINSTANCE GetHINSTANCE() const { return m_Instance; }
};

} // namespace mini::windows