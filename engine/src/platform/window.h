#pragma once

#include "container/string.h"

namespace mini { struct RectInt; }

namespace mini::platform
{

class Window
{
    friend class mini::Platform;

protected:
    Window() = default;

    virtual bool Initialize() = 0;

public:
    virtual ~Window() = default;

    virtual void DialogCritical(String const&) = 0;

    virtual void Resize(RectInt const&) = 0;
    virtual void Minimize() = 0;
    virtual void Maximize() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;

    //virtual RectInt GetSize() const = 0;
};

} // namespace mini::platform