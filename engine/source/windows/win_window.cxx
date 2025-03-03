export module mini.windows:window;

import mini.core;
import mini.platform;

export namespace mini::windows
{

class WINDOWS_API Window : public platform::Window
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

    void DialogCritical(String const&) final;

    void Resize(RectInt const&) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    inline RectInt GetSize() const final { return m_Rect; }
    inline HWND GetHWND() const { return m_WindowHandle; }

protected:
    bool Initialize() final;
};

} // namespace mini::windows