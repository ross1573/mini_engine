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

    void setName(StringView name) { m_event->setLabel(toNSString(name).get()); }

    [[nodiscard]] bool valid() const noexcept { return m_event.valid(); }
    [[nodiscard]] String name() const { return toString(m_event->label()); }

    [[nodiscard]] MTL::Event* mtlEvent() const noexcept { return m_event.get(); }
};

export class METAL4_API SharedEvent final : public Event {
private:
    uint64 m_signaledValue;

public:
    explicit SharedEvent(PtrView<Device> device);

    void wait(uint64 value, Milliseconds timeout = Milliseconds::max());
    bool signal(uint64 value);

    [[nodiscard]] uint64 signaledValue();

    [[nodiscard]] MTL::SharedEvent* mtlSharedEvent() const noexcept;
};

MTL::SharedEvent* SharedEvent::mtlSharedEvent() const noexcept
{
    return static_cast<MTL::SharedEvent*>(m_event.get());
}

} // namespace mini::metal4