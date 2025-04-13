module;

#include <Foundation/NSAutoreleasePool.hpp>
#include <Metal/MTLCommandQueue.hpp>

export module mini.metal:render_context;

import mini.core;
import mini.graphics;

namespace mini::metal {

export class METAL_API RenderContext final : public graphics::RenderContext {
private:
    MTL::CommandQueue* m_CmdQueue;

public:
    RenderContext(MTL::Device*);
    ~RenderContext() final;

    bool Initialize() final;

    void BeginRender() final;
    void EndRender() final;
    void WaitForIdle() final;
    void Execute() final;

    void SetViewport(Rect const&, float, float) final;
    void SetScissorRect(RectInt const&) final;
};

} // namespace mini::metal