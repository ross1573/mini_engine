export module mini.platform:window;

import mini.math;
import mini.container;

export namespace mini::platform
{

class Window
{
protected:
    Window() = default;

public:
    virtual ~Window() = default;

    virtual bool Initialize() = 0;

    virtual void DialogCritical(String const&) = 0;

    virtual void Resize(RectInt const&) = 0;
    virtual void Minimize() = 0;
    virtual void Maximize() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;

    virtual RectInt GetSize() const = 0;
};

} // namespace mini::platform