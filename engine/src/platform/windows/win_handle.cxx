module;

#include <windows.h>

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

    void ProcessMessage(HWND, uint32, WPARAM, LPARAM);

    inline HINSTANCE GetHINSTANCE() const { return m_Instance; }

protected:
    bool Initialize() final;
    void PollEvents() final;

    platform::Window* CreatePlatformWindow() final;
    graphics::Device* CreateGraphicDevice(graphics::API) final;
};

} // namespace mini::windows