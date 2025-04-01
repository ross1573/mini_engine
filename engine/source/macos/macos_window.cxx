module;

#include "appkit.h"
#include "cocoa_window.h"

export module mini.macos:window;

import mini.core;
import mini.platform;

namespace mini::macos {

export class MACOS_API Window
    : public platform::Window
    , public cocoa::Window {
private:
    NSWindow* m_Window;

public:
    Window();
    ~Window();

    bool Initialize() final;

    void DialogCritical(String const& msg) final;

    void Resize(RectInt const& windowSize) final;
    void Minimize() final;
    void Maximize() final;
    void Show() final;
    void Hide() final;

    inline RectInt GetSize() const final { return RectInt::zero; }

    inline NSWindow* GetNSWindow() { return m_Window; }
};

} // namespace mini::macos