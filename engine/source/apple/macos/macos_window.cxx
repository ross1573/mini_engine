module;

#include "cocoa_application.h"
#include "cocoa_window.h"

export module mini.macos:window;

import mini.core;
import mini.apple;
import mini.engine;

namespace mini::macos {

export class MACOS_API Window final
    : public apple::Window
    , private cocoa::Window {
public:
    Window(cocoa::Application*);

    void DialogCritical(String const& msg) final;

    void Resize(RectInt const& windowSize) final;
    inline void Minimize() final { cocoa::Window::Minimize(); }
    inline void Maximize() final { cocoa::Window::Maximize(); }
    inline void Show() final { cocoa::Window::Show(); }
    inline void Hide() final { cocoa::Window::Hide(); }

    inline RectInt GetSize() const final { return RectInt::zero; }
    inline bool IsMinimized() const final { return cocoa::Window::IsMinimized(); }
    inline bool IsMaximized() const final { return cocoa::Window::IsMaximized(); }
    inline bool IsFullScreen() const { return cocoa::Window::IsFullScreen(); }

    inline void SetFullScreen(bool active) final { cocoa::Window::SetFullScreen(active); }
    inline void SetMetalLayer(CA::MetalLayer* layer) final { cocoa::Window::SetMetalLayer(layer); }
    inline NSWindow* GetNSWindow() { return m_Window; }

private:
    inline bool ShouldClose() final { return true; }
    inline void WillClose() final { Engine::Quit(); }
};

} // namespace mini::macos