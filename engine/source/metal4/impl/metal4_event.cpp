module mini.metal4;

import :event;

namespace mini::metal4 {

Event::Event(SharedPtr<MTL::Event> const& event) noexcept
    : m_event(event)
{
}

Event::Event(SharedPtr<MTL::Event>&& event) noexcept
    : m_event(MoveArg(event))
{
}

Event::Event(PtrView<Device> device)
{
    ASSERT(device);
    m_event = TransferShared(device->MTLDevice()->newEvent());
}

SharedEvent::SharedEvent(PtrView<Device> device)
    : m_signaledValue(0)
{
    ASSERT(device);
    m_event = TransferShared<MTL::Event>(device->MTLDevice()->newSharedEvent());
}

void SharedEvent::Wait(uint64 value, Milliseconds timeout)
{
    if (m_signaledValue >= value) {
        return;
    }

    m_signaledValue = MTLSharedEvent()->signaledValue();
    if (m_signaledValue >= value) {
        return;
    }

    size_t tick = static_cast<size_t>(timeout.Count());
    MTLSharedEvent()->waitUntilSignaledValue(value, tick);
}

bool SharedEvent::Signal(uint64 value)
{
    if (m_signaledValue >= value) {
        return false;
    }

    MTLSharedEvent()->setSignaledValue(value);
    m_signaledValue = value;
    return true;
}

uint64 SharedEvent::SignaledValue()
{
    m_signaledValue = MTLSharedEvent()->signaledValue();
    return m_signaledValue;
}

} // namespace mini::metal4