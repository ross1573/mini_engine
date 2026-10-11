module mini.metal4;

import :event;

namespace mini::metal4 {

Event::Event(SharedPtr<MTL::Event> const& event) noexcept
    : m_event(event)
{
}

Event::Event(SharedPtr<MTL::Event>&& event) noexcept
    : m_event(moveCast(event))
{
}

Event::Event(PtrView<Device> device)
{
    ASSERT(device);
    m_event = transfer(device->mtlDevice()->newEvent());
}

SharedEvent::SharedEvent(PtrView<Device> device)
    : m_signaledValue(0)
{
    ASSERT(device);
    m_event = transfer<MTL::Event>(device->mtlDevice()->newSharedEvent());
}

void SharedEvent::wait(uint64 value, Milliseconds timeout)
{
    if (m_signaledValue >= value) {
        return;
    }

    m_signaledValue = mtlSharedEvent()->signaledValue();
    if (m_signaledValue >= value) {
        return;
    }

    size_t tick = static_cast<size_t>(timeout.count());
    mtlSharedEvent()->waitUntilSignaledValue(value, tick);
}

bool SharedEvent::signal(uint64 value)
{
    if (m_signaledValue >= value) {
        return false;
    }

    mtlSharedEvent()->setSignaledValue(value);
    m_signaledValue = value;
    return true;
}

uint64 SharedEvent::signaledValue()
{
    m_signaledValue = mtlSharedEvent()->signaledValue();
    return m_signaledValue;
}

} // namespace mini::metal4