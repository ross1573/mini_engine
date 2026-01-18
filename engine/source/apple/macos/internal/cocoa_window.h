#ifndef COCOA_WINDOW_H
#define COCOA_WINDOW_H

#include <QuartzCore/CAMetalLayer.hpp>

#include "appkit.h"

namespace mini::cocoa {

class Window {
protected:
    NSWindow* m_Window;
    NSView* m_View;

public:
    Window(Application*);
    virtual ~Window();

    bool Valid() const noexcept;

    void Show();
    void Hide();
    void Minimize();
    void Maximize();

    bool IsMinimized() const;
    bool IsMaximized() const;
    bool IsFullScreen() const;

    void SetFullScreen(bool);
    void SetMetalLayer(CA::MetalLayer*);

    virtual bool ShouldClose() = 0;
    virtual void WillClose() = 0;
};

inline bool Window::Valid() const noexcept
{
    return m_Window != nullptr;
}

} // namespace mini::cocoa

#endif // COCOA_WINDOW_H