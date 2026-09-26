module;

#include <Metal/MTLEvent.hpp>

export module mini.metal4:event;

import mini.core;
import mini.apple;
import :device;

export namespace MTL {

using MTL::Event;
using MTL::SharedEvent;

}; // namespace MTL

namespace mini::metal4 {

export class METAL4_API Event {
protected:
    SharedPtr<MTL::Event> m_event;

protected:
    Event() noexcept = default;
    Event(SharedPtr<MTL::Event> const& event) noexcept;
    Event(SharedPtr<MTL::Event>&& event) noexcept;

public:
    explicit Event(Device* device);

    void SetName(StringView name);

    [[nodiscard]] bool Valid() const noexcept { return m_event.Valid(); }
    [[nodiscard]] String Name() const;

    [[nodiscard]] MTL::Event* MTLEvent() const noexcept { return m_event.Get(); }
};

Event::Event(SharedPtr<MTL::Event> const& event) noexcept
    : m_event(event)
{
}

Event::Event(SharedPtr<MTL::Event>&& event) noexcept
    : m_event(MoveArg(event))
{
}

Event::Event(Device* device)
{
    ASSERT(device);
    m_event = TransferShared(device->MTLDevice()->newEvent());
}

void Event::SetName(StringView name)
{
    m_event->setLabel(ToNSString(name).Get());
}

String Event::Name() const
{
    return ToString(m_event->label());
}

export class METAL4_API SharedEvent final : public Event {
private:
    uint64 m_lastValue;

public:
    explicit SharedEvent(Device* device);

    void Wait(uint64 value, Milliseconds timeout = Milliseconds::Max());

    [[nodiscard]] uint64 SignaledValue();

    [[nodiscard]] MTL::SharedEvent* MTLSharedEvent() const noexcept;
};

SharedEvent::SharedEvent(Device* device)
    : m_lastValue(0)
{
    ASSERT(device);
    m_event = TransferShared<MTL::Event>(device->MTLDevice()->newSharedEvent());
}

void SharedEvent::Wait(uint64 value, Milliseconds timeout)
{
    if (m_lastValue >= value) {
        return;
    }

    m_lastValue = MTLSharedEvent()->signaledValue();
    if (m_lastValue >= value) {
        return;
    }

    size_t tick = static_cast<size_t>(timeout.Count());
    MTLSharedEvent()->waitUntilSignaledValue(value, tick);
}

uint64 SharedEvent::SignaledValue()
{
    m_lastValue = MTLSharedEvent()->signaledValue();
    return m_lastValue;
}

MTL::SharedEvent* SharedEvent::MTLSharedEvent() const noexcept
{
    return static_cast<MTL::SharedEvent*>(m_event.Get());
}

} // namespace mini::metal4