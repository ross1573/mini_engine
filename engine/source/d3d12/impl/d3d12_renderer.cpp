module mini.d3d12;

import :device;
import :renderer;
import :swap_chain;

namespace mini::d3d12 {

Renderer::Renderer(PtrView<Device> device)
    : m_device(device)
    , m_frameValue(0)
{
    m_commandQueue = makeUnique<CommandQueue>(device, CommandType::direct);
    m_commandBufferPool = makeUnique<CommandBufferPool>(device, m_commandQueue, size_t{1}, options::bufferCount);
}

Renderer::~Renderer()
{
    waitIdle();
}

void Renderer::prepare()
{
}

void Renderer::render()
{
    m_commandBufferPool->wait(m_frameValue);

    SwapChainBuffer* currentBuffer = interface->swapChain()->GetCurrentBuffer();
    UniquePtr<CommandBuffer> commandBuffer = m_commandBufferPool->allocate();

    // begin
    {
        D3D12_RESOURCE_TRANSITION_BARRIER transition{};
        transition.pResource = currentBuffer->resource;
        transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition = transition;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuOffset = currentBuffer->descriptor.offset;

        commandBuffer->d3d12CommandList()->ResourceBarrier(1, &barrier);
        commandBuffer->d3d12CommandList()->ClearRenderTargetView(cpuOffset, Color::clear().data, 0, nullptr);
    }

    // end
    {
        D3D12_RESOURCE_TRANSITION_BARRIER transition{};
        transition.pResource = currentBuffer->resource;
        transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition = transition;

        commandBuffer->d3d12CommandList()->ResourceBarrier(1, &barrier);
    }

    m_frameValue = m_commandBufferPool->commit(moveCast(commandBuffer));
}

void Renderer::waitIdle()
{
    // TODO
    SharedEvent sharedEvent(m_device);
    m_commandQueue->signal(addressof(sharedEvent), 1);
    sharedEvent.wait(1);
}

} // namespace mini::d3d12