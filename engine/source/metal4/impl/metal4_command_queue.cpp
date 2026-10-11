module mini.metal4;

import :renderer;
import :command_queue;

namespace mini::metal4 {

CommandQueue::CommandQueue(PtrView<Device> device)
    : m_event(device)
    , m_eventValue(0)
{
    ASSERT(device);

    m_commandQueue = transfer(device->mtlDevice()->newMTL4CommandQueue());
}

void CommandQueue::commit(PtrView<CommandBuffer const> commandBuffer)
{
    ASSERT(commandBuffer);

    MTL4::CommandBuffer* buffer[] = {commandBuffer->mtlCommandBuffer()};

    if (options::gpuValidation) {
        SharedPtr<MTL4::CommitOptions> option = feedbackHandlerOption();
        m_commandQueue->commit(buffer, 1, option.get());
    } else {
        m_commandQueue->commit(buffer, 1);
    }

    m_commandQueue->signalEvent(m_event.mtlEvent(), ++m_eventValue);
}

void CommandQueue::commit(ArrayView<CommandBuffer const*> commandBuffers)
{
    if (commandBuffers.empty()) [[unlikely]] {
        return;
    }

    m_commitBuffer.reserve(commandBuffers.size());
    for (auto const& cmdBuf : commandBuffers) {
        ASSERT(cmdBuf);
        m_commitBuffer.pushBack(cmdBuf->mtlCommandBuffer());
    }

    if (options::gpuValidation) {
        SharedPtr<MTL4::CommitOptions> option = feedbackHandlerOption();
        m_commandQueue->commit(m_commitBuffer.data(), m_commitBuffer.size(), option.get());
    } else {
        m_commandQueue->commit(m_commitBuffer.data(), m_commitBuffer.size());
    }

    m_commandQueue->signalEvent(m_event.mtlEvent(), ++m_eventValue);
    m_commitBuffer.clear();
}

void CommandQueue::addResidencySet(PtrView<MTL::ResidencySet const> residencySet)
{
    m_commandQueue->addResidencySet(residencySet);
}

void CommandQueue::wait(PtrView<Drawable const> drawable)
{
    ASSERT(drawable);

    m_commandQueue->wait(drawable->mtlDrawable());
}

void CommandQueue::wait(PtrView<CommandQueue const> other)
{
    ASSERT(other);

    m_commandQueue->wait(other->m_event.mtlEvent(), other->m_eventValue);
}

void CommandQueue::signal(PtrView<Drawable const> drawable)
{
    ASSERT(drawable);

    m_commandQueue->signalDrawable(drawable->mtlDrawable());
}

void CommandQueue::signal(PtrView<Event const> event, uint64 value)
{
    ASSERT(event);

    m_commandQueue->signalEvent(event->mtlEvent(), value);
}

SharedPtr<MTL4::CommitOptions> CommandQueue::feedbackHandlerOption()
{
    SharedPtr<MTL4::CommitOptions> options = transfer(MTL4::CommitOptions::alloc()->init());
    options->addFeedbackHandler(^(MTL4::CommitFeedback* feedback) { Renderer::handleRenderError(feedback->error()); });
    return options;
}

} // namespace mini::metal4