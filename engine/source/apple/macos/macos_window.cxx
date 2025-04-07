module;

#include "cocoa_application.h"
#include "cocoa_window.h"

export module mini.macos:window;

import mini.core;
import mini.apple;

namespace mini::macos {

export class MACOS_API Window final
    : public apple::Window
    , private cocoa::Window {
public:
    Window(cocoa::Application*);

    void DialogCritical(String const& msg) final;

    void Resize(RectInt const& windowSize) final;
    void Minimize() final;
    void Maximize() final;
    inline void Show() final { cocoa::Window::Show(); }
    inline void Hide() final { cocoa::Window::Hide(); }

    inline RectInt GetSize() const final { return RectInt::zero; }

    inline void SetFullScreen(bool active) final { cocoa::Window::SetFullScreen(active); }
    inline void SetMetalLayer(CAMetalLayer* layer) final { cocoa::Window::SetMetalLayer(layer); }
    inline NSWindow* GetNSWindow() { return m_Window; }

private:
    bool ShouldClose() final;
    void WillClose() final;
};

} // namespace mini::macos