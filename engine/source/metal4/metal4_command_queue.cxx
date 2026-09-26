module;

#include <Metal/MTL4CommandQueue.hpp>
#include <Metal/MTL4CommitFeedback.hpp>

export module mini.metal4:command_queue;

import mini.core;
import mini.apple;
import :device;
import :event;
import :command_buffer;
import :swap_chain;

export namespace MTL4 {

using MTL4::CommandQueue;
using MTL4::CommitFeedback;
using MTL4::CommitOptions;

} // namespace MTL4

namespace mini::metal4 {

export class METAL4_API CommandQueue {
private:
    Array<MTL4::CommandBuffer const*> m_commitBuffer;
    SharedPtr<MTL4::CommandQueue> m_commandQueue;
    Event m_event;
    uint64 m_eventValue;

public:
    explicit CommandQueue(Device* device);

    void Commit(CommandBuffer const* commandBuffer);
    void Commit(ArrayView<CommandBuffer const*> commandBuffers);

    void Wait(Drawable* drawable);
    void Wait(CommandQueue const* other);
    void Signal(Drawable* drawable);
    void Signal(Event const* event, uint64 value);

    [[nodiscard]] bool Valid() const noexcept { return m_commandQueue.Valid(); }
    [[nodiscard]] uint64 EventValue() const noexcept { return m_eventValue; }

    [[nodiscard]] MTL4::CommandQueue* MTLCommandQueue() const noexcept { return m_commandQueue.Get(); }
    [[nodiscard]] MTL::Event* MTLEvent() const noexcept { return m_event.MTLEvent(); }

private:
    static SharedPtr<MTL4::CommitOptions> FeedbackHandlerOption();
    static void HandleCommitFeedback(MTL4::CommitFeedback* feedback);
};

} // namespace mini::metal4