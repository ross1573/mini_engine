module;

#include "cocoa_window.h"

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

    void resize(RectInt const& windowSize) final;
    void minimize() final { Base::minimize(); }
    void maximize() final { Base::maximize(); }
    void show() final { Base::show(); }
    void hide() final { Base::hide(); }
    void setFullScreen(bool active) final { Base::setFullScreen(active); }
    void setMetalLayer(CA::MetalLayer* layer) final { Base::setMetalLayer(layer); }

    [[nodiscard]] bool valid() const noexcept final { return Base::valid(); }
    [[nodiscard]] bool minimized() const final { return Base::minimized(); }
    [[nodiscard]] bool maximized() const final { return Base::maximized(); }
    [[nodiscard]] bool fullScreen() const final { return Base::fullScreen(); }

    [[nodiscard]] RectInt size() const final { return RectInt::zero(); }
    [[nodiscard]] NSWindow* nsWindow() { return m_window; }

private:
    bool shouldClose() final { return true; }
    void willClose() final { Engine::quit(); }
};

} // namespace mini::macos