export module mini.macos:window;

import mini.core;
import mini.apple;
import mini.engine;

namespace mini::macos {

export class MACOS_API Window final
    : private cocoa::Window
    , public apple::Window {
private:
    typedef cocoa::Window Base;

public:
    Window(cocoa::Application*);
    ~Window() noexcept = default;

    bool Valid() const noexcept final { return Base::Valid(); }

    void Resize(RectInt const& windowSize) final;
    void Minimize() final { Base::Minimize(); }
    void Maximize() final { Base::Maximize(); }
    void Show() final { Base::Show(); }
    void Hide() final { Base::Hide(); }

    bool IsMinimized() const final { return Base::IsMinimized(); }
    bool IsMaximized() const final { return Base::IsMaximized(); }
    bool IsFullScreen() const { return Base::IsFullScreen(); }

    void SetFullScreen(bool active) final { Base::SetFullScreen(active); }
    void SetMetalLayer(CA::MetalLayer* layer) final { Base::SetMetalLayer(layer); }
    RectInt GetSize() const final { return RectInt::Zero(); }
    NSWindow* GetNSWindow() { return m_Window; }

private:
    bool ShouldClose() final { return true; }
    void WillClose() final { Engine::Quit(); }
};

} // namespace mini::macos