#pragma once

#include "appkit.h"
#include "quartzcore.h"

namespace mini::cocoa {

class Window {
protected:
    NSWindow* m_Window;
    NSView* m_View;

public:
    Window(Application*);
    ~Window();

    void AlertError(char const*);
    void Show();
    void Hide();

    void SetMetalLayer(CAMetalLayer*);

    virtual bool ShouldClose() = 0;
    virtual void WillClose() = 0;
};

} // namespace mini::cocoa