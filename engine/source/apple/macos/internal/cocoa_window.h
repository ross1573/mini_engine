#ifndef COCOA_WINDOW_H
#define COCOA_WINDOW_H

#include <QuartzCore/CAMetalLayer.hpp>

#include "cocoa_application.h"

namespace mini::cocoa {

class Window {
protected:
    NSWindow* m_window;
    NSView* m_view;

public:
    Window(Application*);
    virtual ~Window();

    bool valid() const noexcept { return m_window != nullptr; }

    void show();
    void hide();
    void minimize();
    void maximize();

    [[nodiscard]] bool minimized() const;
    [[nodiscard]] bool maximized() const;
    [[nodiscard]] bool fullScreen() const;

    void setFullScreen(bool);
    void setMetalLayer(CA::MetalLayer*);

    virtual bool shouldClose() = 0;
    virtual void willClose() = 0;
};

} // namespace mini::cocoa

#endif // COCOA_WINDOW_H