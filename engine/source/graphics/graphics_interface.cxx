export module mini.graphics:interface;

import mini.core;
import :device;

namespace mini {

export class Graphics;

}; // namespace mini

namespace mini::graphics {

GRAPHICS_API Graphics* interface = nullptr;

export class GRAPHICS_API Interface : public ModuleInterface {
protected:
    Graphics* m_Graphics;

public:
    virtual Device* CreateDevice() = 0;

protected:
    Interface() noexcept { m_Graphics = interface; }
    virtual ~Interface() noexcept { m_Graphics = nullptr; }
};

} // namespace mini::graphics