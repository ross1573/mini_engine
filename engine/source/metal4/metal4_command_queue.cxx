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

    void Commit(PtrView<CommandBuffer const> commandBuffer);
    void Commit(ArrayView<CommandBuffer const*> commandBuffers);

    void AddResidencySet(PtrView<MTL::ResidencySet const> residencySet);

    void Wait(PtrView<Drawable const> drawable);
    void Wait(PtrView<CommandQueue const> other);
    void Signal(PtrView<Drawable const> drawable);
    void Signal(PtrView<Event const> event, uint64 value);

    [[nodiscard]] bool Valid() const noexcept { return m_commandQueue.Valid(); }
    [[nodiscard]] uint64 EventValue() const noexcept { return m_eventValue; }

    [[nodiscard]] MTL4::CommandQueue* MTLCommandQueue() const noexcept { return m_commandQueue.Get(); }
    [[nodiscard]] MTL::Event* MTLEvent() const noexcept { return m_event.MTLEvent(); }

private:
    static SharedPtr<MTL4::CommitOptions> FeedbackHandlerOption();
    static void HandleCommitFeedback(MTL4::CommitFeedback* feedback);
};

} // namespace mini::metal4