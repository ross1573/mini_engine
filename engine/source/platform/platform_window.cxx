export module mini.platform:window;

import mini.core;

namespace mini::platform {

export class PLATFORM_API Window {
public:
    virtual ~Window() noexcept = default;

    virtual void resize(RectInt const&) = 0;
    virtual void minimize() = 0;
    virtual void maximize() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;

    [[nodiscard]] virtual bool valid() const noexcept = 0;
    [[nodiscard]] virtual bool minimized() const = 0;
    [[nodiscard]] virtual bool maximized() const = 0;
    [[nodiscard]] virtual RectInt size() const = 0;

protected:
    Window() noexcept = default;
};

} // namespace mini::platform