module;

export module mini.windows:handle;

import mini.platform;
import mini.graphics;
import :window;

export namespace mini::windows
{

class Handle : public platform::Handle
{
private:
    HINSTANCE m_Instance;

public:
    Handle();
    ~Handle() final;

    bool Initialize() final;
    void PollEvents() final;

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;

    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    HINSTANCE GetHINSTANCE() const { return m_Instance; }
};

} // namespace mini::windows