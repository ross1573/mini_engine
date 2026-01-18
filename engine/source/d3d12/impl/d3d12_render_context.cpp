module mini.d3d12;

import mini.core;
import :device;
import :render_context;
import :swap_chain;
import :command_queue;

namespace mini::d3d12 {

RenderContext::RenderContext(ID3D12Device* device)
    : m_device(device)
    , m_commandQueue(nullptr)
    , m_currentBuffer()
    , m_commandAllocator(nullptr)
    , m_commandList(nullptr)
{
}

RenderContext::~RenderContext()
{
    WaitForIdle();
}

bool RenderContext::Initialize()
{
    ID3D12Device* device = interface->GetDevice()->GetD3D12Device();
    D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    m_commandQueue = MakeUnique<CommandQueue>(device, graphics::CommandType::Direct);

    VERIFY(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocator)));
    VERIFY(device->CreateCommandList(0, type, m_commandAllocator, nullptr,
                                     IID_PPV_ARGS(&m_commandList)));

    m_commandList->Close();
    return true;
}

void RenderContext::BeginRender()
{
    m_commandQueue->WaitForFence(m_commandQueue->GetCurrentFence());
    m_currentBuffer = interface->GetSwapChain()->GetCurrentBuffer();

    VERIFY(m_commandAllocator->Reset(), "failed to reset command allocator");
    VERIFY(m_commandList->Reset(m_commandAllocator, nullptr), "failed to reset command list");

    D3D12_RESOURCE_TRANSITION_BARRIER transition{};
    transition.pResource = m_currentBuffer->resource;
    transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition = transition;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuOffset = m_currentBuffer->descriptor.offset;

    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->ClearRenderTargetView(cpuOffset, Color::Clear().data, 0, nullptr);
}

void RenderContext::EndRender()
{
    D3D12_RESOURCE_TRANSITION_BARRIER transition{};
    transition.pResource = m_currentBuffer->resource;
    transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition = transition;

    m_commandList->ResourceBarrier(1, &barrier);
}

void RenderContext::SetViewport(Rect const& rect, float32 minZ, float32 maxZ)
{
    D3D12_VIEWPORT d3dViewport{};
    d3dViewport.TopLeftX = rect.x;
    d3dViewport.TopLeftY = rect.y;
    d3dViewport.Width = rect.width;
    d3dViewport.Height = rect.height;
    d3dViewport.MinDepth = minZ;
    d3dViewport.MaxDepth = maxZ;

    m_commandList->RSSetViewports(1, &d3dViewport);
}

void RenderContext::SetScissorRect(RectInt const& rect)
{
    m_commandList->RSSetScissorRects(1, reinterpret_cast<D3D12_RECT const*>(&rect));
}

void RenderContext::WaitForIdle()
{
    m_commandQueue->WaitForIdle();
}

void RenderContext::Execute()
{
    m_commandList->Close();
    m_commandQueue->ExecuteCommandList(m_commandList);
}

} // namespace mini::d3d12