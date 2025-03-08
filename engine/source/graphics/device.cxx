export module mini.graphics:device;

import mini.core;
import :common;
import :render_context;
import :swap_chain;

namespace mini::graphics
{

export class GRAPHICS_API Device
{
public:
    virtual ~Device() = default;

    virtual API GetAPI() const = 0;

public:
    Device() = default;

    virtual bool Initialize() = 0;

    virtual SwapChain* CreateSwapChain() = 0;
    virtual RenderContext* CreateRenderContext() = 0;
};

} // namespace mini::graphics