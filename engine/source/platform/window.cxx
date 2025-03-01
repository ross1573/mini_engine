export module mini.platform:window;

namespace mini { class Platform; }

import mini.core;

export namespace mini::platform
{

class Window
{
    friend class mini::Platform;

public:
    virtual ~Window() = default;

    virtual void DialogCritical(String const&) = 0;

    virtual void Resize(RectInt const&) = 0;
    virtual void Minimize() = 0;
    virtual void Maximize() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;

    virtual RectInt GetSize() const = 0;

protected:
    Window() = default;

    virtual bool Initialize() = 0;
};

} // namespace mini::platform