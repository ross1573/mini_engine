export module mini.windows:window;

import mini.core;
import mini.math;
import mini.platform;
import mini.container;
import :api;

export namespace mini::windows
{

class Window : public platform::Window
{
private:
    HWND m_WindowHandle;
    
    RectInt m_Rect;
    uint32 width;
    uint32 height;

    bool m_IsShowing;

public:
    Window();
    ~Window() = default;

    bool Initialize() final;

    void DialogCritical(String const&) final;

    void Resize(RectInt const&) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    RectInt GetSize() const final { return m_Rect; }
    HWND GetHWND() const { return m_WindowHandle; }
};

} // namespace mini::windows