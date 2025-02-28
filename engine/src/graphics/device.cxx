export module mini.graphics:device;

import mini.core;
import :common;
import :render_context;
import :swap_chain;

export namespace mini::graphics
{

class Device
{
    friend class Graphics;

protected:
    Device() = default;

public:
    virtual bool Initialize() = 0;

    virtual SwapChain* CreateSwapChain() = 0;
    virtual RenderContext* CreateRenderContext() = 0;

public:
    virtual ~Device() = default;

    virtual API GetAPI() const = 0;
};

} // namespace mini::graphics