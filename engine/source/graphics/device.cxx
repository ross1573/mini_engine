export module mini.graphics:device;

import mini.core;
import :common;
import :render_context;
import :swap_chain;

namespace mini { class Graphics; }

export namespace mini::graphics
{

class GRAPHICS_API Device
{
    friend class mini::Graphics;

public:
    virtual ~Device() = default;

    virtual API GetAPI() const = 0;

protected:
    Device() = default;

    virtual bool Initialize() = 0;

    virtual SwapChain* CreateSwapChain() = 0;
    virtual RenderContext* CreateRenderContext() = 0;
};

} // namespace mini::graphics