module;

#include "assertion.h"

module mini.d3d12;

import mini.core;
import :device;
import :render_context;
import :swap_chain;
import :command_queue;

namespace mini::d3d12
{

RenderContext::RenderContext()
    : m_CommandQueue(nullptr)
    , m_CurrentBuffer()
    , m_CommandAllocator(nullptr)
    , m_CommandList(nullptr)
{
}

RenderContext::~RenderContext()
{
    WaitForIdle();

    DELETE(m_CommandQueue);
}

bool RenderContext::Initialize()
{
    ID3D12Device* device = Graphics::GetDevice<d3d12::Device>()->GetD3D12Device();
    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    m_CommandQueue = new CommandQueue(graphics::CommandType::Direct);

    VERIFY(m_CommandQueue->Initialize(device));
    VERIFY(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator)));
    VERIFY(device->CreateCommandList(0, type, m_CommandAllocator,
                                     nullptr, IID_PPV_ARGS(&m_CommandList)));

    m_CommandList->Close();
    return true;
}

void RenderContext::BeginRender()
{
    m_CommandQueue->WaitForFence(m_CommandQueue->GetCurrentFence());
    m_CurrentBuffer = Graphics::GetSwapChain<d3d12::SwapChain>()->GetCurrentBuffer();

    VERIFY(m_CommandAllocator->Reset(), "failed to reset command allocator");
    VERIFY(m_CommandList->Reset(m_CommandAllocator, nullptr), "failed to reset command list");

    D3D12_RESOURCE_TRANSITION_BARRIER transition{};
    transition.pResource = m_CurrentBuffer->resource;
    transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition = transition;

    m_CommandList->ResourceBarrier(1, &barrier);
    m_CommandList->ClearRenderTargetView(m_CurrentBuffer->descriptor.offset,
                                         Color::clear.data, 0, nullptr);
}

void RenderContext::EndRender()
{
    D3D12_RESOURCE_TRANSITION_BARRIER transition{};
    transition.pResource = m_CurrentBuffer->resource;
    transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition = transition;

    m_CommandList->ResourceBarrier(1, &barrier);
}

void RenderContext::SetViewport(Rect const& rect, float minZ, float maxZ)
{
    D3D12_VIEWPORT d3dViewport{};
    d3dViewport.TopLeftX = rect.x;
    d3dViewport.TopLeftY = rect.y;
    d3dViewport.Width = rect.width;
    d3dViewport.Height = rect.height;
    d3dViewport.MinDepth = minZ;
    d3dViewport.MaxDepth = maxZ;

    m_CommandList->RSSetViewports(1, &d3dViewport);
}

void RenderContext::SetScissorRect(RectInt const& rect)
{
    m_CommandList->RSSetScissorRects(1, reinterpret_cast<D3D12_RECT const*>(&rect));
}

void RenderContext::WaitForIdle()
{
    m_CommandQueue->WaitForIdle();
}

void RenderContext::Execute()
{
    m_CommandList->Close();
    m_CommandQueue->ExecuteCommandList(m_CommandList);
}

} // namespace mini::d3d12