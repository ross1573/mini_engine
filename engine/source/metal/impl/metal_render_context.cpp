module;

#include <Metal/Metal.hpp>

module mini.metal;

import mini.core;
import :render_context;

namespace mini::metal {

RenderContext::RenderContext()
    : m_CmdQueue(nullptr)
{
}

RenderContext::~RenderContext()
{
    if (m_CmdQueue) {
        m_CmdQueue->release();
    }
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