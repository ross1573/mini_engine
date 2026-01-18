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
    HWND m_windowHandle;

    RectInt m_rect;
    uint32 width;
    uint32 height;

    bool m_isShowing;
    WindowState m_state;

public:
    Window();
    ~Window() noexcept = default;

    bool Valid() const noexcept final;

    void Resize(RectInt const&) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    RectInt GetSize() const final { return m_rect; }
    bool IsMinimized() const final { return m_state == Minimized; }
    bool IsMaximized() const final { return m_state == Maximized; }

    HWND GetHWND() const { return m_windowHandle; }
};

} // namespace mini::windows