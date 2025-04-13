module;

#include <Metal/MTLCommandBuffer.hpp>
#include <Metal/MTLRenderCommandEncoder.hpp>
#include <Metal/MTLRenderPass.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "assertion.h"

module mini.metal;

import mini.core;
import mini.graphics;
import mini.apple;
import :render_context;

namespace mini::metal {

RenderContext::RenderContext(MTL::Device* device)
    : m_CmdQueue(nullptr)
{
    m_CmdQueue = TransferShared(device->newCommandQueue());
    ASSERT(m_CmdQueue, "failed to create MTLCommandQueue");
}

bool RenderContext::Initialize()
{
    return true;
}

void RenderContext::BeginRender()
{
}

void RenderContext::EndRender()
{
}

void RenderContext::WaitForIdle()
{
}

void RenderContext::Execute()
{
}

void RenderContext::SetViewport(Rect const&, float, float)
{
}

void RenderContext::SetScissorRect(RectInt const&)
{
}

} // namespace mini::metal