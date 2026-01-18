export module mini.platform:window;

import mini.core;

namespace mini::platform {

export class PLATFORM_API Window {
public:
    virtual ~Window() noexcept = default;

    virtual bool Valid() const noexcept = 0;

    virtual void Resize(RectInt const&) = 0;
    virtual void Minimize() = 0;
    virtual void Maximize() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;

    virtual RectInt GetSize() const = 0;
    virtual bool IsMinimized() const = 0;
    virtual bool IsMaximized() const = 0;

protected:
    Window() = default;
};

} // namespace mini::platform