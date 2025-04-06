#pragma once

#include "appkit.h"
#include "quartzcore.h"

namespace mini::cocoa {

class Window {
protected:
    NSWindow* m_Window;
    NSView* m_View;

public:
    Window();
    ~Window();

    void Show();
    void Hide();

    void SetMetalLayer(CAMetalLayer*);
};

} // namespace mini::cocoa