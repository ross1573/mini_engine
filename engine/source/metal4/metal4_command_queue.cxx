module;

#include <Metal/MTL4CommandQueue.hpp>
#include <Metal/MTL4CommitFeedback.hpp>
#include <Metal/MTLResidencySet.hpp>

export module mini.metal4:command_queue;

import mini.core;
import mini.apple;
import :event;

export namespace MTL4 {

using MTL4::CommandQueue;
using MTL4::CommitFeedback;
using MTL4::CommitOptions;

} // namespace MTL4

export namespace MTL {

using MTL::ResidencySet;

} // namespace MTL

namespace mini::metal4 {

class Device;
class Drawable;
class CommandBuffer;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API CommandQueue {
private:
    Array<MTL4::CommandBuffer const*> m_commitBuffer;
    SharedPtr<MTL4::CommandQueue> m_commandQueue;
    Event m_event;
    uint64 m_eventValue;

public:
    explicit CommandQueue(PtrView<Device> device);

    void commit(PtrView<CommandBuffer const> commandBuffer);
    void commit(ArrayView<CommandBuffer const*> commandBuffers);

    void addResidencySet(PtrView<MTL::ResidencySet const> residencySet);

    void wait(PtrView<Drawable const> drawable);
    void wait(PtrView<CommandQueue const> other);
    void signal(PtrView<Drawable const> drawable);
    void signal(PtrView<Event const> event, uint64 value);

    [[nodiscard]] bool valid() const noexcept { return m_commandQueue.valid(); }
    [[nodiscard]] uint64 eventValue() const noexcept { return m_eventValue; }

    [[nodiscard]] MTL4::CommandQueue* mtlCommandQueue() const noexcept { return m_commandQueue.get(); }
    [[nodiscard]] MTL::Event* mtlEvent() const noexcept { return m_event.mtlEvent(); }

private:
    static SharedPtr<MTL4::CommitOptions> feedbackHandlerOption();
    static void handleCommitFeedback(MTL4::CommitFeedback* feedback);
};

} // namespace mini::metal4