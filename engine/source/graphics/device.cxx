export module mini.graphics:device;

import mini.core;
import :common;
import :render_context;
import :swap_chain;

namespace mini::graphics {

export class GRAPHICS_API Device {
public:
    virtual ~Device() = default;

    virtual bool Initialize() = 0;

    virtual SwapChain* CreateSwapChain() = 0;
    virtual RenderContext* CreateRenderContext() = 0;

    virtual API GetAPI() const = 0;

protected:
    Device() = default;
};

} // namespace mini::graphics