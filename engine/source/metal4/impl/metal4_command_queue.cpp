module mini.metal4;

import :renderer;
import :command_queue;

namespace mini::metal4 {

CommandQueue::CommandQueue(PtrView<Device> device)
    : m_event(device)
    , m_eventValue(0)
{
    ASSERT(device);

    m_commandQueue = TransferShared(device->MTLDevice()->newMTL4CommandQueue());
}

void CommandQueue::Commit(PtrView<CommandBuffer const> commandBuffer)
{
    ASSERT(commandBuffer);

    MTL4::CommandBuffer* buffer[] = {commandBuffer->MTLCommandBuffer()};

    if (options::gpuValidation) {
        SharedPtr<MTL4::CommitOptions> option = FeedbackHandlerOption();
        m_commandQueue->commit(buffer, 1, option.Get());
    } else {
        m_commandQueue->commit(buffer, 1);
    }

    m_commandQueue->signalEvent(m_event.MTLEvent(), ++m_eventValue);
}

void CommandQueue::Commit(ArrayView<CommandBuffer const*> commandBuffers)
{
    if (commandBuffers.Size() == 0) [[unlikely]] {
        return;
    }

    m_commitBuffer.Reserve(commandBuffers.Size());
    for (auto const& cmdBuf : commandBuffers) {
        ASSERT(cmdBuf);
        m_commitBuffer.PushBack(cmdBuf->MTLCommandBuffer());
    }

    if (options::gpuValidation) {
        SharedPtr<MTL4::CommitOptions> option = FeedbackHandlerOption();
        m_commandQueue->commit(m_commitBuffer.Data(), m_commitBuffer.Size(), option.Get());
    } else {
        m_commandQueue->commit(m_commitBuffer.Data(), m_commitBuffer.Size());
    }

    m_commandQueue->signalEvent(m_event.MTLEvent(), ++m_eventValue);
    m_commitBuffer.Clear();
}

void CommandQueue::AddResidencySet(PtrView<MTL::ResidencySet const> residencySet)
{
    m_commandQueue->addResidencySet(residencySet);
}

void CommandQueue::Wait(PtrView<Drawable const> drawable)
{
    ASSERT(drawable);

    m_commandQueue->wait(drawable->MTLDrawable());
}

void CommandQueue::Wait(PtrView<CommandQueue const> other)
{
    ASSERT(other);

    m_commandQueue->wait(other->m_event.MTLEvent(), other->m_eventValue);
}

void CommandQueue::Signal(PtrView<Drawable const> drawable)
{
    ASSERT(drawable);

    m_commandQueue->signalDrawable(drawable->MTLDrawable());
}

void CommandQueue::Signal(PtrView<Event const> event, uint64 value)
{
    ASSERT(event);

    m_commandQueue->signalEvent(event->MTLEvent(), value);
}

SharedPtr<MTL4::CommitOptions> CommandQueue::FeedbackHandlerOption()
{
    SharedPtr<MTL4::CommitOptions> options = TransferShared(MTL4::CommitOptions::alloc()->init());
    options->addFeedbackHandler(^(MTL4::CommitFeedback* feedback) { Renderer::HandleRenderError(feedback->error()); });
    return options;
}

} // namespace mini::metal4