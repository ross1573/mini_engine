export module mini.windows:window;

import mini.core;
import mini.platform;

namespace mini::windows {

enum WindowState : uint8 {
    Default,
    Minimized,
    Maximized,
};

export class WINDOWS_API Window final : public platform::Window {
private:
    HWND m_WindowHandle;

    RectInt m_Rect;
    uint32 width;
    uint32 height;

    bool m_IsShowing;
    WindowState m_State;

public:
    Window();
    ~Window() noexcept = default;

    bool IsValid() const noexcept final;

    void Resize(RectInt const&) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    RectInt GetSize() const final { return m_Rect; }
    bool IsMinimized() const final { return m_State == Minimized; }
    bool IsMaximized() const final { return m_State == Maximized; }

    HWND GetHWND() const { return m_WindowHandle; }
};

} // namespace mini::windows