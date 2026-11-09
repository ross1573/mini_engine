export module mini.d3d12:event;

import mini.core;
import mini.windows;
import :common;

D3D12_EXPORT(ID3D12Fence);

namespace mini::d3d12 {

class Device;

} // namespace mini::d3d12

namespace mini::d3d12 {

export class D3D12_API Event {
protected:
    SharedPtr<ID3D12Fence> m_fence;
    uint64 m_signaledValue;

public:
    explicit Event(PtrView<Device> device);

    bool signal(uint64 value);

    [[nodiscard]] bool valid() const noexcept { return m_fence.valid(); }
    [[nodiscard]] uint64 signaledValue();

    [[nodiscard]] ID3D12Fence* d3d12Fence() const noexcept { return m_fence.get(); }
};

export class D3D12_API SharedEvent final : public Event {
private:
    HANDLE m_eventHandle;

public:
    explicit SharedEvent(PtrView<Device> device);
    ~SharedEvent() noexcept;

    void wait(uint64 value, Milliseconds timeout = Milliseconds::max());

    [[nodiscard]] HANDLE eventHandle() const noexcept { return m_eventHandle; }
};

} // namespace mini::d3d12