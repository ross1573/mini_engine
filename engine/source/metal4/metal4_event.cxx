module;

#include <Metal/MTLEvent.hpp>

export module mini.metal4:event;

import mini.core;
import mini.apple;

export namespace MTL {

using MTL::Event;
using MTL::SharedEvent;

}; // namespace MTL

namespace mini::metal4 {

class Device;

} // namespace mini::metal4

namespace mini::metal4 {

export class METAL4_API Event {
protected:
    SharedPtr<MTL::Event> m_event;

protected:
    Event() noexcept = default;
    Event(SharedPtr<MTL::Event> const& event) noexcept;
    Event(SharedPtr<MTL::Event>&& event) noexcept;

public:
    explicit Event(PtrView<Device> device);

    void SetName(StringView name) { m_event->setLabel(ToNSString(name).Get()); }

    [[nodiscard]] bool Valid() const noexcept { return m_event.Valid(); }
    [[nodiscard]] String Name() const { return ToString(m_event->label()); }

    [[nodiscard]] MTL::Event* MTLEvent() const noexcept { return m_event.Get(); }
};

export class METAL4_API SharedEvent final : public Event {
private:
    uint64 m_signaledValue;

public:
    explicit SharedEvent(PtrView<Device> device);

    void Wait(uint64 value, Milliseconds timeout = Milliseconds::Max());
    bool Signal(uint64 value);

    [[nodiscard]] uint64 SignaledValue();

    [[nodiscard]] MTL::SharedEvent* MTLSharedEvent() const noexcept;
};

MTL::SharedEvent* SharedEvent::MTLSharedEvent() const noexcept
{
    return static_cast<MTL::SharedEvent*>(m_event.Get());
}

} // namespace mini::metal4