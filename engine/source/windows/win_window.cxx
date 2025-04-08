export module mini.windows:window;

import mini.core;
import mini.platform;

namespace mini::windows {

export class WINDOWS_API Window final : public platform::Window {
private:
    HWND m_WindowHandle;

    RectInt m_Rect;
    uint32 width;
    uint32 height;

    bool m_IsShowing;

public:
    Window();
    ~Window() = default;

    void DialogCritical(String const&) final;

    void Resize(RectInt const&) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    inline RectInt GetSize() const final { return m_Rect; }
    inline bool IsMinimized() const final { return false; } // TODO
    inline bool IsMaximized() const final { return false; } // TODO

    inline HWND GetHWND() const { return m_WindowHandle; }
};

} // namespace mini::windows