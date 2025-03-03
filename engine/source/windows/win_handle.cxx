export module mini.windows:handle;

import mini.platform;
import mini.graphics;
import :window;

export namespace mini::windows
{

class WINDOWS_API Handle : public platform::Handle
{
private:
    HINSTANCE m_Instance;

public:
    Handle(HINSTANCE);
    ~Handle() final;

    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    inline HINSTANCE GetHINSTANCE() const { return m_Instance; }

protected:
    bool Initialize() final;
    bool PollEvents() final;

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;
};

} // namespace mini::windows