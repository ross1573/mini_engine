#pragma once

#include "appkit.h"

namespace mini::cocoa {

class Window {
protected:
    NSWindow* m_Window;

public:
    Window();

    void Show();
    void Hide();
};

} // namespace mini::cocoa