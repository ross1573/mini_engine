#pragma once

#include "platform/handle.h"
#include "graphics/graphics.h"

namespace mini::platform { class Window; }
namespace mini::graphics { class Device; }

namespace mini::windows
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